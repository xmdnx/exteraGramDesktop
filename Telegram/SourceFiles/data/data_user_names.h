/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace Data {

struct Username final {
	QString username;
	bool active = false;
	bool editable = false;
};

using Usernames = std::vector<Username>;

class UsernamesInfo final {
public:
	UsernamesInfo();

	void setUsername(const QString &username);
	void setUsernames(const Usernames &usernames);

	[[nodiscard]] QString username() const;
	[[nodiscard]] QString editableUsername() const;
	[[nodiscard]] const std::vector<QString> &usernames() const;

private:
	std::vector<QString> _usernames;
	int _indexEditableUsername = -1;

};

} // namespace Data
