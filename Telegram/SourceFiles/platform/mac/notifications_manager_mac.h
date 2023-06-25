/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "platform/platform_notifications_manager.h"
#include "base/weak_ptr.h"

namespace Platform {
namespace Notifications {

class Manager : public Window::Notifications::NativeManager, public base::has_weak_ptr {
public:
	Manager(Window::Notifications::System *system);
	~Manager();

protected:
	void doShowNativeNotification(
		not_null<PeerData*> peer,
		MsgId topicRootId,
		Ui::PeerUserpicView &userpicView,
		MsgId msgId,
		const QString &title,
		const QString &subtitle,
		const QString &msg,
		DisplayOptions options) override;
	void doClearAllFast() override;
	void doClearFromItem(not_null<HistoryItem*> item) override;
	void doClearFromTopic(not_null<Data::ForumTopic*> topic) override;
	void doClearFromHistory(not_null<History*> history) override;
	void doClearFromSession(not_null<Main::Session*> session) override;
	QString accountNameSeparator() override;
	bool doSkipToast() const override;
	void doMaybePlaySound(Fn<void()> playSound) override;
	void doMaybeFlashBounce(Fn<void()> flashBounce) override;

private:
	class Private;
	const std::unique_ptr<Private> _private;

};

} // namespace Notifications
} // namespace Platform
