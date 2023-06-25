/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "core/file_utilities.h"

namespace Platform {
namespace File {

QString UrlToLocal(const QUrl &url);

// All these functions may enter a nested event loop. Use with caution.
void UnsafeOpenUrl(const QString &url);
void UnsafeOpenEmailLink(const QString &email);
bool UnsafeShowOpenWithDropdown(const QString &filepath);
bool UnsafeShowOpenWith(const QString &filepath);
void UnsafeLaunch(const QString &filepath);

void PostprocessDownloaded(const QString &filepath);

} // namespace File

namespace FileDialog {

void InitLastPath();

bool Get(
	QPointer<QWidget> parent,
	QStringList &files,
	QByteArray &remoteContent,
	const QString &caption,
	const QString &filter,
	::FileDialog::internal::Type type,
	QString startFile = QString());

} // namespace FileDialog
} // namespace Platform

// Platform dependent implementations.

#ifdef Q_OS_MAC
#include "platform/mac/file_utilities_mac.h"
#elif defined Q_OS_UNIX // Q_OS_MAC
#include "platform/linux/file_utilities_linux.h"
#elif defined Q_OS_WINRT || defined Q_OS_WIN // Q_OS_MAC || Q_OS_UNIX
#include "platform/win/file_utilities_win.h"
#endif // Q_OS_MAC || Q_OS_UNIX || Q_OS_WINRT || Q_OS_WIN
