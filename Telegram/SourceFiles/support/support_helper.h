/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "base/timer.h"
#include "support/support_templates.h"
#include "mtproto/sender.h"

class History;

namespace Main {
class Session;
} // namespace Main

namespace Window {
class SessionController;
} // namespace Window

namespace Support {

struct UserInfo {
	QString author;
	TimeId date = 0;
	TextWithEntities text;
};

inline bool operator==(const UserInfo &a, const UserInfo &b) {
	return (a.author == b.author)
		&& (a.date == b.date)
		&& (a.text == b.text);
}

inline bool operator!=(const UserInfo &a, const UserInfo &b) {
	return !(a == b);
}

class Helper final {
public:
	explicit Helper(not_null<Main::Session*> session);

	static std::unique_ptr<Helper> Create(not_null<Main::Session*> session);

	void registerWindow(not_null<Window::SessionController*> controller);
	void cloudDraftChanged(not_null<History*> history);

	void chatOccupiedUpdated(not_null<History*> history);

	bool isOccupiedByMe(History *history) const;
	bool isOccupiedBySomeone(History *history) const;

	void refreshInfo(not_null<UserData*> user);
	rpl::producer<UserInfo> infoValue(not_null<UserData*> user) const;
	rpl::producer<QString> infoLabelValue(not_null<UserData*> user) const;
	rpl::producer<TextWithEntities> infoTextValue(
		not_null<UserData*> user) const;
	UserInfo infoCurrent(not_null<UserData*> user) const;
	void editInfo(
		not_null<Window::SessionController*> controller,
		not_null<UserData*> user);

	Templates &templates();

private:
	struct SavingInfo {
		TextWithEntities data;
		mtpRequestId requestId = 0;
	};
	void checkOccupiedChats();
	void updateOccupiedHistory(
		not_null<Window::SessionController*> controller,
		History *history);
	void setSupportName(const QString &name);
	void occupyIfNotYet();
	void occupyInDraft();
	void reoccupy();

	void applyInfo(
		not_null<UserData*> user,
		const MTPhelp_UserInfo &result);
	void showEditInfoBox(
		not_null<Window::SessionController*> controller,
		not_null<UserData*> user);
	void saveInfo(
		not_null<UserData*> user,
		TextWithEntities text,
		Fn<void(bool success)> done);

	not_null<Main::Session*> _session;
	MTP::Sender _api;
	Templates _templates;
	QString _supportName;
	QString _supportNameNormalized;

	History *_occupiedHistory = nullptr;
	base::Timer _reoccupyTimer;
	base::Timer _checkOccupiedTimer;
	base::flat_map<not_null<History*>, TimeId> _occupiedChats;

	base::flat_map<not_null<UserData*>, UserInfo> _userInformation;
	base::flat_map<
		not_null<UserData*>,
		base::weak_ptr<Window::SessionController>> _userInfoEditPending;
	base::flat_map<not_null<UserData*>, SavingInfo> _userInfoSaving;

	rpl::lifetime _lifetime;

};

QString ChatOccupiedString(not_null<History*> history);

QString InterpretSendPath(
	not_null<Window::SessionController*> window,
	const QString &path);

} // namespace Support
