/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#include "boxes/about_box.h"

#include "extera/extera_lang.h"
#include "lang/lang_keys.h"
#include "mainwidget.h"
#include "mainwindow.h"
#include "ui/boxes/confirm_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/text/text_utilities.h"
#include "base/platform/base_platform_info.h"
#include "core/file_utilities.h"
#include "core/click_handler_types.h"
#include "core/update_checker.h"
#include "core/application.h"
#include "styles/style_layers.h"
#include "styles/style_boxes.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QClipboard>

namespace {

rpl::producer<TextWithEntities> Text1() {
	return rktre("etg_about_text1", {
		"tdesktop_link",
		Ui::Text::Link(ktr("etg_about_text1_tdesktop"), "https://desktop.telegram.org/")
	});
}

rpl::producer<TextWithEntities> Text2() {
	return tr::lng_about_text2(
		lt_gpl_link,
		rpl::single(Ui::Text::Link(
			"GNU GPL",
			"https://github.com/xmdnx/exteraGramDesktop/blob/master/LICENSE")),
		lt_github_link,
		rpl::single(Ui::Text::Link(
			"GitHub",
			"https://github.com/xmdnx/exteraGramDesktop")),
		Ui::Text::WithEntities);
}

rpl::producer<TextWithEntities> Text3() {
	return rktre("etg_about_text3", {
		"channel_link",
		Ui::Text::Link(ktr("etg_about_text3_channel"), "https://t.me/exteraGramDesktop")
	}, {
		"faq_link",
		Ui::Text::Link(tr::lng_about_text3_faq(tr::now), telegramFaqLink())
	});
}

} // namespace

AboutBox::AboutBox(QWidget *parent)
: _version(this, tr::lng_about_version(tr::now, lt_version, currentVersionText()), st::aboutVersionLink)
, _text1(this, Text1(), st::aboutLabel)
, _text2(this, Text2(), st::aboutLabel)
, _text3(this, Text3(), st::aboutLabel) {
}

void AboutBox::prepare() {
	setTitle(rpl::single(u"exteraGram Desktop"_q));

	addButton(tr::lng_close(), [this] { closeBox(); });

	_text1->setLinksTrusted();
	_text2->setLinksTrusted();
	_text3->setLinksTrusted();

	_version->setClickedCallback([this] { showVersionHistory(); });

	setDimensions(st::aboutWidth, st::aboutTextTop + _text1->height() + st::aboutSkip + _text2->height() + st::aboutSkip + _text3->height());
}

void AboutBox::resizeEvent(QResizeEvent *e) {
	BoxContent::resizeEvent(e);

	const auto available = width()
		- st::boxPadding.left()
		- st::boxPadding.right();
	_version->moveToLeft(st::boxPadding.left(), st::aboutVersionTop);
	_text1->resizeToWidth(available);
	_text1->moveToLeft(st::boxPadding.left(), st::aboutTextTop);
	_text2->resizeToWidth(available);
	_text2->moveToLeft(st::boxPadding.left(), _text1->y() + _text1->height() + st::aboutSkip);
	_text3->resizeToWidth(available);
	_text3->moveToLeft(st::boxPadding.left(), _text2->y() + _text2->height() + st::aboutSkip);
}

void AboutBox::showVersionHistory() {
	if (cRealAlphaVersion()) {
		auto url = u"https://tdesktop.com/"_q;
		if (Platform::IsWindows32Bit()) {
			url += u"win/%1.zip"_q;
		} else if (Platform::IsWindows64Bit()) {
			url += u"win64/%1.zip"_q;
		} else if (Platform::IsMac()) {
			url += u"mac/%1.zip"_q;
		} else if (Platform::IsLinux()) {
			url += u"linux/%1.tar.xz"_q;
		} else {
			Unexpected("Platform value.");
		}
		url = url.arg(u"talpha%1_%2"_q.arg(cRealAlphaVersion()).arg(Core::countAlphaVersionSignature(cRealAlphaVersion())));

		QGuiApplication::clipboard()->setText(url);

		getDelegate()->show(
			Ui::MakeInformBox(
				"The link to the current private alpha "
				"version of Telegram Desktop was copied to the clipboard."),
			Ui::LayerOption::CloseOther);
	} else {
		File::OpenUrl(Core::App().changelogLink());
	}
}

void AboutBox::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
		closeBox();
	} else {
		BoxContent::keyPressEvent(e);
	}
}

QString telegramFaqLink() {
	const auto result = u"https://telegram.org/faq"_q;
	const auto langpacked = [&](const char *language) {
		return result + '/' + language;
	};
	const auto current = Lang::Id();
	for (const auto language : { "de", "es", "it", "ko" }) {
		if (current.startsWith(QLatin1String(language))) {
			return langpacked(language);
		}
	}
	if (current.startsWith(u"pt-br"_q)) {
		return langpacked("br");
	}
	return result;
}

QString currentVersionText() {
	auto result = QString::fromLatin1(AppVersionStr);
	if (cAlphaVersion()) {
		result += u" alpha %1"_q.arg(cAlphaVersion() % 1000);
	} else if (AppBetaVersion) {
		result += " beta";
	}
	if (Platform::IsWindows64Bit()) {
		result += " x64";
	}
	return result;
}
