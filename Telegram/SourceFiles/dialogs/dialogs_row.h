/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/text/text.h"
#include "ui/effects/animations.h"
#include "ui/unread_badge.h"
#include "ui/userpic_view.h"
#include "dialogs/dialogs_key.h"
#include "dialogs/ui/dialogs_message_view.h"

class History;
class HistoryItem;

namespace style {
struct DialogRow;
} // namespace style

namespace Ui {
class RippleAnimation;
} // namespace Ui

namespace Dialogs::Ui {
using namespace ::Ui;
class RowPainter;
class VideoUserpic;
struct PaintContext;
struct TopicJumpCache;
} // namespace Dialogs::Ui

namespace Dialogs {

enum class SortMode;

[[nodiscard]] QRect CornerBadgeTTLRect(int photoSize);

class BasicRow {
public:
	BasicRow();
	virtual ~BasicRow();

	virtual void paintUserpic(
		Painter &p,
		not_null<PeerData*> peer,
		Ui::VideoUserpic *videoUserpic,
		History *historyForCornerBadge,
		const Ui::PaintContext &context) const;

	void addRipple(QPoint origin, QSize size, Fn<void()> updateCallback);
	virtual void stopLastRipple();
	virtual void clearRipple();
	void addRippleWithMask(
		QPoint origin,
		QImage mask,
		Fn<void()> updateCallback);

	void paintRipple(
		QPainter &p,
		int x,
		int y,
		int outerWidth,
		const QColor *colorOverride = nullptr) const;

	[[nodiscard]] Ui::PeerUserpicView &userpicView() const {
		return _userpic;
	}

private:
	mutable Ui::PeerUserpicView _userpic;
	mutable std::unique_ptr<Ui::RippleAnimation> _ripple;

};

class List;
class Row final : public BasicRow {
public:
	explicit Row(std::nullptr_t) {
	}
	Row(Key key, int index, int top);
	~Row();

	[[nodiscard]] int top() const {
		return _top;
	}
	[[nodiscard]] int height() const {
		Expects(_height != 0);

		return _height;
	}
	void recountHeight(float64 narrowRatio);

	void updateCornerBadgeShown(
		not_null<PeerData*> peer,
		Fn<void()> updateCallback = nullptr) const;
	void paintUserpic(
		Painter &p,
		not_null<PeerData*> peer,
		Ui::VideoUserpic *videoUserpic,
		History *historyForCornerBadge,
		const Ui::PaintContext &context) const final override;

	[[nodiscard]] bool lookupIsInTopicJump(int x, int y) const;
	void stopLastRipple() override;
	void clearRipple() override;
	void addTopicJumpRipple(
		QPoint origin,
		not_null<Ui::TopicJumpCache*> topicJumpCache,
		Fn<void()> updateCallback);
	void clearTopicJumpRipple();
	[[nodiscard]] bool topicJumpRipple() const;

	[[nodiscard]] Key key() const {
		return _id;
	}
	[[nodiscard]] History *history() const {
		return _id.history();
	}
	[[nodiscard]] Data::Folder *folder() const {
		return _id.folder();
	}
	[[nodiscard]] Data::ForumTopic *topic() const {
		return _id.topic();
	}
	[[nodiscard]] Data::Thread *thread() const {
		return _id.thread();
	}
	[[nodiscard]] not_null<Entry*> entry() const {
		return _id.entry();
	}
	[[nodiscard]] int index() const {
		return _index;
	}
	[[nodiscard]] uint64 sortKey(FilterId filterId) const;

	// for any attached data, for example View in contacts list
	void *attached = nullptr;

private:
	friend class List;

	class CornerLayersManager {
	public:
		using Layer = int;
		CornerLayersManager();

		[[nodiscard]] bool isSameLayer(Layer layer) const;
		[[nodiscard]] bool isDisplayedNone() const;
		[[nodiscard]] float64 progressForLayer(Layer layer) const;
		[[nodiscard]] float64 progress() const;
		[[nodiscard]] bool isFinished() const;
		void setLayer(Layer layer, Fn<void()> updateCallback);
		void markFrameShown();

	private:
		bool _lastFrameShown = false;
		Layer _prevLayer = 0;
		Layer _nextLayer = 0;
		Ui::Animations::Simple _animation;

	};

	struct CornerBadgeUserpic {
		InMemoryKey key;
		CornerLayersManager layersManager;
		int paletteVersion = 0;
		int frameIndex = -1;
		bool active = false;
		QImage frame;
		QImage cacheTTL;
	};

	void setCornerBadgeShown(
		CornerLayersManager::Layer nextLayer,
		Fn<void()> updateCallback) const;
	void ensureCornerBadgeUserpic() const;
	static void PaintCornerBadgeFrame(
		not_null<CornerBadgeUserpic*> data,
		not_null<PeerData*> peer,
		Ui::VideoUserpic *videoUserpic,
		Ui::PeerUserpicView &view,
		const Ui::PaintContext &context);

	Key _id;
	mutable std::unique_ptr<CornerBadgeUserpic> _cornerBadgeUserpic;
	int _top = 0;
	int _height = 0;
	int _index : 30 = 0;
	int _cornerBadgeShown : 1 = 0;
	int _topicJumpRipple : 1 = 0;

};

class FakeRow final : public BasicRow, public base::has_weak_ptr {
public:
	FakeRow(
		Key searchInChat,
		not_null<HistoryItem*> item,
		Fn<void()> repaint);

	[[nodiscard]] Key searchInChat() const {
		return _searchInChat;
	}
	[[nodiscard]] Data::ForumTopic *topic() const {
		return _topic;
	}
	[[nodiscard]] not_null<HistoryItem*> item() const {
		return _item;
	}
	[[nodiscard]] Ui::MessageView &itemView() const {
		return _itemView;
	}
	[[nodiscard]] Fn<void()> repaint() const {
		return _repaint;
	}
	[[nodiscard]] Ui::PeerBadge &badge() const {
		return _badge;
	}
	[[nodiscard]] const Ui::Text::String &name() const;

	void invalidateTopic();

private:
	friend class Ui::RowPainter;

	const Key _searchInChat;
	const not_null<HistoryItem*> _item;
	Data::ForumTopic *_topic = nullptr;
	const Fn<void()> _repaint;
	mutable Ui::MessageView _itemView;
	mutable Ui::PeerBadge _badge;
	mutable Ui::Text::String _name;

};

} // namespace Dialogs
