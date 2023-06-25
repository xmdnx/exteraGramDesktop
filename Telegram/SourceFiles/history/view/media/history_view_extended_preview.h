/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "history/view/media/history_view_media.h"
#include "history/view/media/history_view_media_spoiler.h"

enum class ImageRoundRadius;

namespace Ui {
class SpoilerAnimation;
} // namespace Ui

namespace Data {
struct Invoice;
} // namespace Data

namespace HistoryView {

class Element;

class ExtendedPreview final : public Media {
public:
	ExtendedPreview(
		not_null<Element*> parent,
		not_null<Data::Invoice*> invoice);
	~ExtendedPreview();

	void draw(Painter &p, const PaintContext &context) const override;
	TextState textState(QPoint point, StateRequest request) const override;

	[[nodiscard]] bool toggleSelectionByHandlerClick(
		const ClickHandlerPtr &p) const override;
	[[nodiscard]] bool dragItemByHandler(
		const ClickHandlerPtr &p) const override;

	[[nodiscard]] TextSelection adjustSelection(
			TextSelection selection,
			TextSelectType type) const override {
		return _caption.adjustSelection(selection, type);
	}
	uint16 fullSelectionLength() const override {
		return _caption.length();
	}
	bool hasTextForCopy() const override {
		return !_caption.isEmpty();
	}

	TextForMimeData selectedText(TextSelection selection) const override;

	TextWithEntities getCaption() const override {
		return _caption.toTextWithEntities();
	}
	void hideSpoilers() override;
	bool needsBubble() const override;
	bool customInfoLayout() const override {
		return _caption.isEmpty();
	}
	QPoint resolveCustomInfoRightBottom() const override;
	bool skipBubbleTail() const override {
		return isRoundedInBubbleBottom() && _caption.isEmpty();
	}

	void parentTextUpdated() override;

	bool hasHeavyPart() const override;
	void unloadHeavyPart() override;

private:
	int minWidthForButton() const;
	void resolveButtonText();
	void ensureThumbnailRead() const;

	QSize countOptimalSize() override;
	QSize countCurrentSize(int newWidth) override;

	bool needInfoDisplay() const;
	void validateImageCache(
		QSize outer,
		std::optional<Ui::BubbleRounding> rounding) const;
	[[nodiscard]] QImage prepareImageCache(QSize outer) const;
	void paintButton(
		Painter &p,
		QRect outer,
		const PaintContext &context) const;

	const not_null<Data::Invoice*> _invoice;
	Ui::Text::String _caption;
	mutable MediaSpoiler _spoiler;
	mutable QImage _inlineThumbnail;
	mutable QImage _buttonBackground;
	mutable QColor _buttonBackgroundOverlay;
	mutable Ui::Text::String _buttonText;
	mutable bool _imageCacheInvalid = false;

};

} // namespace HistoryView
