/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace Ui {
class RpWidget;
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace UserpicBuilder {

struct StartData;
struct Result;

template <typename Result>
struct BothWayCommunication;

not_null<Ui::VerticalLayout*> CreateUserpicBuilder(
	not_null<Ui::RpWidget*> parent,
	not_null<Window::SessionController*> controller,
	StartData data,
	BothWayCommunication<UserpicBuilder::Result> communication);

[[nodiscard]] not_null<Ui::RpWidget*> CreateEmojiUserpic(
	not_null<Ui::RpWidget*> parent,
	const QSize &size,
	rpl::producer<not_null<DocumentData*>> document,
	rpl::producer<int> colorIndex,
	bool isForum);

} // namespace UserpicBuilder
