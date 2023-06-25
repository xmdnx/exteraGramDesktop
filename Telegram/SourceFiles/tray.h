/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "platform/platform_tray.h"

namespace Core {

class Tray final {
public:
	Tray();

	void create();
	void updateMenuText();
	void updateIconCounters();

	[[nodiscard]] rpl::producer<> aboutToShowRequests() const;
	[[nodiscard]] rpl::producer<> showFromTrayRequests() const;
	[[nodiscard]] rpl::producer<> hideToTrayRequests() const;

	[[nodiscard]] bool has() const;

private:
	void rebuildMenu();
	void toggleSoundNotifications();

	Platform::Tray _tray;

	bool _activeForTrayIconAction = false;
	crl::time _lastTrayClickTime = 0;

	rpl::event_stream<> _textUpdates;
	rpl::event_stream<> _minimizeMenuItemClicks;

};

} // namespace Core
