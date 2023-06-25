/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace HistoryView {

struct ItemPreviewImage {
	QImage data;
	uint64 cacheKey = 0;

	[[nodiscard]] bool hasSpoiler() const {
		return (cacheKey & 1);
	}

	explicit operator bool() const {
		return !data.isNull();
	}
};

struct ItemPreview {
	TextWithEntities text;
	std::vector<ItemPreviewImage> images;
	int arrowInTextPosition = -1;
	int imagesInTextPosition = 0;
	std::any loadingContext;
};

struct ToPreviewOptions {
	const std::vector<ItemPreviewImage> *existing = nullptr;
	bool hideSender = false;
	bool hideCaption = false;
	bool generateImages = true;
	bool ignoreGroup = false;
	bool ignoreTopic = true;
	bool spoilerLoginCode = false;
	bool translated = false;
};

} // namespace HistoryView
