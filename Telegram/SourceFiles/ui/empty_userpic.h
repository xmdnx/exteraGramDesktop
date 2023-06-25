/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "base/weak_ptr.h"

namespace Ui {

class EmptyUserpic final : public base::has_weak_ptr {
public:
	struct BgColors {
		const style::color color1;
		const style::color color2;
	};

	[[nodiscard]] static int ColorIndex(uint64 id);
	[[nodiscard]] static EmptyUserpic::BgColors UserpicColor(int id);

	[[nodiscard]] static QString ExternalName();
	[[nodiscard]] static QString InaccessibleName();

	EmptyUserpic(const BgColors &colors, const QString &name);

	void paintCircle(
		QPainter &p,
		int x,
		int y,
		int outerWidth,
		int size) const;
	void paintRounded(
		QPainter &p,
		int x,
		int y,
		int outerWidth,
		int size,
		int radius = 0) const;
	void paintSquare(
		QPainter &p,
		int x,
		int y,
		int outerWidth,
		int size) const;
	[[nodiscard]] QPixmap generate(int size);
	[[nodiscard]] std::pair<uint64, uint64> uniqueKey() const;

	static void PaintSavedMessages(
		QPainter &p,
		int x,
		int y,
		int outerWidth,
		int size);
	static void PaintSavedMessages(
		QPainter &p,
		int x,
		int y,
		int outerWidth,
		int size,
		QBrush bg,
		const style::color &fg);
	[[nodiscard]] static QImage GenerateSavedMessages(int size);

	static void PaintRepliesMessages(
		QPainter &p,
		int x,
		int y,
		int outerWidth,
		int size);
	static void PaintRepliesMessages(
		QPainter &p,
		int x,
		int y,
		int outerWidth,
		int size,
		QBrush bg,
		const style::color &fg);
	[[nodiscard]] static QImage GenerateRepliesMessages(int size);

	~EmptyUserpic();

private:
	void paint(
		QPainter &p,
		int x,
		int y,
		int outerWidth,
		int size,
		Fn<void()> paintBackground) const;

	void fillString(const QString &name);

	const BgColors _colors;
	QString _string;

};

} // namespace Ui
