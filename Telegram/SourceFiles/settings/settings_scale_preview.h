/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace Ui {
class RpWidget;
} // namespace Ui

namespace Window {
class Controller;
} // namespace Window

namespace Settings {

enum class ScalePreviewShow {
	Show,
	Update,
	Hide,
};

[[nodiscard]] Fn<void(ScalePreviewShow, int, int)> SetupScalePreview(
	not_null<Window::Controller*> window,
	not_null<Ui::RpWidget*> slider);

} // namespace Settings
