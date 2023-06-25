/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace Ui {
enum class WhoReadType;
} // namespace Ui

namespace Data {
struct ReactionId;
struct MessageReaction;
} // namespace Data

namespace HistoryView::Reactions {

struct Tabs {
	Fn<void(int, int)> move;
	Fn<void(int)> resizeToWidth;
	Fn<rpl::producer<Data::ReactionId>()> changes;
	Fn<rpl::producer<int>()> heightValue;
};

not_null<Tabs*> CreateTabs(
	not_null<QWidget*> parent,
	Ui::Text::CustomEmojiFactory factory,
	Fn<bool()> paused,
	const std::vector<Data::MessageReaction> &items,
	const Data::ReactionId &selected,
	Ui::WhoReadType whoReadType);

} // namespace HistoryView::Reactions
