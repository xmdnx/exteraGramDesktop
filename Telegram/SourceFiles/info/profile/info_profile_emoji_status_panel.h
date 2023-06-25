/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "base/unique_qptr.h"

namespace Data {
class Session;
enum class CustomEmojiSizeTag : uchar;
} // namespace Data

namespace Window {
class SessionController;
} // namespace Window

namespace Ui {
struct MessageSendingAnimationFrom;
class EmojiFlyAnimation;
class RpWidget;
} // namespace Ui

namespace Ui::Text {
class CustomEmoji;
struct CustomEmojiPaintContext;
} // namespace Ui::Text

namespace ChatHelpers {
class TabbedPanel;
} // namespace ChatHelpers

namespace Info::Profile {

class EmojiStatusPanel final {
public:
	EmojiStatusPanel();
	~EmojiStatusPanel();

	void setChooseFilter(Fn<bool(DocumentId)> filter);

	void show(
		not_null<Window::SessionController*> controller,
		not_null<QWidget*> button,
		Data::CustomEmojiSizeTag animationSizeTag = {});

	bool paintBadgeFrame(not_null<Ui::RpWidget*> widget);

private:
	void create(not_null<Window::SessionController*> controller);
	[[nodiscard]] bool filter(
		not_null<Window::SessionController*> controller,
		DocumentId chosenId) const;

	void startAnimation(
		not_null<Data::Session*> owner,
		not_null<Ui::RpWidget*> body,
		DocumentId statusId,
		Ui::MessageSendingAnimationFrom from);

	base::unique_qptr<ChatHelpers::TabbedPanel> _panel;
	Fn<bool(DocumentId)> _chooseFilter;
	QPointer<QWidget> _panelButton;
	std::unique_ptr<Ui::EmojiFlyAnimation> _animation;
	Data::CustomEmojiSizeTag _animationSizeTag = {};

};

} // namespace Info::Profile
