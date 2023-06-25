/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "base/object_ptr.h"

class HistoryItem;

namespace Data {
struct ReactionId;
} // namespace Data

namespace Api {
struct WhoReadList;
} // namespace Api

namespace Window {
class SessionController;
} // namespace Window

namespace Ui {
class BoxContent;
} // namespace Ui

namespace HistoryView::Reactions {

object_ptr<Ui::BoxContent> FullListBox(
	not_null<Window::SessionController*> window,
	not_null<HistoryItem*> item,
	Data::ReactionId selected,
	std::shared_ptr<Api::WhoReadList> whoReadIds = nullptr);

} // namespace HistoryView::Reactions
