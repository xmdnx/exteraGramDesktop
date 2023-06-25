/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace Ui {
class RpWindow;
} // namespace Ui

namespace Ui::Platform {
struct SeparateTitleControls;
} // namespace Ui::Platform

namespace Media::View {

inline constexpr auto kMaximizedIconOpacity = 0.6;
inline constexpr auto kNormalIconOpacity = 0.9;
inline constexpr auto kOverBackgroundOpacity = 0.2775;
[[nodiscard]] QColor OverBackgroundColor();

} // namespace Media::View

namespace Platform {

class OverlayWidgetHelper {
public:
	virtual ~OverlayWidgetHelper() = default;

	virtual void orderWidgets() {
	}
	[[nodiscard]] virtual bool skipTitleHitTest(QPoint position) {
		return false;
	}
	[[nodiscard]] virtual rpl::producer<> controlsActivations() {
		return rpl::never<>();
	}
	[[nodiscard]] virtual rpl::producer<bool> controlsSideRightValue() {
		return rpl::single(true);
	}
	virtual void beforeShow(bool fullscreen) {
	}
	virtual void afterShow(bool fullscreen) {
	}
	virtual void notifyFileDialogShown(bool shown) {
	}
	virtual void minimize(not_null<Ui::RpWindow*> window);
	virtual void clearState() {
	}
	virtual void setControlsOpacity(float64 opacity) {
	}
	[[nodiscard]] virtual auto mouseEvents() const
	-> rpl::producer<not_null<QMouseEvent*>> {
		return rpl::never<not_null<QMouseEvent*>>();
	}
};

[[nodiscard]] std::unique_ptr<OverlayWidgetHelper> CreateOverlayWidgetHelper(
	not_null<Ui::RpWindow*> window,
	Fn<void(bool)> maximize);

class DefaultOverlayWidgetHelper final : public OverlayWidgetHelper {
public:
	DefaultOverlayWidgetHelper(
		not_null<Ui::RpWindow*> window,
		Fn<void(bool)> maximize);
	~DefaultOverlayWidgetHelper();

	void orderWidgets() override;
	bool skipTitleHitTest(QPoint position) override;
	rpl::producer<> controlsActivations() override;
	void beforeShow(bool fullscreen) override;
	void clearState() override;
	void setControlsOpacity(float64 opacity) override;
	rpl::producer<bool> controlsSideRightValue() override;
	rpl::producer<not_null<QMouseEvent*>> mouseEvents() const override;

private:
	class Buttons;

	const not_null<Buttons*> _buttons;
	const std::unique_ptr<Ui::Platform::SeparateTitleControls> _controls;

};

} // namespace Platform

// Platform dependent implementations.

#ifdef Q_OS_MAC
#include "platform/mac/overlay_widget_mac.h"
#elif defined Q_OS_UNIX // Q_OS_MAC
#include "platform/linux/overlay_widget_linux.h"
#elif defined Q_OS_WIN // Q_OS_MAC || Q_OS_UNIX
#include "platform/win/overlay_widget_win.h"
#endif // Q_OS_MAC || Q_OS_UNIX || Q_OS_WIN
