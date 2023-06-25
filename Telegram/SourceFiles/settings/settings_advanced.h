/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "settings/settings_common.h"

namespace Main {
class Account;
} // namespace Main

namespace Window {
class Controller;
} // namespace Window

namespace Settings {

void SetupConnectionType(
	not_null<Window::Controller*> controller,
	not_null<::Main::Account*> account,
	not_null<Ui::VerticalLayout*> container);
bool HasUpdate();
void SetupUpdate(
	not_null<Ui::VerticalLayout*> container,
	Fn<void(Type)> showOther);
void SetupWindowTitleContent(
	Window::SessionController *controller,
	not_null<Ui::VerticalLayout*> container);
void SetupSystemIntegrationContent(
	Window::SessionController *controller,
	not_null<Ui::VerticalLayout*> container);
void SetupAnimations(
	not_null<Window::Controller*> window,
	not_null<Ui::VerticalLayout*> container);

class Advanced : public Section<Advanced> {
public:
	Advanced(
		QWidget *parent,
		not_null<Window::SessionController*> controller);

	[[nodiscard]] rpl::producer<QString> title() override;

	rpl::producer<Type> sectionShowOther() override;

private:
	void setupContent(not_null<Window::SessionController*> controller);

	rpl::event_stream<Type> _showOther;

};

} // namespace Settings
