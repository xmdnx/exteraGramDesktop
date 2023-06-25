/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "history/view/media/history_view_media.h"
#include "data/data_location.h"

namespace Data {
class CloudImage;
} // namespace Data

namespace HistoryView {

class Location : public Media {
public:
	Location(
		not_null<Element*> parent,
		not_null<Data::CloudImage*> data,
		Data::LocationPoint point,
		const QString &title = QString(),
		const QString &description = QString());
	~Location();

	void draw(Painter &p, const PaintContext &context) const override;
	TextState textState(QPoint point, StateRequest request) const override;

	[[nodiscard]] TextSelection adjustSelection(
		TextSelection selection,
		TextSelectType type) const override;
	uint16 fullSelectionLength() const override {
		return _title.length() + _description.length();
	}
	bool hasTextForCopy() const override {
		return !_title.isEmpty() || !_description.isEmpty();
	}

	bool toggleSelectionByHandlerClick(const ClickHandlerPtr &p) const override {
		return p == _link;
	}
	bool dragItemByHandler(const ClickHandlerPtr &p) const override {
		return p == _link;
	}

	TextForMimeData selectedText(TextSelection selection) const override;

	bool needsBubble() const override;
	bool customInfoLayout() const override {
		return true;
	}
	QPoint resolveCustomInfoRightBottom() const override;

	bool skipBubbleTail() const override {
		return isRoundedInBubbleBottom();
	}

	void unloadHeavyPart() override;
	bool hasHeavyPart() const override;

private:
	void ensureMediaCreated() const;

	void validateImageCache(
		QSize outer,
		Ui::BubbleRounding rounding) const;

	QSize countOptimalSize() override;
	QSize countCurrentSize(int newWidth) override;

	[[nodiscard]] TextSelection toDescriptionSelection(
		TextSelection selection) const;
	[[nodiscard]] TextSelection fromDescriptionSelection(
		TextSelection selection) const;

	[[nodiscard]] int fullWidth() const;
	[[nodiscard]] int fullHeight() const;

	const not_null<Data::CloudImage*> _data;
	mutable std::shared_ptr<QImage> _media;
	Ui::Text::String _title, _description;
	ClickHandlerPtr _link;

	mutable QImage _imageCache;
	mutable Ui::BubbleRounding _imageCacheRounding;

};

} // namespace HistoryView
