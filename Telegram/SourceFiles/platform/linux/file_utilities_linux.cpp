/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#include "platform/linux/file_utilities_linux.h"

#include "base/platform/linux/base_linux_app_launch_context.h"
#include "platform/linux/linux_xdp_open_with_dialog.h"

#include <QtGui/QDesktopServices>

#include <glibmm.h>
#include <giomm.h>

namespace Platform {
namespace File {

void UnsafeOpenUrl(const QString &url) {
	try {
		if (Gio::AppInfo::launch_default_for_uri(
			url.toStdString(),
			base::Platform::AppLaunchContext())) {
			return;
		}
	} catch (const std::exception &e) {
		LOG(("App Error: %1").arg(QString::fromStdString(e.what())));
	}

	QDesktopServices::openUrl(url);
}

void UnsafeOpenEmailLink(const QString &email) {
	UnsafeOpenUrl(u"mailto:"_q + email);
}

bool UnsafeShowOpenWith(const QString &filepath) {
	if (internal::ShowXDPOpenWithDialog(filepath)) {
		return true;
	}

	return false;
}

void UnsafeLaunch(const QString &filepath) {
	try {
		if (Gio::AppInfo::launch_default_for_uri(
			Glib::filename_to_uri(filepath.toStdString()),
			base::Platform::AppLaunchContext())) {
			return;
		}
	} catch (const std::exception &e) {
		LOG(("App Error: %1").arg(QString::fromStdString(e.what())));
	}

	if (UnsafeShowOpenWith(filepath)) {
		return;
	}

	QDesktopServices::openUrl(QUrl::fromLocalFile(filepath));
}

} // namespace File

namespace FileDialog {

bool Get(
		QPointer<QWidget> parent,
		QStringList &files,
		QByteArray &remoteContent,
		const QString &caption,
		const QString &filter,
		::FileDialog::internal::Type type,
		QString startFile) {
	if (parent) {
		parent = parent->window();
	}
	// Workaround for sandboxed paths
	static const auto docRegExp = QRegularExpression("^/run/user/\\d+/doc");
	if (cDialogLastPath().contains(docRegExp)) {
		InitLastPath();
	}
	return ::FileDialog::internal::GetDefault(
		parent,
		files,
		remoteContent,
		caption,
		filter,
		type,
		startFile);
}

} // namespace FileDialog
} // namespace Platform
