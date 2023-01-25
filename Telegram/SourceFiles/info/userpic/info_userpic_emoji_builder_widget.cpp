/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "info/userpic/info_userpic_emoji_builder_widget.h"

#include "api/api_peer_photo.h"
#include "apiwrap.h"
#include "chat_helpers/emoji_list_widget.h"
#include "chat_helpers/stickers_list_widget.h"
#include "data/data_message_reactions.h"
#include "data/data_session.h"
#include "data/stickers/data_custom_emoji.h"
#include "editor/photo_editor_layer_widget.h" // Editor::kProfilePhotoSize.
#include "info/userpic/info_userpic_bubble_wrap.h"
#include "info/userpic/info_userpic_color_circle_button.h"
#include "info/userpic/info_userpic_emoji_builder_common.h"
#include "info/userpic/info_userpic_emoji_builder_preview.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "settings/settings_common.h"
#include "ui/controls/emoji_button.h"
#include "ui/empty_userpic.h"
#include "ui/painter.h"
#include "ui/rect.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/scroll_area.h"
#include "ui/wrap/padding_wrap.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_chat.h"
#include "styles/style_chat_helpers.h"
#include "styles/style_info_userpic_builder.h"
#include "styles/style_layers.h"
#include "styles/style_settings.h"
#include "styles/style_menu_icons.h"

namespace UserpicBuilder {
namespace {

void AlignChildren(not_null<Ui::RpWidget*> widget, int fullWidth) {
	const auto children = widget->children();
	const auto widgets = ranges::views::all(
		children
	) | ranges::views::filter([](not_null<const QObject*> object) {
		return object->isWidgetType();
	}) | ranges::views::transform([](not_null<QObject*> object) {
		return static_cast<QWidget*>(object.get());
	}) | ranges::to_vector;

	const auto widgetWidth = widgets.front()->width();
	const auto widgetsCount = widgets.size();
	const auto widgetsWidth = widgetWidth * widgetsCount;
	const auto step = (fullWidth - widgetsWidth) / (widgetsCount - 1);
	for (auto i = 0; i < widgetsCount; i++) {
		widgets[i]->move(i * (widgetWidth + step), widgets[i]->y());
	}
}

[[nodiscard]] std::vector<QColor> ColorsByIndex(int index) {
	const auto c = Ui::EmptyUserpic::UserpicColor(
		Ui::EmptyUserpic::ColorIndex(index));
	return { c.color1->c, c.color2->c };
}

class EmojiSelector final : public Ui::RpWidget {
public:
	EmojiSelector(
		not_null<Ui::RpWidget*> parent,
		not_null<Window::SessionController*> controller);

	[[nodiscard]] rpl::producer<not_null<DocumentData*>> chosen() const;

private:
	using Footer = ChatHelpers::TabbedSelector::InnerFooter;
	using List = ChatHelpers::TabbedSelector::Inner;
	using Type = ChatHelpers::SelectorTab;
	void createSelector(Type type);

	struct Selector {
		not_null<List*> list;
		not_null<Footer*> footer;
	};
	[[nodiscard]] Selector createEmojiList() const;
	[[nodiscard]] Selector createStickersList() const;

	const not_null<Window::SessionController*> _controller;
	base::unique_qptr<Ui::RpWidget> _container;
	base::unique_qptr<Ui::ScrollArea> _scroll;

	rpl::event_stream<not_null<DocumentData*>> _chosen;

};

EmojiSelector::EmojiSelector(
	not_null<Ui::RpWidget*> parent,
	not_null<Window::SessionController*> controller)
: RpWidget(parent)
, _controller(controller) {
	createSelector(Type::Emoji);
}

rpl::producer<not_null<DocumentData*>> EmojiSelector::chosen() const {
	return _chosen.events();
}

EmojiSelector::Selector EmojiSelector::createEmojiList() const {
	const auto session = &_controller->session();
	const auto manager = &session->data().customEmojiManager();
	const auto tag = Data::CustomEmojiManager::SizeTag::Large;
	auto args = ChatHelpers::EmojiListDescriptor{
		.session = session,
		.mode = ChatHelpers::EmojiListMode::FullReactions,
		.controller = _controller,
		.paused = [=] { return true; },
		.customRecentList = session->api().peerPhoto().profileEmojiList(),
		.customRecentFactory = [=](DocumentId id, Fn<void()> repaint) {
			return manager->create(id, std::move(repaint), tag);
		},
		.st = &st::reactPanelEmojiPan,
	};
	const auto list = _scroll->setOwnedWidget(
		object_ptr<ChatHelpers::EmojiListWidget>(_scroll, std::move(args)));
	const auto footer = list->createFooter().data();
	list->refreshEmoji();
	list->customChosen(
	) | rpl::start_with_next([=](const ChatHelpers::FileChosen &chosen) {
		_chosen.fire_copy(chosen.document);
	}, list->lifetime());
	return { list, footer };
}

EmojiSelector::Selector EmojiSelector::createStickersList() const {
	const auto list = _scroll->setOwnedWidget(
		object_ptr<ChatHelpers::StickersListWidget>(
			_scroll,
			_controller,
			Window::GifPauseReason::Any));
	const auto footer = list->createFooter().data();
	list->refreshRecent();
	list->chosen(
	) | rpl::start_with_next([=](const ChatHelpers::FileChosen &chosen) {
		_chosen.fire_copy(chosen.document);
	}, list->lifetime());
	return { list, footer };
}

void EmojiSelector::createSelector(Type type) {
	Expects((type == Type::Emoji) || (type == Type::Stickers));

	const auto isEmoji = (type == Type::Emoji);
	const auto &stScroll = st::reactPanelScroll;

	_container = base::make_unique_q<Ui::RpWidget>(this);
	const auto container = _container.get();
	container->show();
	sizeValue(
	) | rpl::start_with_next([=](const QSize &s) {
		container->setGeometry(Rect(s));
	}, container->lifetime());

	_scroll = base::make_unique_q<Ui::ScrollArea>(container, stScroll);

	const auto selector = isEmoji
		? createEmojiList()
		: createStickersList();
	selector.footer->setParent(container);

	const auto toggleButton = Ui::CreateChild<Ui::AbstractButton>(container);
	const auto &togglePos = st::userpicBuilderEmojiSelectorTogglePosition;
	{
		const auto &pos = togglePos;
		toggleButton->resize(st::menuIconStickers.size()
			// Trying to overlap the settings button under.
			+ QSize(pos.x() * 2, pos.y() * 2));
		toggleButton->show();
		toggleButton->paintRequest(
		) | rpl::start_with_next([=] {
			auto p = QPainter(toggleButton);
			const auto r = toggleButton->rect()
				- QMargins(pos.x(), pos.y(), pos.x(), pos.y());
			p.fillRect(r, st::boxBg);
			const auto &icon = st::userpicBuilderEmojiToggleStickersIcon;
			if (isEmoji) {
				icon.paintInCenter(p, r);
			} else {
				st::userpicBuilderEmojiToggleEmojiIcon.paintInCenter(p, r);
				const auto line = style::ConvertScaleExact(
					st::historyEmojiCircleLine);
				p.setPen(QPen(
					st::emojiIconFg,
					line,
					Qt::SolidLine,
					Qt::RoundCap));
				p.setBrush(Qt::NoBrush);
				PainterHighQualityEnabler hq(p);
				const auto diff = (icon.width()
					- st::userpicBuilderEmojiToggleEmojiSize) / 2;
				p.drawEllipse(r - Margins(diff));
			}
		}, toggleButton->lifetime());
	}
	toggleButton->show();
	toggleButton->setClickedCallback([=] {
		createSelector(isEmoji ? Type::Stickers : Type::Emoji);
	});

	_scroll->scrollTopChanges(
	) | rpl::start_with_next([=] {
		const auto scrollTop = _scroll->scrollTop();
		const auto scrollBottom = scrollTop + _scroll->height();
		selector.list->setVisibleTopBottom(scrollTop, scrollBottom);
	}, selector.list->lifetime());

	selector.list->scrollToRequests(
	) | rpl::start_with_next([=](int y) {
		_scroll->scrollToY(y);
		// _shadow->update();
	}, selector.list->lifetime());

	const auto separator = Ui::CreateChild<Ui::RpWidget>(container);
	separator->paintRequest(
	) | rpl::start_with_next([=](const QRect &r) {
		auto p = QPainter(separator);
		p.fillRect(r, st::shadowFg);
	}, separator->lifetime());

	selector.footer->show();
	separator->show();
	_scroll->show();

	const auto scrollWidth = stScroll.width;
	sizeValue(
	) | rpl::start_with_next([=](const QSize &s) {
		const auto left = st::userpicBuilderEmojiSelectorLeft;
		const auto mostTop = st::userpicBuilderEmojiSelectorLeft;

		toggleButton->move(QPoint(left, mostTop));

		selector.footer->setGeometry(
			(isEmoji ? (rect::right(toggleButton) - togglePos.x()) : left),
			mostTop,
			s.width() - left,
			selector.footer->height());

		separator->setGeometry(
			0,
			rect::bottom(selector.footer),
			s.width(),
			st::lineWidth);

		selector.list->resizeToWidth(s.width() - st::boxRadius * 2);
		_scroll->setGeometry(
			st::boxRadius,
			rect::bottom(separator),
			selector.list->width() + scrollWidth,
			s.height() - rect::bottom(separator));
	}, lifetime());
}

} // namespace

not_null<Ui::VerticalLayout*> CreateUserpicBuilder(
		not_null<Ui::RpWidget*> parent,
		not_null<Window::SessionController*> controller,
		StartData data,
		BothWayCommunication<QImage&&> communication) {
	const auto container = Ui::CreateChild<Ui::VerticalLayout>(parent.get());

	struct State {
		std::vector<not_null<CircleButton*>> circleButtons;
		Ui::Animations::Simple chosenColorAnimation;
		int colorIndex = -1;
	};
	const auto state = container->lifetime().make_state<State>();

	const auto preview = container->add(
		object_ptr<Ui::CenterWrap<EmojiUserpic>>(
			container,
			object_ptr<EmojiUserpic>(
				container,
				Size(st::settingsInfoPhotoSize))),
		st::userpicBuilderEmojiPreviewPadding)->entity();
	if (const auto id = data.documentId) {
		if (const auto document = controller->session().data().document(id)) {
			preview->setDocument(document);
		}
	}

	container->add(
		object_ptr<Ui::CenterWrap<Ui::FlatLabel>>(
			container,
			object_ptr<Ui::FlatLabel>(
				container,
				tr::lng_userpic_builder_color_subtitle(),
				st::userpicBuilderEmojiSubtitle)),
		st::userpicBuilderEmojiSubtitlePadding);

	const auto paletteBg = Ui::AddBubbleWrap(
		container,
		st::userpicBuilderEmojiBubblePaletteSize,
		[=] { return controller->chatStyle(); });

	const auto palette = Ui::CreateChild<Ui::RpWidget>(paletteBg.get());
	{
		constexpr auto kColorsCount = int(7);
		const auto size = st::userpicBuilderEmojiAccentColorSize;
		for (auto i = 0; i < kColorsCount; i++) {
			const auto colors = ColorsByIndex(i);
			const auto button = Ui::CreateChild<CircleButton>(palette);
			state->circleButtons.push_back(button);
			button->resize(size, size);
			button->setBrush(GenerateGradient(Size(size), colors));
			button->setClickedCallback([=] {
				const auto was = state->colorIndex;
				const auto now = i;
				if (was == now) {
					return;
				}
				state->chosenColorAnimation.stop();
				state->chosenColorAnimation.start([=](float64 progress) {
					if (was >= 0) {
						state->circleButtons[was]->setSelectedProgress(
							1. - progress);
					}
					state->circleButtons[now]->setSelectedProgress(progress);
				}, 0., 1., st::slideDuration);
				state->colorIndex = now;

				preview->setGradientColors(colors);
			});
		}
		const auto current = data.builderColorIndex % kColorsCount;
		state->circleButtons[current]->setSelectedProgress(1.);
		state->circleButtons[current]->clicked({}, Qt::LeftButton);
	}
	paletteBg->innerRectValue(
	) | rpl::start_with_next([=](const QRect &r) {
		palette->setGeometry(r - st::userpicBuilderEmojiBubblePalettePadding);
		AlignChildren(palette, palette->width());
	}, palette->lifetime());

	container->add(
		object_ptr<Ui::CenterWrap<Ui::FlatLabel>>(
			container,
			object_ptr<Ui::FlatLabel>(
				container,
				tr::lng_userpic_builder_emoji_subtitle(),
				st::userpicBuilderEmojiSubtitle)),
		st::userpicBuilderEmojiSubtitlePadding);

	const auto selectorBg = Ui::AddBubbleWrap(
		container,
		QSize(
			st::userpicBuilderEmojiBubblePaletteSize.width(),
			st::userpicBuilderEmojiSelectorMinHeight),
		[=] { return controller->chatStyle(); });
	const auto selector = Ui::CreateChild<EmojiSelector>(
		selectorBg.get(),
		controller);
	selector->chosen(
	) | rpl::start_with_next([=](not_null<DocumentData*> document) {
		preview->setDocument(document);
	}, preview->lifetime());
	selectorBg->innerRectValue(
	) | rpl::start_with_next([=](const QRect &r) {
		selector->setGeometry(r);
	}, selector->lifetime());

	base::take(
		communication.triggers
	) | rpl::start_with_next([=, done = base::take(communication.result)] {
		preview->result(Editor::kProfilePhotoSize, [=](QImage &&image) {
			done(std::move(image));
		});
	}, preview->lifetime());

	return container;
}

not_null<Ui::RpWidget*> CreateEmojiUserpic(
		not_null<Ui::RpWidget*> parent,
		const QSize &size,
		rpl::producer<not_null<DocumentData*>> document,
		rpl::producer<int> colorIndex) {
	const auto widget = Ui::CreateChild<EmojiUserpic>(parent.get(), size);
	std::move(
		document
	) | rpl::start_with_next([=](not_null<DocumentData*> d) {
		widget->setDocument(d);
	}, widget->lifetime());
	std::move(
		colorIndex
	) | rpl::start_with_next([=](int index) {
		widget->setGradientColors(ColorsByIndex(index));
	}, widget->lifetime());
	return widget;
}

} // namespace UserpicBuilder