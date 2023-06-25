/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/layers/layer_widget.h"

namespace Ui {
class RpWidget;
class SeparatePanel;
class BoxContent;
} // namespace Ui

namespace Passport {

class PanelController;

class Panel {
public:
	Panel(not_null<PanelController*> controller);

	int hideAndDestroyGetDuration();

	void showAskPassword();
	void showNoPassword();
	void showForm();
	void showCriticalError(const QString &error);
	void showEditValue(object_ptr<Ui::RpWidget> form);
	void showBox(
		object_ptr<Ui::BoxContent> box,
		Ui::LayerOptions options,
		anim::type animated);
	void showToast(const QString &text);

	rpl::producer<> backRequests() const;
	void setBackAllowed(bool allowed);

	not_null<Ui::RpWidget*> widget() const;

	~Panel();

private:
	not_null<PanelController*> _controller;
	std::unique_ptr<Ui::SeparatePanel> _widget;

};

} // namespace Passport
