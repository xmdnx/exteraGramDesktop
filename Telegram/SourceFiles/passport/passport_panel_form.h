/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/rp_widget.h"
#include "base/object_ptr.h"

namespace Ui {
class BoxContentDivider;
class ScrollArea;
class RoundButton;
class FlatLabel;
class UserpicButton;
} // namespace Ui

namespace Passport::Ui {
using namespace ::Ui;
class FormRow;
} // namespace Passport::Ui

namespace Passport {

class PanelController;

class PanelForm : public Ui::RpWidget {
public:
	PanelForm(
		QWidget *parent,
		not_null<PanelController*> controller);

protected:
	void resizeEvent(QResizeEvent *e) override;

private:
	using Row = Ui::FormRow;

	void setupControls();
	not_null<Ui::RpWidget*> setupContent();
	void updateControlsGeometry();

	not_null<PanelController*> _controller;

	object_ptr<Ui::ScrollArea> _scroll;
	object_ptr<Ui::RoundButton> _submit;

	QPointer<Ui::UserpicButton> _userpic;
	QPointer<Ui::FlatLabel> _about1;
	QPointer<Ui::FlatLabel> _about2;
	std::vector<QPointer<Row>> _rows;

};

} // namespace Passport
