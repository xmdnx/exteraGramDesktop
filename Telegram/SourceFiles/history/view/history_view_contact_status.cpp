/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#include "history/view/history_view_contact_status.h"

#include "lang/lang_keys.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/padding_wrap.h"
#include "ui/layers/generic_box.h"
#include "ui/toast/toast.h"
#include "ui/text/format_values.h" // Ui::FormatPhone
#include "ui/text/text_utilities.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "core/ui_integration.h"
#include "data/notify/data_notify_settings.h"
#include "data/data_peer.h"
#include "data/data_user.h"
#include "data/data_chat.h"
#include "data/data_channel.h"
#include "data/data_changes.h"
#include "data/data_document.h"
#include "data/data_session.h"
#include "data/data_forum_topic.h"
#include "data/data_peer_values.h"
#include "data/stickers/data_custom_emoji.h"
#include "settings/settings_premium.h"
#include "window/window_peer_menu.h"
#include "window/window_controller.h"
#include "window/window_session_controller.h"
#include "apiwrap.h"
#include "api/api_blocked_peers.h"
#include "main/main_session.h"
#include "base/unixtime.h"
#include "boxes/peers/edit_contact_box.h"
#include "styles/style_chat.h"
#include "styles/style_layers.h"
#include "styles/style_info.h"
#include "styles/style_menu_icons.h"

namespace HistoryView {
namespace {

[[nodiscard]] bool BarCurrentlyHidden(not_null<PeerData*> peer) {
	const auto settings = peer->settings();
	if (!settings) {
		return false;
	} else if (!(*settings)) {
		return true;
	}
	if (const auto user = peer->asUser()) {
		if (user->isBlocked()) {
			return true;
		} else if (user->isContact()
			&& !((*settings) & PeerSetting::ShareContact)) {
			return true;
		}
	} else if (!((*settings) & PeerSetting::ReportSpam)) {
		return true;
	}
	return false;
}

[[nodiscard]] rpl::producer<TextWithEntities> ResolveIsCustom(
		not_null<Data::Session*> owner,
		DocumentId id) {
	return owner->customEmojiManager().resolve(
		id
	) | rpl::map([=](not_null<DocumentData*> document) {
		const auto sticker = document->sticker();
		Assert(sticker != nullptr);

		const auto &manager = document->owner().customEmojiManager();
		const auto setId = manager.coloredSetId();
		const auto text = (setId == sticker->set.id)
			? QString()
			: sticker->alt;
		if (text.isEmpty()) {
			return TextWithEntities();
		}
		return TextWithEntities{
			.text = text,
			.entities = { EntityInText(
				EntityType::CustomEmoji,
				0,
				text.size(),
				Data::SerializeCustomEmojiId(document)) },
		};
	});
}

[[nodiscard]] rpl::producer<TextWithEntities> PeerCustomStatus(
		not_null<PeerData*> peer) {
	const auto user = peer->asUser();
	if (!user) {
		return rpl::single(TextWithEntities());
	}
	const auto owner = &user->owner();
	return user->session().changes().peerFlagsValue(
		user,
		Data::PeerUpdate::Flag::EmojiStatus
	) | rpl::map([=] {
		const auto id = user->emojiStatusId();
		return id
			? ResolveIsCustom(owner, id)
			: rpl::single(TextWithEntities());
	}) | rpl::flatten_latest() | rpl::distinct_until_changed();
}

[[nodiscard]] object_ptr<Ui::AbstractButton> MakeIconButton(
		QWidget *parent,
		const style::icon &icon) {
	auto result = object_ptr<Ui::RippleButton>(
		parent,
		st::historyContactStatusButton.ripple);
	const auto raw = result.data();
	raw->paintRequest(
	) | rpl::start_with_next([=, &icon] {
		auto p = QPainter(raw);
		p.fillRect(raw->rect(), st::historyContactStatusButton.bgColor);
		raw->paintRipple(p, 0, 0);
		icon.paintInCenter(p, raw->rect());
	}, raw->lifetime());
	return result;
}

} // namespace

class ContactStatus::BgButton final : public Ui::RippleButton {
public:
	BgButton(QWidget *parent, const style::FlatButton &st);

protected:
	void paintEvent(QPaintEvent *e) override;

	void onStateChanged(State was, StateChangeSource source) override;

private:
	const style::FlatButton &_st;

};

class ContactStatus::Bar final : public Ui::RpWidget {
public:
	Bar(QWidget *parent, const QString &name);

	void showState(
		State state,
		TextWithEntities status,
		Fn<std::any(Fn<void()> customEmojiRepaint)> context);

	[[nodiscard]] rpl::producer<> unarchiveClicks() const;
	[[nodiscard]] rpl::producer<> addClicks() const;
	[[nodiscard]] rpl::producer<> blockClicks() const;
	[[nodiscard]] rpl::producer<> shareClicks() const;
	[[nodiscard]] rpl::producer<> reportClicks() const;
	[[nodiscard]] rpl::producer<> closeClicks() const;
	[[nodiscard]] rpl::producer<> requestInfoClicks() const;
	[[nodiscard]] rpl::producer<> emojiStatusClicks() const;

private:
	int resizeGetHeight(int newWidth) override;

	void emojiStatusRepaint();

	QString _name;
	object_ptr<Ui::FlatButton> _add;
	object_ptr<Ui::FlatButton> _unarchive;
	object_ptr<Ui::AbstractButton> _unarchiveIcon;
	object_ptr<Ui::FlatButton> _block;
	object_ptr<Ui::FlatButton> _share;
	object_ptr<Ui::FlatButton> _report;
	object_ptr<Ui::AbstractButton> _reportIcon;
	object_ptr<Ui::IconButton> _close;
	object_ptr<BgButton> _requestChatBg;
	object_ptr<Ui::FlatLabel> _requestChatInfo;
	object_ptr<Ui::PaddingWrap<Ui::FlatLabel>> _emojiStatusInfo;
	object_ptr<Ui::PlainShadow> _emojiStatusShadow;
	bool _emojiStatusRepaintScheduled = false;
	bool _narrow = false;
	rpl::event_stream<> _emojiStatusClicks;

};

ContactStatus::BgButton::BgButton(
	QWidget *parent,
	const style::FlatButton &st)
: RippleButton(parent, st.ripple)
, _st(st) {
}

void ContactStatus::BgButton::onStateChanged(
		State was,
		StateChangeSource source) {
	RippleButton::onStateChanged(was, source);
	update();
}

void ContactStatus::BgButton::paintEvent(QPaintEvent *e) {
	QPainter p(this);

	p.fillRect(e->rect(), isOver() ? _st.overBgColor : _st.bgColor);
	paintRipple(p, 0, 0);
}

ContactStatus::Bar::Bar(
	QWidget *parent,
	const QString &name)
: RpWidget(parent)
, _name(name)
, _add(
	this,
	QString(),
	st::historyContactStatusButton)
, _unarchive(
	this,
	tr::lng_new_contact_unarchive(tr::now).toUpper(),
	st::historyContactStatusButton)
, _unarchiveIcon(MakeIconButton(this, st::menuIconUnarchive))
, _block(
	this,
	tr::lng_new_contact_block(tr::now).toUpper(),
	st::historyContactStatusBlock)
, _share(
	this,
	tr::lng_new_contact_share(tr::now).toUpper(),
	st::historyContactStatusButton)
, _report(
	this,
	QString(),
	st::historyContactStatusBlock)
, _reportIcon(MakeIconButton(this, st::menuIconReportAttention))
, _close(this, st::historyReplyCancel)
, _requestChatBg(this, st::historyContactStatusButton)
, _requestChatInfo(
	this,
	QString(),
	st::historyContactStatusLabel)
, _emojiStatusInfo(
	this,
	object_ptr<Ui::FlatLabel>(this, u""_q, st::historyEmojiStatusInfoLabel),
	QMargins(
		st::historyContactStatusMinSkip,
		st::topBarArrowPadding.top(),
		st::historyContactStatusMinSkip,
		st::topBarArrowPadding.top()))
, _emojiStatusShadow(this) {
	_requestChatInfo->setAttribute(Qt::WA_TransparentForMouseEvents);
	_emojiStatusInfo->paintRequest(
	) | rpl::start_with_next([=, raw = _emojiStatusInfo.data()](QRect clip) {
		_emojiStatusRepaintScheduled = false;
		QPainter(raw).fillRect(clip, st::historyComposeButtonBg);
	}, lifetime());
}

void ContactStatus::Bar::showState(
		State state,
		TextWithEntities status,
		Fn<std::any(Fn<void()> customEmojiRepaint)> context) {
	using Type = State::Type;
	const auto type = state.type;
	_add->setVisible(type == Type::AddOrBlock || type == Type::Add);
	const auto unarchive = (type == Type::UnarchiveOrBlock)
		|| (type == Type::UnarchiveOrReport);
	_unarchive->setVisible(!_narrow && unarchive);
	_unarchiveIcon->setVisible(_narrow && unarchive);
	_block->setVisible(type == Type::AddOrBlock
		|| type == Type::UnarchiveOrBlock);
	_share->setVisible(type == Type::SharePhoneNumber);
	_close->setVisible(!_narrow && type != Type::RequestChatInfo);
	const auto report = (type == Type::ReportSpam)
		|| (type == Type::UnarchiveOrReport);
	_report->setVisible(!_narrow && report);
	_reportIcon->setVisible(_narrow && report);
	_requestChatInfo->setVisible(type == Type::RequestChatInfo);
	_requestChatBg->setVisible(type == Type::RequestChatInfo);
	const auto has = !status.empty();
	_emojiStatusShadow->setVisible(
		has && (type == Type::AddOrBlock || type == Type::UnarchiveOrBlock));
	if (has) {
		_emojiStatusInfo->entity()->setMarkedText(
			tr::lng_new_contact_about_status(
				tr::now,
				lt_emoji,
				status,
				lt_link,
				Ui::Text::Link(
					tr::lng_new_contact_about_status_link(tr::now)),
				Ui::Text::WithEntities),
			context([=] { emojiStatusRepaint(); }));
		_emojiStatusInfo->entity()->overrideLinkClickHandler([=] {
			_emojiStatusClicks.fire({});
		});
	}
	_emojiStatusInfo->setVisible(has);
	_add->setText((type == Type::Add)
		? tr::lng_new_contact_add_name(tr::now, lt_user, _name).toUpper()
		: tr::lng_new_contact_add(tr::now).toUpper());
	_report->setText((type == Type::ReportSpam)
		? tr::lng_report_spam_and_leave(tr::now).toUpper()
		: tr::lng_report_spam(tr::now).toUpper());
	_requestChatInfo->setMarkedText(
		(state.requestChatIsBroadcast
			? tr::lng_new_contact_from_request_channel
			: tr::lng_new_contact_from_request_group)(
			tr::now,
			lt_user,
			Ui::Text::Bold(_name),
			lt_name,
			Ui::Text::Bold(state.requestChatName),
			Ui::Text::WithEntities));
	resizeToWidth(width());
}

rpl::producer<> ContactStatus::Bar::unarchiveClicks() const {
	return rpl::merge(
		_unarchive->clicks(),
		_unarchiveIcon->clicks()
	) | rpl::to_empty;
}

rpl::producer<> ContactStatus::Bar::addClicks() const {
	return _add->clicks() | rpl::to_empty;
}

rpl::producer<> ContactStatus::Bar::blockClicks() const {
	return _block->clicks() | rpl::to_empty;
}

rpl::producer<> ContactStatus::Bar::shareClicks() const {
	return _share->clicks() | rpl::to_empty;
}

rpl::producer<> ContactStatus::Bar::reportClicks() const {
	return rpl::merge(
		_report->clicks(),
		_reportIcon->clicks()
	) | rpl::to_empty;
}

rpl::producer<> ContactStatus::Bar::closeClicks() const {
	return _close->clicks() | rpl::to_empty;
}

rpl::producer<> ContactStatus::Bar::requestInfoClicks() const {
	return _requestChatBg->clicks() | rpl::to_empty;
}

rpl::producer<> ContactStatus::Bar::emojiStatusClicks() const {
	return _emojiStatusClicks.events();
}

int ContactStatus::Bar::resizeGetHeight(int newWidth) {
	_close->moveToRight(0, 0, newWidth);
	const auto narrow = (newWidth < _close->width() * 2);
	if (_narrow != narrow) {
		_narrow = narrow;
		_close->setVisible(_requestChatInfo->isHidden() && !_narrow);
		const auto report = !_report->isHidden() || !_reportIcon->isHidden();
		_report->setVisible(!_narrow && report);
		_reportIcon->setVisible(_narrow && report);
		const auto unarchive = !_unarchive->isHidden()
			|| !_unarchiveIcon->isHidden();
		_unarchive->setVisible(!_narrow && unarchive);
		_unarchiveIcon->setVisible(_narrow && unarchive);
	}

	if (!_unarchiveIcon->isHidden()) {
		const auto half = newWidth / 2;
		_unarchiveIcon->setGeometry(0, 0, half, _close->height());
		_reportIcon->setGeometry(half, 0, newWidth - half, _close->height());
	} else if (!_reportIcon->isHidden()) {
		_reportIcon->setGeometry(0, 0, newWidth, _close->height());
	}

	const auto closeWidth = _close->width();
	const auto closeHeight = _close->height();
	const auto available = newWidth - closeWidth;
	const auto skip = st::historyContactStatusMinSkip;
	if (available <= 2 * skip) {
		return closeHeight;
	}
	const auto buttonWidth = [&](const object_ptr<Ui::FlatButton> &button) {
		return button->textWidth() + 2 * skip;
	};

	auto accumulatedLeft = 0;
	const auto placeButton = [&](
			const object_ptr<Ui::FlatButton> &button,
			int buttonWidth,
			int rightTextMargin = 0) {
		button->setGeometry(accumulatedLeft, 0, buttonWidth, closeHeight);
		button->setTextMargins({ 0, 0, rightTextMargin, 0 });
		accumulatedLeft += buttonWidth;
	};
	const auto placeOne = [&](const object_ptr<Ui::FlatButton> &button) {
		if (button->isHidden()) {
			return;
		}
		const auto thatWidth = buttonWidth(button);
		const auto margin = std::clamp(
			thatWidth + closeWidth - available,
			0,
			closeWidth);
		placeButton(button, newWidth, margin);
	};
	const auto &leftButton = _unarchive->isHidden() ? _add : _unarchive;
	const auto &rightButton = _block->isHidden() ? _report : _block;
	if (!leftButton->isHidden() && !rightButton->isHidden()) {
		const auto leftWidth = buttonWidth(leftButton);
		const auto rightWidth = buttonWidth(rightButton);
		const auto half = newWidth / 2;
		if (leftWidth <= half
			&& rightWidth + 2 * closeWidth <= newWidth - half) {
			placeButton(leftButton, half);
			placeButton(rightButton, newWidth - half);
		} else if (leftWidth + rightWidth <= available) {
			const auto margin = std::clamp(
				leftWidth + rightWidth + closeWidth - available,
				0,
				closeWidth);
			const auto realBlockWidth = rightWidth + 2 * closeWidth - margin;
			if (leftWidth > realBlockWidth) {
				placeButton(leftButton, leftWidth);
				placeButton(rightButton, newWidth - leftWidth, margin);
			} else {
				placeButton(leftButton, newWidth - realBlockWidth);
				placeButton(rightButton, realBlockWidth, margin);
			}
		} else {
			const auto forLeft = (available * leftWidth)
				/ (leftWidth + rightWidth);
			placeButton(leftButton, forLeft);
			placeButton(rightButton, newWidth - forLeft, closeWidth);
		}
	} else {
		placeOne(_add);
		placeOne(_share);
		placeOne(_report);
	}
	if (_requestChatInfo->isHidden()) {
		_emojiStatusInfo->resizeToWidth(newWidth);
		_emojiStatusInfo->move(0, _close->height());
		_emojiStatusShadow->setGeometry(
			0,
			closeHeight,
			newWidth,
			st::lineWidth);
		_emojiStatusShadow->move(0, _close->height());
		return closeHeight + (_emojiStatusInfo->isHidden()
			? 0
			: _emojiStatusInfo->height());
	}
	const auto vskip = st::topBarArrowPadding.top();
	_requestChatInfo->resizeToWidth(available - 2 * skip);
	_requestChatInfo->move(skip, vskip);
	const auto newHeight = _requestChatInfo->height() + 2 * vskip;
	_requestChatBg->setGeometry(0, 0, newWidth, newHeight);
	return newHeight;
}

void ContactStatus::Bar::emojiStatusRepaint() {
	if (_emojiStatusRepaintScheduled) {
		return;
	}
	_emojiStatusRepaintScheduled = true;
	_emojiStatusInfo->entity()->update();
}

SlidingBar::SlidingBar(
	not_null<Ui::RpWidget*> parent,
	object_ptr<Ui::RpWidget> wrapped)
: _wrapped(parent, std::move(wrapped))
, _shadow(parent) {
	setup(parent);
	_wrapped.hide(anim::type::instant);
	_shadow.hide();
}

void SlidingBar::setup(not_null<Ui::RpWidget*> parent) {
	parent->widthValue(
	) | rpl::start_with_next([=](int width) {
		_wrapped.resizeToWidth(width);
	}, _wrapped.lifetime());

	_wrapped.geometryValue(
	) | rpl::start_with_next([=](QRect geometry) {
		_shadow.setGeometry(
			geometry.x(),
			geometry.y() + geometry.height(),
			geometry.width(),
			st::lineWidth);
	}, _shadow.lifetime());

	_shadow.showOn(rpl::combine(
		_wrapped.shownValue(),
		_wrapped.heightValue(),
		rpl::mappers::_1 && rpl::mappers::_2 > 0
	) | rpl::filter([=](bool shown) {
		return (shown == _shadow.isHidden());
	}));
}

void SlidingBar::toggleContent(bool visible) {
	_contentShown = visible;
	if (_shown) {
		_wrapped.toggle(visible, anim::type::normal);
	}
}

void SlidingBar::raise() {
	_wrapped.raise();
	_shadow.raise();
}

void SlidingBar::setVisible(bool visible) {
	_shown = visible;
	if (!_shown) {
		_wrapped.hide(anim::type::instant);
	} else if (_contentShown) {
		_wrapped.show(anim::type::instant);
	} else if (!_wrapped.isHidden() && !_wrapped.animating()) {
		_wrapped.hide(anim::type::instant);
	}
}

void SlidingBar::move(int x, int y) {
	_wrapped.move(x, y);
	_shadow.move(x, y + _wrapped.height());
}

int SlidingBar::height() const {
	return _wrapped.height();
}

rpl::producer<int> SlidingBar::heightValue() const {
	return _wrapped.heightValue();
}

ContactStatus::ContactStatus(
	not_null<Window::SessionController*> window,
	not_null<Ui::RpWidget*> parent,
	not_null<PeerData*> peer,
	bool showInForum)
: _controller(window)
, _inner(Ui::CreateChild<Bar>(parent.get(), peer->shortName()))
, _bar(parent, object_ptr<Bar>::fromRaw(_inner)) {
	setupState(peer, showInForum);
	setupHandlers(peer);
}

auto ContactStatus::PeerState(not_null<PeerData*> peer)
-> rpl::producer<State> {
	using SettingsChange = PeerData::Settings::Change;
	using Type = State::Type;
	if (const auto user = peer->asUser()) {
		using FlagsChange = UserData::Flags::Change;
		using Flag = UserDataFlag;

		auto changes = user->flagsValue(
		) | rpl::filter([](FlagsChange flags) {
			return flags.diff
				& (Flag::Contact | Flag::MutualContact | Flag::Blocked);
		});
		return rpl::combine(
			std::move(changes),
			user->settingsValue()
		) | rpl::map([=](
				FlagsChange flags,
				SettingsChange settings) -> State {
			if (flags.value & Flag::Blocked) {
				return { Type::None };
			} else if (user->isContact()) {
				if (settings.value & PeerSetting::ShareContact) {
					return { Type::SharePhoneNumber };
				} else {
					return { Type::None };
				}
			} else if (settings.value & PeerSetting::RequestChat) {
				return {
					.type = Type::RequestChatInfo,
					.requestChatName = peer->requestChatTitle(),
					.requestChatIsBroadcast = !!(settings.value
						& PeerSetting::RequestChatIsBroadcast),
					.requestDate = peer->requestChatDate(),
				};
			} else if (settings.value & PeerSetting::AutoArchived) {
				return { Type::UnarchiveOrBlock };
			} else if (settings.value & PeerSetting::BlockContact) {
				return { Type::AddOrBlock };
			} else if (settings.value & PeerSetting::AddContact) {
				return { Type::Add };
			} else {
				return { Type::None };
			}
		});
	}

	return peer->settingsValue(
	) | rpl::map([=](SettingsChange settings) {
		using Type = State::Type;
		return (settings.value & PeerSetting::AutoArchived)
			? State{ Type::UnarchiveOrReport }
			: (settings.value & PeerSetting::ReportSpam)
			? State{ Type::ReportSpam }
			: State{ Type::None };
	});
}

void ContactStatus::setupState(not_null<PeerData*> peer, bool showInForum) {
	if (!BarCurrentlyHidden(peer)) {
		peer->session().api().requestPeerSettings(peer);
	}

	_context = [=](Fn<void()> customEmojiRepaint) {
		return Core::MarkedTextContext{
			.session = &peer->session(),
			.customEmojiRepaint = customEmojiRepaint,
		};
	};
	_inner->showState({}, {}, _context);
	const auto channel = peer->asChannel();
	rpl::combine(
		PeerState(peer),
		PeerCustomStatus(peer),
		((channel && !showInForum)
			? Data::PeerFlagValue(channel, ChannelData::Flag::Forum)
			: (rpl::single(false) | rpl::type_erased()))
	) | rpl::start_with_next([=](
			State state,
			TextWithEntities status,
			bool hiddenByForum) {
		_state = state;
		_status = status;
		_hiddenByForum = hiddenByForum;
		if (state.type == State::Type::None || hiddenByForum) {
			_bar.toggleContent(false);
		} else {
			_inner->showState(state, std::move(status), _context);
			_bar.toggleContent(true);
		}
	}, _bar.lifetime());
}

void ContactStatus::setupHandlers(not_null<PeerData*> peer) {
	if (const auto user = peer->asUser()) {
		setupAddHandler(user);
		setupBlockHandler(user);
		setupShareHandler(user);
	}
	setupUnarchiveHandler(peer);
	setupReportHandler(peer);
	setupCloseHandler(peer);
	setupRequestInfoHandler(peer);
	setupEmojiStatusHandler(peer);
}

void ContactStatus::setupAddHandler(not_null<UserData*> user) {
	_inner->addClicks(
	) | rpl::start_with_next([=] {
		_controller->window().show(Box(EditContactBox, _controller, user));
	}, _bar.lifetime());
}

void ContactStatus::setupBlockHandler(not_null<UserData*> user) {
	_inner->blockClicks(
	) | rpl::start_with_next([=] {
		_controller->window().show(Box(
			Window::PeerMenuBlockUserBox,
			&_controller->window(),
			user,
			v::null,
			Window::ClearChat{}));
	}, _bar.lifetime());
}

void ContactStatus::setupShareHandler(not_null<UserData*> user) {
	_inner->shareClicks(
	) | rpl::start_with_next([=] {
		const auto show = _controller->uiShow();
		const auto share = [=](Fn<void()> &&close) {
			user->setSettings(0);
			user->session().api().request(MTPcontacts_AcceptContact(
				user->inputUser
			)).done([=](const MTPUpdates &result) {
				user->session().api().applyUpdates(result);
				show->showToast(tr::lng_new_contact_share_done(
					tr::now,
					lt_user,
					user->shortName()));
			}).send();
			close();
		};
		show->showBox(Ui::MakeConfirmBox({
			.text = tr::lng_new_contact_share_sure(
				tr::now,
				lt_phone,
				Ui::Text::WithEntities(
					Ui::FormatPhone(user->session().user()->phone())),
				lt_user,
				Ui::Text::Bold(user->name()),
				Ui::Text::WithEntities),
			.confirmed = share,
			.confirmText = tr::lng_box_ok(),
		}));
	}, _bar.lifetime());
}

void ContactStatus::setupUnarchiveHandler(not_null<PeerData*> peer) {
	_inner->unarchiveClicks(
	) | rpl::start_with_next([=] {
		Window::ToggleHistoryArchived(peer->owner().history(peer), false);
		peer->owner().notifySettings().resetToDefault(peer);
		if (const auto settings = peer->settings()) {
			const auto flags = PeerSetting::AutoArchived
				| PeerSetting::BlockContact
				| PeerSetting::ReportSpam;
			peer->setSettings(*settings & ~flags);
		}
	}, _bar.lifetime());
}

void ContactStatus::setupReportHandler(not_null<PeerData*> peer) {
	_inner->reportClicks(
	) | rpl::start_with_next([=] {
		Expects(!peer->isUser());

		const auto show = _controller->uiShow();
		const auto callback = crl::guard(_inner, [=](Fn<void()> &&close) {
			close();

			peer->session().api().request(MTPmessages_ReportSpam(
				peer->input
			)).send();

			crl::on_main(&peer->session(), [=] {
				if (const auto from = peer->migrateFrom()) {
					peer->session().api().deleteConversation(from, false);
				}
				peer->session().api().deleteConversation(peer, false);
			});

			show->showToast(tr::lng_report_spam_done(tr::now));

			// Destroys _bar.
			_controller->showBackFromStack();
		});
		if (const auto user = peer->asUser()) {
			peer->session().api().blockedPeers().block(user);
		}
		auto text = ((peer->isChat() || peer->isMegagroup())
			? tr::lng_report_spam_sure_group
			: tr::lng_report_spam_sure_channel)();
		show->showBox(Ui::MakeConfirmBox({
			.text= std::move(text),
			.confirmed = callback,
			.confirmText = tr::lng_report_spam_ok(),
			.confirmStyle = &st::attentionBoxButton,
		}));
	}, _bar.lifetime());
}

void ContactStatus::setupCloseHandler(not_null<PeerData*> peer) {
	const auto request = _bar.lifetime().make_state<mtpRequestId>(0);
	_inner->closeClicks(
	) | rpl::filter([=] {
		return !(*request);
	}) | rpl::start_with_next([=] {
		peer->setSettings(0);
		*request = peer->session().api().request(
			MTPmessages_HidePeerSettingsBar(peer->input)
		).send();
	}, _bar.lifetime());
}

void ContactStatus::setupRequestInfoHandler(not_null<PeerData*> peer) {
	const auto request = _bar.lifetime().make_state<mtpRequestId>(0);
	_inner->requestInfoClicks(
	) | rpl::filter([=] {
		return !(*request);
	}) | rpl::start_with_next([=] {
		_controller->show(Box([=](not_null<Ui::GenericBox*> box) {
			box->setTitle((_state.requestChatIsBroadcast
				? tr::lng_from_request_title_channel
				: tr::lng_from_request_title_group)());

			box->addRow(object_ptr<Ui::FlatLabel>(
				box,
				tr::lng_from_request_body(
					lt_name,
					rpl::single(Ui::Text::Bold(_state.requestChatName)),
					lt_date,
					rpl::single(langDateTimeFull(
						base::unixtime::parse(_state.requestDate)
					)) | Ui::Text::ToWithEntities(),
					Ui::Text::WithEntities),
				st::boxLabel));

			box->addButton(tr::lng_from_request_understand(), [=] {
				if (*request) {
					return;
				}
				peer->setSettings(0);
				*request = peer->session().api().request(
					MTPmessages_HidePeerSettingsBar(peer->input)
				).send();
				box->closeBox();
			});
		}));
	}, _bar.lifetime());
}

void ContactStatus::setupEmojiStatusHandler(not_null<PeerData*> peer) {
	_inner->emojiStatusClicks(
	) | rpl::start_with_next([=] {
		Settings::ShowEmojiStatusPremium(_controller, peer);
	}, _bar.lifetime());
}

void ContactStatus::show() {
	if (!_shown) {
		_shown = true;
		if (_state.type != State::Type::None && !_hiddenByForum) {
			_inner->showState(_state, _status, _context);
			_bar.toggleContent(true);
		}
	}
	_bar.show();
}

void ContactStatus::hide() {
	_bar.hide();
}

TopicReopenBar::TopicReopenBar(
	not_null<Ui::RpWidget*> parent,
	not_null<Data::ForumTopic*> topic)
: _topic(topic)
, _reopen(Ui::CreateChild<Ui::FlatButton>(
	parent.get(),
	tr::lng_forum_topic_reopen(tr::now),
	st::historyContactStatusButton))
, _bar(parent, object_ptr<Ui::FlatButton>::fromRaw(_reopen)) {
	setupState();
	setupHandler();
}

void TopicReopenBar::setupState() {
	const auto channel = _topic->channel();
	auto canToggle = (_topic->my() || channel->amCreator())
		? (rpl::single(true) | rpl::type_erased())
		: channel->adminRightsValue(
		) | rpl::map([=] { return _topic->canToggleClosed(); });

	rpl::combine(
		_topic->session().changes().topicFlagsValue(
			_topic,
			Data::TopicUpdate::Flag::Closed),
		std::move(canToggle)
	) | rpl::start_with_next([=](const auto &, bool can) {
		_bar.toggleContent(can && _topic->closed());
	}, _bar.lifetime());
}

void TopicReopenBar::setupHandler() {
	_reopen->setClickedCallback([=] {
		_topic->setClosedAndSave(false);
	});
}

} // namespace HistoryView
