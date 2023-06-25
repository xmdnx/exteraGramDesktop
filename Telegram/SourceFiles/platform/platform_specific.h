/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace Core {
enum class QuitReason;
} // namespace Core

namespace Platform {

void start();
void finish();

enum class PermissionStatus {
	Granted,
	CanRequest,
	Denied,
};

enum class PermissionType {
	Microphone,
	Camera,
};

enum class SystemSettingsType {
	Audio,
};

void SetApplicationIcon(const QIcon &icon);
QString SingleInstanceLocalServerName(const QString &hash);
PermissionStatus GetPermissionStatus(PermissionType type);
void RequestPermission(PermissionType type, Fn<void(PermissionStatus)> resultCallback);
void OpenSystemSettingsForPermission(PermissionType type);
bool OpenSystemSettings(SystemSettingsType type);
void IgnoreApplicationActivationRightNow();
[[nodiscard]] bool AutostartSupported();
void AutostartRequestStateFromSystem(Fn<void(bool)> callback);
void AutostartToggle(bool enabled, Fn<void(bool)> done = nullptr);
[[nodiscard]] bool AutostartSkip();
bool TrayIconSupported();
bool SkipTaskbarSupported();
void WriteCrashDumpDetails();
void NewVersionLaunched(int oldVersion);
[[nodiscard]] QImage DefaultApplicationIcon();
[[nodiscard]] bool PreventsQuit(Core::QuitReason reason);
[[nodiscard]] QString ExecutablePathForShortcuts();

#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
[[nodiscard]] std::optional<bool> IsDarkMode();
#endif // Qt < 6.5.0

namespace ThirdParty {

void start();
void finish();

} // namespace ThirdParty
} // namespace Platform

#ifdef Q_OS_MAC
#include "platform/mac/specific_mac.h"
#elif defined Q_OS_UNIX // Q_OS_MAC
#include "platform/linux/specific_linux.h"
#elif defined Q_OS_WIN // Q_OS_MAC || Q_OS_UNIX
#include "platform/win/specific_win.h"
#endif // Q_OS_MAC || Q_OS_UNIX || Q_OS_WIN
