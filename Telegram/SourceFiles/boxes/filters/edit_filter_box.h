/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/layers/generic_box.h"

namespace Window {
class SessionController;
} // namespace Window

namespace Data {
class ChatFilter;
} // namespace Data

void EditFilterBox(
	not_null<Ui::GenericBox*> box,
	not_null<Window::SessionController*> window,
	const Data::ChatFilter &filter,
	Fn<void(const Data::ChatFilter &)> doneCallback,
	Fn<void(const Data::ChatFilter &, Fn<void(Data::ChatFilter)>)> saveAnd);

void EditExistingFilter(
	not_null<Window::SessionController*> window,
	FilterId id);
