/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "base/timer.h"

namespace Main {
class Session;
} // namespace Main

namespace Ui {
struct EmojiGroup;
} // namespace Ui

namespace Data {

class DocumentMedia;
class Session;

class EmojiStatuses final {
public:
	explicit EmojiStatuses(not_null<Session*> owner);
	~EmojiStatuses();

	[[nodiscard]] Session &owner() const {
		return *_owner;
	}
	[[nodiscard]] Main::Session &session() const;

	void refreshRecent();
	void refreshRecentDelayed();
	void refreshDefault();
	void refreshColored();

	enum class Type {
		Recent,
		Default,
		Colored,
	};
	[[nodiscard]] const std::vector<DocumentId> &list(Type type) const;

	[[nodiscard]] rpl::producer<> recentUpdates() const;
	[[nodiscard]] rpl::producer<> defaultUpdates() const;
	[[nodiscard]] rpl::producer<> coloredUpdates() const;

	void set(DocumentId id, TimeId until = 0);
	[[nodiscard]] bool setting() const;

	void registerAutomaticClear(not_null<UserData*> user, TimeId until);

	using Groups = std::vector<Ui::EmojiGroup>;
	[[nodiscard]] rpl::producer<Groups> emojiGroupsValue() const;
	[[nodiscard]] rpl::producer<Groups> statusGroupsValue() const;
	[[nodiscard]] rpl::producer<Groups> profilePhotoGroupsValue() const;
	void requestEmojiGroups();
	void requestStatusGroups();
	void requestProfilePhotoGroups();

private:
	struct GroupsType {
		rpl::variable<Groups> data;
		mtpRequestId requestId = 0;
		int32 hash = 0;
	};

	void requestRecent();
	void requestDefault();
	void requestColored();

	void updateRecent(const MTPDaccount_emojiStatuses &data);
	void updateDefault(const MTPDaccount_emojiStatuses &data);
	void updateColored(const MTPDmessages_stickerSet &data);

	void processClearingIn(TimeId wait);
	void processClearing();

	template <typename Request>
	void requestGroups(not_null<GroupsType*> type, Request &&request);

	const not_null<Session*> _owner;

	std::vector<DocumentId> _recent;
	std::vector<DocumentId> _default;
	std::vector<DocumentId> _colored;
	rpl::event_stream<> _recentUpdated;
	rpl::event_stream<> _defaultUpdated;
	rpl::event_stream<> _coloredUpdated;

	mtpRequestId _recentRequestId = 0;
	bool _recentRequestScheduled = false;
	uint64 _recentHash = 0;

	mtpRequestId _defaultRequestId = 0;
	uint64 _defaultHash = 0;

	mtpRequestId _coloredRequestId = 0;

	mtpRequestId _sentRequestId = 0;

	base::flat_map<not_null<UserData*>, TimeId> _clearing;
	base::Timer _clearingTimer;

	GroupsType _emojiGroups;
	GroupsType _statusGroups;
	GroupsType _profilePhotoGroups;

	rpl::lifetime _lifetime;

};

struct EmojiStatusData {
	DocumentId id = 0;
	TimeId until = 0;
};
[[nodiscard]] EmojiStatusData ParseEmojiStatus(const MTPEmojiStatus &status);

} // namespace Data
