/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/widgets/buttons.h"
#include "ui/effects/animations.h"

namespace style {
struct SendAsButton;
} // namespace style

namespace Ui {

class SendAsButton final : public AbstractButton {
public:
	SendAsButton(QWidget *parent, const style::SendAsButton &st);

	void setUserpic(QImage userpic);

	void setActive(bool active);

private:
	void paintEvent(QPaintEvent *e) override;

	const style::SendAsButton &_st;

	Animations::Simple _activeAnimation;
	bool _active = false;

	QImage _userpic;

};

} // namespace Ui
