/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/cached_round_corners.h"
#include "ui/chat/message_bubble.h"
#include "ui/chat/chat_style_radius.h"
#include "ui/style/style_core_palette.h"
#include "layout/layout_selection.h"
#include "styles/style_basic.h"

enum class ImageRoundRadius;

namespace style {
struct TwoIconButton;
struct ScrollArea;
} // namespace style

namespace Ui {

class ChatTheme;
class ChatStyle;
struct BubblePattern;

struct MessageStyle {
	CornersPixmaps msgBgCornersSmall;
	CornersPixmaps msgBgCornersLarge;
	style::color msgBg;
	style::color msgShadow;
	style::color msgServiceFg;
	style::color msgDateFg;
	style::color msgFileThumbLinkFg;
	style::color msgFileBg;
	style::color msgReplyBarColor;
	style::color msgWaveformActive;
	style::color msgWaveformInactive;
	style::color historyTextFg;
	style::color historyFileNameFg;
	style::color historyFileRadialFg;
	style::color mediaFg;
	style::TextPalette textPalette;
	style::TextPalette semiboldPalette;
	style::TextPalette fwdTextPalette;
	style::TextPalette replyTextPalette;
	style::icon tailLeft = { Qt::Uninitialized };
	style::icon tailRight = { Qt::Uninitialized };
	style::icon historyRepliesIcon = { Qt::Uninitialized };
	style::icon historyViewsIcon = { Qt::Uninitialized };
	style::icon historyPinIcon = { Qt::Uninitialized };
	style::icon historySentIcon = { Qt::Uninitialized };
	style::icon historyReceivedIcon = { Qt::Uninitialized };
	style::icon historyPsaIcon = { Qt::Uninitialized };
	style::icon historyCommentsOpen = { Qt::Uninitialized };
	style::icon historyComments = { Qt::Uninitialized };
	style::icon historyCallArrow = { Qt::Uninitialized };
	style::icon historyCallArrowMissed = { Qt::Uninitialized };
	style::icon historyCallIcon = { Qt::Uninitialized };
	style::icon historyCallCameraIcon = { Qt::Uninitialized };
	style::icon historyFilePlay = { Qt::Uninitialized };
	style::icon historyFileWaiting = { Qt::Uninitialized };
	style::icon historyFileDownload = { Qt::Uninitialized };
	style::icon historyFileCancel = { Qt::Uninitialized };
	style::icon historyFilePause = { Qt::Uninitialized };
	style::icon historyFileImage = { Qt::Uninitialized };
	style::icon historyFileDocument = { Qt::Uninitialized };
	style::icon historyAudioDownload = { Qt::Uninitialized };
	style::icon historyAudioCancel = { Qt::Uninitialized };
	style::icon historyQuizTimer = { Qt::Uninitialized };
	style::icon historyQuizExplain = { Qt::Uninitialized };
	style::icon historyPollChosen = { Qt::Uninitialized };
	style::icon historyPollChoiceRight = { Qt::Uninitialized };
	style::icon historyTranscribeIcon = { Qt::Uninitialized };
	style::icon historyTranscribeHide = { Qt::Uninitialized };

};

struct MessageImageStyle {
	CornersPixmaps msgDateImgBgCorners;
	CornersPixmaps msgServiceBgCornersSmall;
	CornersPixmaps msgServiceBgCornersLarge;
	CornersPixmaps msgShadowCornersSmall;
	CornersPixmaps msgShadowCornersLarge;
	style::color msgServiceBg;
	style::color msgDateImgBg;
	style::color msgShadow;
	style::color historyFileThumbRadialFg;
	style::icon historyFileThumbPlay = { Qt::Uninitialized };
	style::icon historyFileThumbWaiting = { Qt::Uninitialized };
	style::icon historyFileThumbDownload = { Qt::Uninitialized };
	style::icon historyFileThumbCancel = { Qt::Uninitialized };
	style::icon historyFileThumbPause = { Qt::Uninitialized };
	style::icon historyVideoDownload = { Qt::Uninitialized };
	style::icon historyVideoCancel = { Qt::Uninitialized };
	style::icon historyVideoMessageMute = { Qt::Uninitialized };
};

struct ReactionPaintInfo {
	QPoint position;
	QPoint effectOffset;
	Fn<QRect(QPainter&)> effectPaint;
};

struct ChatPaintContext {
	not_null<const ChatStyle*> st;
	const BubblePattern *bubblesPattern = nullptr;
	ReactionPaintInfo *reactionInfo = nullptr;
	QRect viewport;
	QRect clip;
	TextSelection selection;
	bool outbg = false;
	bool paused = false;
	crl::time now = 0;

	void translate(int x, int y) {
		viewport.translate(x, y);
		clip.translate(x, y);
	}
	void translate(QPoint point) {
		translate(point.x(), point.y());
	}

	[[nodiscard]] bool selected() const {
		return (selection == FullSelection);
	}
	[[nodiscard]] not_null<const MessageStyle*> messageStyle() const;
	[[nodiscard]] not_null<const MessageImageStyle*> imageStyle() const;

	[[nodiscard]] ChatPaintContext translated(int x, int y) const {
		auto result = *this;
		result.translate(x, y);
		return result;
	}
	[[nodiscard]] ChatPaintContext translated(QPoint point) const {
		return translated(point.x(), point.y());
	}
	[[nodiscard]] ChatPaintContext withSelection(
			TextSelection selection) const {
		auto result = *this;
		result.selection = selection;
		return result;
	}

	// This is supported only in unwrapped media for now.
	enum class SkipDrawingParts {
		None,
		Content,
		Surrounding,
	};
	SkipDrawingParts skipDrawingParts = SkipDrawingParts::None;

};

[[nodiscard]] int HistoryServiceMsgRadius();
[[nodiscard]] int HistoryServiceMsgInvertedRadius();
[[nodiscard]] int HistoryServiceMsgInvertedShrink();

class ChatStyle final : public style::palette {
public:
	ChatStyle();
	explicit ChatStyle(not_null<const style::palette*> isolated);

	void apply(not_null<ChatTheme*> theme);
	void applyCustomPalette(const style::palette *palette);
	void applyAdjustedServiceBg(QColor serviceBg);

	[[nodiscard]] rpl::producer<> paletteChanged() const {
		return _paletteChanged.events();
	}

	template <typename Type>
	[[nodiscard]] Type value(const Type &original) const {
		auto my = Type();
		make(my, original);
		return my;
	}

	template <typename Type>
	[[nodiscard]] const Type &value(
			rpl::lifetime &parentLifetime,
			const Type &original) const {
		const auto my = parentLifetime.make_state<Type>();
		make(*my, original);
		return *my;
	}

	[[nodiscard]] const CornersPixmaps &serviceBgCornersNormal() const;
	[[nodiscard]] const CornersPixmaps &serviceBgCornersInverted() const;

	[[nodiscard]] const MessageStyle &messageStyle(
		bool outbg,
		bool selected) const;
	[[nodiscard]] const MessageImageStyle &imageStyle(bool selected) const;

	[[nodiscard]] const CornersPixmaps &msgBotKbOverBgAddCornersSmall() const;
	[[nodiscard]] const CornersPixmaps &msgBotKbOverBgAddCornersLarge() const;
	[[nodiscard]] const CornersPixmaps &msgSelectOverlayCorners(
		CachedCornerRadius radius) const;

	[[nodiscard]] const style::TextPalette &historyPsaForwardPalette() const {
		return _historyPsaForwardPalette;
	}
	[[nodiscard]] const style::TextPalette &imgReplyTextPalette() const {
		return _imgReplyTextPalette;
	}
	[[nodiscard]] const style::TextPalette &serviceTextPalette() const {
		return _serviceTextPalette;
	}
	[[nodiscard]] const style::icon &historyRepliesInvertedIcon() const {
		return _historyRepliesInvertedIcon;
	}
	[[nodiscard]] const style::icon &historyViewsInvertedIcon() const {
		return _historyViewsInvertedIcon;
	}
	[[nodiscard]] const style::icon &historyViewsSendingIcon() const {
		return _historyViewsSendingIcon;
	}
	[[nodiscard]] const style::icon &historyViewsSendingInvertedIcon() const {
		return _historyViewsSendingInvertedIcon;
	}
	[[nodiscard]] const style::icon &historyPinInvertedIcon() const {
		return _historyPinInvertedIcon;
	}
	[[nodiscard]] const style::icon &historySendingIcon() const {
		return _historySendingIcon;
	}
	[[nodiscard]] const style::icon &historySendingInvertedIcon() const {
		return _historySendingInvertedIcon;
	}
	[[nodiscard]] const style::icon &historySentInvertedIcon() const {
		return _historySentInvertedIcon;
	}
	[[nodiscard]] const style::icon &historyReceivedInvertedIcon() const {
		return _historyReceivedInvertedIcon;
	}
	[[nodiscard]] const style::icon &msgBotKbUrlIcon() const {
		return _msgBotKbUrlIcon;
	}
	[[nodiscard]] const style::icon &msgBotKbPaymentIcon() const {
		return _msgBotKbPaymentIcon;
	}
	[[nodiscard]] const style::icon &msgBotKbSwitchPmIcon() const {
		return _msgBotKbSwitchPmIcon;
	}
	[[nodiscard]] const style::icon &msgBotKbWebviewIcon() const {
		return _msgBotKbWebviewIcon;
	}
	[[nodiscard]] const style::icon &historyFastCommentsIcon() const {
		return _historyFastCommentsIcon;
	}
	[[nodiscard]] const style::icon &historyFastShareIcon() const {
		return _historyFastShareIcon;
	}
	[[nodiscard]] const style::icon &historyFastTranscribeIcon() const {
		return _historyFastTranscribeIcon;
	}
	[[nodiscard]] const style::icon &historyGoToOriginalIcon() const {
		return _historyGoToOriginalIcon;
	}
	[[nodiscard]] const style::icon &historyMapPoint() const {
		return _historyMapPoint;
	}
	[[nodiscard]] const style::icon &historyMapPointInner() const {
		return _historyMapPointInner;
	}
	[[nodiscard]] const style::icon &youtubeIcon() const {
		return _youtubeIcon;
	}
	[[nodiscard]] const style::icon &videoIcon() const {
		return _videoIcon;
	}
	[[nodiscard]] const style::icon &historyPollChoiceRight() const {
		return _historyPollChoiceRight;
	}
	[[nodiscard]] const style::icon &historyPollChoiceWrong() const {
		return _historyPollChoiceWrong;
	}

private:
	void assignPalette(not_null<const style::palette*> palette);

	void make(style::color &my, const style::color &original) const;
	void make(style::icon &my, const style::icon &original) const;
	void make(
		style::TextPalette &my,
		const style::TextPalette &original) const;
	void make(
		style::TwoIconButton &my,
		const style::TwoIconButton &original) const;
	void make(
		style::ScrollArea &my,
		const style::ScrollArea &original) const;

	[[nodiscard]] MessageStyle &messageStyleRaw(
		bool outbg,
		bool selected) const;
	[[nodiscard]] MessageStyle &messageIn();
	[[nodiscard]] MessageStyle &messageInSelected();
	[[nodiscard]] MessageStyle &messageOut();
	[[nodiscard]] MessageStyle &messageOutSelected();

	[[nodiscard]] MessageImageStyle &imageStyleRaw(bool selected) const;
	[[nodiscard]] MessageImageStyle &image();
	[[nodiscard]] MessageImageStyle &imageSelected();

	template <typename Type>
	void make(
		Type MessageStyle::*my,
		const Type &originalIn,
		const Type &originalInSelected,
		const Type &originalOut,
		const Type &originalOutSelected);

	template <typename Type>
	void make(
		Type MessageImageStyle::*my,
		const Type &original,
		const Type &originalSelected);

	mutable CornersPixmaps _serviceBgCornersNormal;
	mutable CornersPixmaps _serviceBgCornersInverted;

	mutable std::array<MessageStyle, 4> _messageStyles;
	mutable std::array<MessageImageStyle, 2> _imageStyles;

	mutable CornersPixmaps _msgBotKbOverBgAddCornersSmall;
	mutable CornersPixmaps _msgBotKbOverBgAddCornersLarge;
	mutable CornersPixmaps _msgSelectOverlayCorners[
		int(CachedCornerRadius::kCount)];

	style::TextPalette _historyPsaForwardPalette;
	style::TextPalette _imgReplyTextPalette;
	style::TextPalette _serviceTextPalette;
	style::icon _historyRepliesInvertedIcon = { Qt::Uninitialized };
	style::icon _historyViewsInvertedIcon = { Qt::Uninitialized };
	style::icon _historyViewsSendingIcon = { Qt::Uninitialized };
	style::icon _historyViewsSendingInvertedIcon = { Qt::Uninitialized };
	style::icon _historyPinInvertedIcon = { Qt::Uninitialized };
	style::icon _historySendingIcon = { Qt::Uninitialized };
	style::icon _historySendingInvertedIcon = { Qt::Uninitialized };
	style::icon _historySentInvertedIcon = { Qt::Uninitialized };
	style::icon _historyReceivedInvertedIcon = { Qt::Uninitialized };
	style::icon _msgBotKbUrlIcon = { Qt::Uninitialized };
	style::icon _msgBotKbPaymentIcon = { Qt::Uninitialized };
	style::icon _msgBotKbSwitchPmIcon = { Qt::Uninitialized };
	style::icon _msgBotKbWebviewIcon = { Qt::Uninitialized };
	style::icon _historyFastCommentsIcon = { Qt::Uninitialized };
	style::icon _historyFastShareIcon = { Qt::Uninitialized };
	style::icon _historyFastTranscribeIcon = { Qt::Uninitialized };
	style::icon _historyGoToOriginalIcon = { Qt::Uninitialized };
	style::icon _historyMapPoint = { Qt::Uninitialized };
	style::icon _historyMapPointInner = { Qt::Uninitialized };
	style::icon _youtubeIcon = { Qt::Uninitialized };
	style::icon _videoIcon = { Qt::Uninitialized };
	style::icon _historyPollChoiceRight = { Qt::Uninitialized };
	style::icon _historyPollChoiceWrong = { Qt::Uninitialized };

	rpl::event_stream<> _paletteChanged;

	rpl::lifetime _defaultPaletteChangeLifetime;

};

void FillComplexOverlayRect(
	QPainter &p,
	QRect rect,
	const style::color &color,
	const CornersPixmaps &corners);

void FillComplexEllipse(
	QPainter &p,
	not_null<const ChatStyle*> st,
	QRect rect);

} // namespace Ui
