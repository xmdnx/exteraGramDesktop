/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace Platform {
namespace Audio {

void Init();

void DeInit();

} // namespace Audio
} // namespace Platform

// Platform dependent implementations.

#if defined Q_OS_WINRT || defined Q_OS_WIN
#include "platform/win/audio_win.h"
#else // Q_OS_WINRT || Q_OS_WIN
namespace Platform {
namespace Audio {

inline void Init() {
}

inline void DeInit() {
}

} // namespace Audio
} // namespace Platform
#endif // Q_OS_WINRT || Q_OS_WIN
