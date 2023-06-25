/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/abstract_button.h"

namespace UserpicBuilder {

class CircleButton final : public Ui::AbstractButton {
public:
	using Ui::AbstractButton::AbstractButton;

	void setIndex(int index);
	[[nodiscard]] int index() const;
	void setBrush(QBrush brush);
	void setSelectedProgress(float64 progress);

private:
	void paintEvent(QPaintEvent *event) override;

	int _index = 0;
	float64 _selectedProgress = 0.;
	QBrush _brush;

};

} // namespace UserpicBuilder
