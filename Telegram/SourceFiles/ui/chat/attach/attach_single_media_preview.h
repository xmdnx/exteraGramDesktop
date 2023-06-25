/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/chat/attach/attach_abstract_single_media_preview.h"
#include "media/clip/media_clip_reader.h"

namespace Lottie {
class SinglePlayer;
} // namespace Lottie

namespace Ui {

struct PreparedFile;

class SingleMediaPreview final : public AbstractSingleMediaPreview {
public:
	static SingleMediaPreview *Create(
		QWidget *parent,
		Fn<bool()> gifPaused,
		const PreparedFile &file,
		AttachControls::Type type = AttachControls::Type::Full);

	SingleMediaPreview(
		QWidget *parent,
		Fn<bool()> gifPaused,
		QImage preview,
		bool animated,
		bool sticker,
		bool spoiler,
		const QString &animatedPreviewPath,
		AttachControls::Type type);

protected:
	bool supportsSpoilers() const override;
	bool drawBackground() const override;
	bool tryPaintAnimation(QPainter &p) override;
	bool isAnimatedPreviewReady() const override;

private:
	void prepareAnimatedPreview(
		const QString &animatedPreviewPath,
		bool animated);
	void clipCallback(Media::Clip::Notification notification);

	const Fn<bool()> _gifPaused;
	const bool _sticker = false;
	Media::Clip::ReaderPointer _gifPreview;
	std::unique_ptr<Lottie::SinglePlayer> _lottiePreview;

};

} // namespace Ui
