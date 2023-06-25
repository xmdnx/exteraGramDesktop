/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/effects/animations.h"

namespace Window {

enum class SlideDirection {
	FromRight,
	FromLeft,
};

class SlideAnimation {
public:
	void paintContents(QPainter &p) const;

	[[nodiscard]] float64 progress() const;

	void setDirection(SlideDirection direction);
	void setPixmaps(
		const QPixmap &oldContentCache,
		const QPixmap &newContentCache);
	void setTopBarShadow(bool enabled);
	void setTopSkip(int skip);
	void setTopBarMask(const QPixmap &mask);
	void setWithFade(bool withFade);

	using RepaintCallback = Fn<void()>;
	void setRepaintCallback(RepaintCallback &&callback);

	using FinishedCallback = Fn<void()>;
	void setFinishedCallback(FinishedCallback &&callback);

	void start();

	static const anim::transition &transition() {
		return anim::easeOutCirc;
	}

private:
	void animationCallback();

	SlideDirection _direction = SlideDirection::FromRight;
	int _topSkip = 0;
	bool _topBarShadowEnabled = false;
	bool _withFade = false;

	mutable Ui::Animations::Simple _animation;
	QPixmap _cacheUnder, _cacheOver;
	QPixmap _mask;

	RepaintCallback _repaintCallback;
	FinishedCallback _finishedCallback;

};

} // namespace Window
