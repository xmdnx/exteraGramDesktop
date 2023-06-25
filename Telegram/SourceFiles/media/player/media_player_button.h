/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/effects/animations.h"
#include "ui/widgets/buttons.h"

#include <QtGui/QFontMetrics>

namespace style {
struct MediaPlayerButton;
struct MediaSpeedButton;
} // namespace style

namespace Media::Player {

class PlayButtonLayout {
public:
	enum class State {
		Play,
		Pause,
		Cancel,
	};
	PlayButtonLayout(const style::MediaPlayerButton &st, Fn<void()> callback);

	void setState(State state);
	void finishTransform();
	void paint(QPainter &p, const QBrush &brush);

private:
	void animationCallback();
	void startTransform(float64 from, float64 to);

	void paintPlay(QPainter &p, const QBrush &brush);
	void paintPlayToPause(QPainter &p, const QBrush &brush, float64 progress);
	void paintPlayToCancel(QPainter &p, const QBrush &brush, float64 progress);
	void paintPauseToCancel(QPainter &p, const QBrush &brush, float64 progress);

	const style::MediaPlayerButton &_st;

	State _state = State::Play;
	State _oldState = State::Play;
	State _nextState = State::Play;
	Ui::Animations::Simple _transformProgress;
	bool _transformBackward = false;

	Fn<void()> _callback;

};

class SpeedButtonLayout {
public:
	SpeedButtonLayout(
		const style::MediaSpeedButton &st,
		Fn<void()> callback,
		float64 speed);

	void setSpeed(float64 speed);
	void paint(QPainter &p, bool over, bool active);

private:
	const style::MediaSpeedButton &_st;

	float64 _speed = 1.;

	QFontMetricsF _metrics;

	QString _text;
	float64 _textWidth = 0;

	Fn<void()> _callback;

};

class SpeedButton final : public Ui::RippleButton {
public:
	SpeedButton(QWidget *parent, const style::MediaSpeedButton &st);

	[[nodiscard]] const style::MediaSpeedButton &st() const {
		return _st;
	}

	void setSpeed(float64 speed, anim::type animated = anim::type::normal);

private:
	void paintEvent(QPaintEvent *e) override;

	QPoint prepareRippleStartPosition() const override;
	QImage prepareRippleMask() const override;

	const style::MediaSpeedButton &_st;
	SpeedButtonLayout _layout;
	bool _isDefault = false;

};

} // namespace Media::Player
