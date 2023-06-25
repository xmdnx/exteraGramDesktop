/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/effects/reaction_fly_animation.h"
#include "ui/rp_widget.h"

namespace Ui {

class EmojiFlyAnimation {
public:
	EmojiFlyAnimation(
		not_null<RpWidget*> body,
		not_null<Data::Reactions*> owner,
		Ui::ReactionFlyAnimationArgs &&args,
		Fn<void()> repaint,
		Data::CustomEmojiSizeTag tag);

	[[nodiscard]] not_null<Ui::RpWidget*> layer();
	[[nodiscard]] bool finished() const;

	void repaint();
	bool paintBadgeFrame(not_null<Ui::RpWidget*> widget);

private:
	const int _flySize = 0;
	Ui::ReactionFlyAnimation _fly;
	Ui::RpWidget _layer;
	QRect _area;
	bool _areaUpdated = false;
	QPointer<Ui::RpWidget> _target;

};

} // namespace Ui
