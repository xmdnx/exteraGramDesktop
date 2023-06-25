/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "base/platform/win/base_windows_h.h"

namespace Platform {
namespace AppUserModelId {

void cleanupShortcut();
void checkPinned();

const WCHAR *getId();
bool validateShortcut();

const PROPERTYKEY &getKey();

} // namespace AppUserModelId
} // namespace Platform
