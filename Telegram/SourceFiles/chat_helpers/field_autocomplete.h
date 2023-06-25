/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "api/api_common.h"
#include "ui/effects/animations.h"
#include "ui/effects/message_sending_animation_common.h"
#include "ui/rp_widget.h"
#include "base/timer.h"
#include "base/object_ptr.h"

namespace Ui {
class PopupMenu;
class ScrollArea;
class InputField;
} // namespace Ui

namespace Lottie {
class SinglePlayer;
class FrameRenderer;
} // namespace Lottie;

namespace Main {
class Session;
} // namespace Main

namespace Window {
class SessionController;
} // namespace Window

namespace Data {
class DocumentMedia;
} // namespace Data

namespace SendMenu {
enum class Type;
} // namespace SendMenu

namespace ChatHelpers {
struct FileChosen;
class Show;
} // namespace ChatHelpers

class FieldAutocomplete final : public Ui::RpWidget {
public:
	FieldAutocomplete(
		QWidget *parent,
		not_null<Window::SessionController*> controller);
	FieldAutocomplete(
		QWidget *parent,
		std::shared_ptr<ChatHelpers::Show> show);
	~FieldAutocomplete();

	[[nodiscard]] std::shared_ptr<ChatHelpers::Show> uiShow() const;

	bool clearFilteredBotCommands();
	void showFiltered(
		not_null<PeerData*> peer,
		QString query,
		bool addInlineBots);
	void showStickers(EmojiPtr emoji);
	void setBoundings(QRect boundings);

	const QString &filter() const;
	ChatData *chat() const;
	ChannelData *channel() const;
	UserData *user() const;

	int32 innerTop();
	int32 innerBottom();

	bool eventFilter(QObject *obj, QEvent *e) override;

	enum class ChooseMethod {
		ByEnter,
		ByTab,
		ByClick,
	};
	struct MentionChosen {
		not_null<UserData*> user;
		QString mention;
		ChooseMethod method = ChooseMethod::ByEnter;
	};
	struct HashtagChosen {
		QString hashtag;
		ChooseMethod method = ChooseMethod::ByEnter;
	};
	struct BotCommandChosen {
		QString command;
		ChooseMethod method = ChooseMethod::ByEnter;
	};
	using StickerChosen = ChatHelpers::FileChosen;
	enum class Type {
		Mentions,
		Hashtags,
		BotCommands,
		Stickers,
	};

	bool chooseSelected(ChooseMethod method) const;

	bool stickersShown() const {
		return !_srows.empty();
	}

	bool overlaps(const QRect &globalRect) {
		if (isHidden() || !testAttribute(Qt::WA_OpaquePaintEvent)) return false;

		return rect().contains(QRect(mapFromGlobal(globalRect.topLeft()), globalRect.size()));
	}

	void setModerateKeyActivateCallback(Fn<bool(int)> callback) {
		_moderateKeyActivateCallback = std::move(callback);
	}
	void setSendMenuType(Fn<SendMenu::Type()> &&callback);

	void hideFast();
	void showAnimated();
	void hideAnimated();

	rpl::producer<MentionChosen> mentionChosen() const;
	rpl::producer<HashtagChosen> hashtagChosen() const;
	rpl::producer<BotCommandChosen> botCommandChosen() const;
	rpl::producer<StickerChosen> stickerChosen() const;
	rpl::producer<Type> choosingProcesses() const;

protected:
	void paintEvent(QPaintEvent *e) override;

private:
	class Inner;
	friend class Inner;
	struct StickerSuggestion;
	struct MentionRow;
	struct BotCommandRow;

	using HashtagRows = std::vector<QString>;
	using BotCommandRows = std::vector<BotCommandRow>;
	using StickerRows = std::vector<StickerSuggestion>;
	using MentionRows = std::vector<MentionRow>;

	void animationCallback();
	void hideFinish();

	void updateFiltered(bool resetScroll = false);
	void recount(bool resetScroll = false);
	StickerRows getStickerSuggestions();

	const std::shared_ptr<ChatHelpers::Show> _show;
	const not_null<Main::Session*> _session;
	QPixmap _cache;
	MentionRows _mrows;
	HashtagRows _hrows;
	BotCommandRows _brows;
	StickerRows _srows;

	void rowsUpdated(
		MentionRows &&mrows,
		HashtagRows &&hrows,
		BotCommandRows &&brows,
		StickerRows &&srows,
		bool resetScroll);

	object_ptr<Ui::ScrollArea> _scroll;
	QPointer<Inner> _inner;

	ChatData *_chat = nullptr;
	UserData *_user = nullptr;
	ChannelData *_channel = nullptr;
	EmojiPtr _emoji;
	uint64 _stickersSeed = 0;
	Type _type = Type::Mentions;
	QString _filter;
	QRect _boundings;
	bool _addInlineBots;

	bool _hiding = false;

	Ui::Animations::Simple _a_opacity;

	Fn<bool(int)> _moderateKeyActivateCallback;

};
