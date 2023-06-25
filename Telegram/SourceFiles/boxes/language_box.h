/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "lang/lang_cloud_manager.h"
#include "boxes/abstract_box.h"
#include "base/binary_guard.h"

struct LanguageId;

namespace Ui {
class MultiSelect;
struct ScrollToRequest;
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

class LanguageBox : public Ui::BoxContent {
public:
	LanguageBox(QWidget*, Window::SessionController *controller);

	void setInnerFocus() override;

	static base::binary_guard Show(Window::SessionController *controller);

protected:
	void prepare() override;

	void keyPressEvent(QKeyEvent *e) override;

private:
	using Languages = Lang::CloudManager::Languages;

	void setupTop(not_null<Ui::VerticalLayout*> container);
	[[nodiscard]] int rowsInPage() const;

	Window::SessionController *_controller = nullptr;
	rpl::event_stream<bool> _translateChatTurnOff;
	Fn<void()> _setInnerFocus;
	Fn<Ui::ScrollToRequest(int rows)> _jump;

};
