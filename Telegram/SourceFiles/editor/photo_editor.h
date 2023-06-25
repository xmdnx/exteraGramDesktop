/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/rp_widget.h"
#include "ui/image/image.h"
#include "base/unique_qptr.h"
#include "editor/photo_editor_common.h"
#include "editor/photo_editor_inner_common.h"

namespace Ui {
class LayerWidget;
} // namespace Ui

namespace Window {
class Controller;
} // namespace Window

namespace Editor {

class ColorPicker;
class PhotoEditorContent;
class PhotoEditorControls;
struct Controllers;

class PhotoEditor final : public Ui::RpWidget {
public:
	PhotoEditor(
		not_null<QWidget*> parent,
		not_null<Window::Controller*> controller,
		std::shared_ptr<Image> photo,
		PhotoModifications modifications,
		EditorData data = EditorData());

	void save();
	[[nodiscard]] rpl::producer<PhotoModifications> doneRequests() const;
	[[nodiscard]] rpl::producer<> cancelRequests() const;

private:
	void keyPressEvent(QKeyEvent *e) override;

	PhotoModifications _modifications;

	const std::shared_ptr<Controllers> _controllers;

	base::unique_qptr<PhotoEditorContent> _content;
	base::unique_qptr<PhotoEditorControls> _controls;
	const std::unique_ptr<ColorPicker> _colorPicker;

	rpl::variable<PhotoEditorMode> _mode = PhotoEditorMode{
		.mode = PhotoEditorMode::Mode::Transform,
		.action = PhotoEditorMode::Action::None,
	};
	rpl::event_stream<PhotoModifications> _done;
	rpl::event_stream<> _cancel;

};

void InitEditorLayer(
	not_null<Ui::LayerWidget*> layer,
	not_null<PhotoEditor*> editor,
	Fn<void(PhotoModifications)> doneCallback);

} // namespace Editor
