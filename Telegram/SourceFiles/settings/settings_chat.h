/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "settings/settings_common.h"

namespace Window {
class Controller;
} // namespace Window

namespace Settings {

void SetupDataStorage(
	not_null<Window::SessionController*> controller,
	not_null<Ui::VerticalLayout*> container);
void SetupAutoDownload(
	not_null<Window::SessionController*> controller,
	not_null<Ui::VerticalLayout*> container);
void SetupDefaultThemes(
	not_null<Window::Controller*> window,
	not_null<Ui::VerticalLayout*> container);
void SetupSupport(
	not_null<Window::SessionController*> controller,
	not_null<Ui::VerticalLayout*> container);
void SetupExport(
	not_null<Window::SessionController*> controller,
	not_null<Ui::VerticalLayout*> container);

void PaintRoundColorButton(
	QPainter &p,
	int size,
	QBrush brush,
	float64 selected);

class Chat : public Section<Chat> {
public:
	Chat(QWidget *parent, not_null<Window::SessionController*> controller);

	[[nodiscard]] rpl::producer<QString> title() override;

private:
	void setupContent(not_null<Window::SessionController*> controller);

};

} // namespace Settings
