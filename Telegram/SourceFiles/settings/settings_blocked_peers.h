/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "settings/settings_common.h"

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Window {
class Controller;
} // namespace Window

namespace Settings {

class Blocked : public Section<Blocked> {
public:
	Blocked(
		QWidget *parent,
		not_null<Window::SessionController*> controller);

	void showFinished() override;

	[[nodiscard]] rpl::producer<QString> title() override;

	[[nodiscard]] QPointer<Ui::RpWidget> createPinnedToTop(
		not_null<QWidget*> parent) override;

private:
	void setupContent();
	void checkTotal(int total);

	void visibleTopBottomUpdated(int visibleTop, int visibleBottom) override;

	const not_null<Window::SessionController*> _controller;
	const not_null<Ui::VerticalLayout*> _container;

	base::unique_qptr<Ui::RpWidget> _loading;

	rpl::variable<int> _countBlocked;

	rpl::event_stream<> _showFinished;
	rpl::event_stream<bool> _emptinessChanges;

};

} // namespace Settings
