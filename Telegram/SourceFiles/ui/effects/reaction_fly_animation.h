/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/effects/animations.h"
#include "data/data_message_reaction_id.h"

namespace Ui::Text {
class CustomEmoji;
} // namespace Ui::Text

namespace Data {
class Reactions;
enum class CustomEmojiSizeTag : uchar;
} // namespace Data

namespace Ui {

class AnimatedIcon;

struct ReactionFlyAnimationArgs {
	::Data::ReactionId id;
	QImage flyIcon;
	QRect flyFrom;

	[[nodiscard]] ReactionFlyAnimationArgs translated(QPoint point) const;
};

class ReactionFlyAnimation final {
public:
	ReactionFlyAnimation(
		not_null<::Data::Reactions*> owner,
		ReactionFlyAnimationArgs &&args,
		Fn<void()> repaint,
		int size,
		Data::CustomEmojiSizeTag customSizeTag = {});
	~ReactionFlyAnimation();

	void setRepaintCallback(Fn<void()> repaint);
	QRect paintGetArea(
		QPainter &p,
		QPoint origin,
		QRect target,
		const QColor &colored,
		QRect clip,
		crl::time now) const;

	[[nodiscard]] bool flying() const;
	[[nodiscard]] float64 flyingProgress() const;
	[[nodiscard]] bool finished() const;

private:
	struct Parabolic {
		float64 a = 0.;
		float64 b = 0.;
		std::optional<int> key;
	};
	struct MiniCopy {
		mutable Parabolic cached;
		float64 maxScale = 1.;
		float64 duration = 1.;
		int flyUp = 0;
		int finalX = 0;
		int finalY = 0;
	};

	[[nodiscard]] auto flyCallback();
	[[nodiscard]] auto callback();
	void startAnimations();
	int computeParabolicTop(
		Parabolic &cache,
		int from,
		int to,
		int top,
		float64 progress) const;
	void paintCenterFrame(
		QPainter &p,
		QRect target,
		const QColor &colored,
		crl::time now) const;
	void paintMiniCopies(
		QPainter &p,
		QPoint center,
		const QColor &colored,
		crl::time now) const;
	void generateMiniCopies(int size);

	const not_null<::Data::Reactions*> _owner;
	Fn<void()> _repaint;
	QImage _flyIcon;
	std::unique_ptr<Text::CustomEmoji> _custom;
	std::unique_ptr<AnimatedIcon> _center;
	std::unique_ptr<AnimatedIcon> _effect;
	std::vector<MiniCopy> _miniCopies;
	Animations::Simple _fly;
	Animations::Simple _minis;
	QRect _flyFrom;
	float64 _centerSizeMultiplier = 0.;
	int _customSize = 0;
	bool _valid = false;

	mutable Parabolic _cached;

};

} // namespace Ui
