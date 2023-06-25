/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/chat/attach/attach_controls.h"
#include "ui/chat/attach/attach_send_files_way.h"
#include "ui/effects/animations.h"
#include "ui/grouped_layout.h"
#include "ui/round_rect.h"
#include "base/object_ptr.h"

namespace Ui {

struct PreparedFile;
class IconButton;
class SpoilerAnimation;

class AlbumThumbnail final {
public:
	AlbumThumbnail(
		const PreparedFile &file,
		const GroupMediaLayout &layout,
		QWidget *parent,
		Fn<void()> repaint,
		Fn<void()> editCallback,
		Fn<void()> deleteCallback);

	void moveToLayout(const GroupMediaLayout &layout);
	void animateLayoutToInitial();
	void resetLayoutAnimation();

	void setSpoiler(bool spoiler);
	[[nodiscard]] bool hasSpoiler() const;

	int photoHeight() const;
	int fileHeight() const;

	void paintInAlbum(
		QPainter &p,
		int left,
		int top,
		float64 shrinkProgress,
		float64 moveProgress);
	void paintPhoto(Painter &p, int left, int top, int outerWidth);
	void paintFile(Painter &p, int left, int top, int outerWidth);

	bool containsPoint(QPoint position) const;
	bool buttonsContainPoint(QPoint position) const;
	AttachButtonType buttonTypeFromPoint(QPoint position) const;
	int distanceTo(QPoint position) const;
	bool isPointAfter(QPoint position) const;
	void moveInAlbum(QPoint to);
	QPoint center() const;
	void suggestMove(float64 delta, Fn<void()> callback);
	void finishAnimations();

	void setButtonVisible(bool value);
	void moveButtons(int thumbTop);

	bool isCompressedSticker() const;

	static constexpr auto kShrinkDuration = crl::time(150);

private:
	QRect countRealGeometry() const;
	QRect countCurrentGeometry(float64 progress) const;
	void prepareCache(QSize size, int shrink);
	void drawSimpleFrame(QPainter &p, QRect to, QSize size) const;
	QRect paintButtons(
		QPainter &p,
		QPoint point,
		int outerWidth,
		float64 shrinkProgress);
	void paintPlayVideo(QPainter &p, QRect geometry);

	GroupMediaLayout _layout;
	std::optional<QRect> _animateFromGeometry;
	const QImage _fullPreview;
	const int _shrinkSize;
	const bool _isPhoto;
	const bool _isVideo;
	QPixmap _albumImage;
	QPixmap _albumImageBlurred;
	QImage _albumCache;
	QPoint _albumPosition;
	RectParts _albumCorners = RectPart::None;
	QPixmap _photo;
	QPixmap _photoBlurred;
	QPixmap _fileThumb;
	QString _name;
	QString _status;
	int _nameWidth = 0;
	int _statusWidth = 0;
	float64 _suggestedMove = 0.;
	Animations::Simple _suggestedMoveAnimation;
	int _lastShrinkValue = 0;
	AttachControls _buttons;

	bool _isCompressedSticker = false;
	std::unique_ptr<SpoilerAnimation> _spoiler;
	QImage _cornerCache;
	Fn<void()> _repaint;

	QRect _lastRectOfModify;
	QRect _lastRectOfButtons;

	object_ptr<IconButton> _editMedia = { nullptr };
	object_ptr<IconButton> _deleteMedia = { nullptr };

};

} // namespace Ui
