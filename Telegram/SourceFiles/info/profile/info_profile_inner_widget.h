/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/rp_widget.h"
#include "base/object_ptr.h"

namespace Data {
class ForumTopic;
class PhotoMedia;
} // namespace Data

namespace Window {
class SessionController;
} // namespace Window

namespace Ui {
class VerticalLayout;
template <typename Widget>
class SlideWrap;
struct ScrollToRequest;
class MultiSlideTracker;
} // namespace Ui

namespace Info {

enum class Wrap;
class Controller;

namespace Profile {

class Memento;
class Members;
class Cover;

class InnerWidget final : public Ui::RpWidget {
public:
	InnerWidget(
		QWidget *parent,
		not_null<Controller*> controller);

	void saveState(not_null<Memento*> memento);
	void restoreState(not_null<Memento*> memento);

	rpl::producer<Ui::ScrollToRequest> scrollToRequests() const;
	rpl::producer<int> desiredHeightValue() const override;

protected:
	int resizeGetHeight(int newWidth) override;
	void visibleTopBottomUpdated(
		int visibleTop,
		int visibleBottom) override;

private:
	object_ptr<RpWidget> setupContent(not_null<RpWidget*> parent);
	object_ptr<RpWidget> setupSharedMedia(not_null<RpWidget*> parent);
	void setupMembers(not_null<Ui::VerticalLayout*> container);

	int countDesiredHeight() const;
	void updateDesiredHeight() {
		_desiredHeight.fire(countDesiredHeight());
	}

	const not_null<Controller*> _controller;
	const not_null<PeerData*> _peer;
	PeerData * const _migrated = nullptr;
	Data::ForumTopic * const _topic = nullptr;

	std::shared_ptr<Data::PhotoMedia> _nonPersonalView;

	Members *_members = nullptr;
	Cover *_cover = nullptr;
	Ui::SlideWrap<RpWidget> *_sharedMediaWrap = nullptr;
	object_ptr<RpWidget> _content;

	bool _inResize = false;
	rpl::event_stream<Ui::ScrollToRequest> _scrollToRequests;
	rpl::event_stream<int> _desiredHeight;

};

} // namespace Profile
} // namespace Info
