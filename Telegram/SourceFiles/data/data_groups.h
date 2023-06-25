/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "data/data_types.h"

namespace Data {

class Session;

struct Group {
	HistoryItemsList items;
};

class Groups {
public:
	Groups(not_null<Session*> data);

	[[nodiscard]] bool isGrouped(not_null<const HistoryItem*> item) const;
	[[nodiscard]] bool isGroupOfOne(not_null<const HistoryItem*> item) const;
	void registerMessage(not_null<HistoryItem*> item);
	void unregisterMessage(not_null<const HistoryItem*> item);
	void refreshMessage(
		not_null<HistoryItem*> item,
		bool justRefreshViews = false);

	[[nodiscard]] const Group *find(not_null<const HistoryItem*> item) const;

	not_null<HistoryItem*> findItemToEdit(not_null<HistoryItem*> item) const;

private:
	HistoryItemsList::const_iterator findPositionForItem(
		const HistoryItemsList &group,
		not_null<HistoryItem*> item);
	void refreshViews(const HistoryItemsList &items);

	not_null<Session*> _data;
	std::map<MessageGroupId, Group> _groups;

};

} // namespace Data
