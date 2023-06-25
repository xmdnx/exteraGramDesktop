/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace Platform {

class Tray;

} // namespace Platform

// Platform dependent implementations.

#ifdef Q_OS_MAC
#include "platform/mac/tray_mac.h"
#elif defined Q_OS_UNIX // Q_OS_MAC
#include "platform/linux/tray_linux.h"
#elif defined Q_OS_WIN // Q_OS_MAC || Q_OS_UNIX
#include "platform/win/tray_win.h"
#endif // Q_OS_MAC || Q_OS_UNIX || Q_OS_WIN
