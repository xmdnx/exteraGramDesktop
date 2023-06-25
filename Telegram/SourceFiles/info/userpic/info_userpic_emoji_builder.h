/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace Window {
class SessionController;
} // namespace Window

namespace UserpicBuilder {

struct Result;
struct StartData;

void ShowLayer(
	not_null<Window::SessionController*> controller,
	StartData data,
	Fn<void(UserpicBuilder::Result)> &&doneCallback);

} // namespace UserpicBuilder
