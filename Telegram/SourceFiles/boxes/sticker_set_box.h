/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "boxes/abstract_box.h"
#include "base/timer.h"
#include "data/stickers/data_stickers.h"

namespace Window {
class SessionController;
} // namespace Window

namespace Ui {
class PlainShadow;
} // namespace Ui

namespace Data {
class StickersSet;
} // namespace Data

namespace SendMenu {
enum class Type;
} // namespace SendMenu

namespace ChatHelpers {
struct FileChosen;
class Show;
} // namespace ChatHelpers

class StickerPremiumMark final {
public:
	explicit StickerPremiumMark(not_null<Main::Session*> session);

	void paint(
		QPainter &p,
		const QImage &frame,
		QImage &backCache,
		QPoint position,
		QSize singleSize,
		int outerWidth);

private:
	void validateLock(const QImage &frame, QImage &backCache);
	void validateStar();

	QImage _lockGray;
	QImage _star;
	bool _premium = false;

	rpl::lifetime _lifetime;

};

class StickerSetBox final : public Ui::BoxContent {
public:
	StickerSetBox(
		QWidget*,
		std::shared_ptr<ChatHelpers::Show> show,
		const StickerSetIdentifier &set,
		Data::StickersType type);
	StickerSetBox(
		QWidget*,
		std::shared_ptr<ChatHelpers::Show> show,
		not_null<Data::StickersSet*> set);

	static QPointer<Ui::BoxContent> Show(
		std::shared_ptr<ChatHelpers::Show> show,
		not_null<DocumentData*> document);

protected:
	void prepare() override;

	void resizeEvent(QResizeEvent *e) override;

private:
	enum class Error {
		NotFound,
	};

	void updateTitleAndButtons();
	void updateButtons();
	void addStickers();
	void copyStickersLink();
	void handleError(Error error);

	const std::shared_ptr<ChatHelpers::Show> _show;
	const not_null<Main::Session*> _session;
	const StickerSetIdentifier _set;
	const Data::StickersType _type;

	class Inner;
	QPointer<Inner> _inner;

};
