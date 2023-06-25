/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/layers/generic_box.h"

namespace Data {
struct CloudTheme;
} // namespace Data

namespace Window {

class Controller;

namespace Theme {

struct Object;
struct ParsedTheme;

void StartEditor(
	not_null<Window::Controller*> window,
	const Data::CloudTheme &cloud);
void CreateBox(
	not_null<Ui::GenericBox*> box,
	not_null<Window::Controller*> window);
void CreateForExistingBox(
	not_null<Ui::GenericBox*> box,
	not_null<Window::Controller*> window,
	const Data::CloudTheme &cloud);
void SaveTheme(
	not_null<Window::Controller*> window,
	const Data::CloudTheme &cloud,
	const QByteArray &palette,
	Fn<void()> unlock);
void SaveThemeBox(
	not_null<Ui::GenericBox*> box,
	not_null<Window::Controller*> window,
	const Data::CloudTheme &cloud,
	const QByteArray &palette);

[[nodiscard]] bool PaletteChanged(
	const QByteArray &editorPalette,
	const Data::CloudTheme &cloud);

[[nodiscard]] QByteArray CollectForExport(const QByteArray &palette);

[[nodiscard]] ParsedTheme ParseTheme(
	const Object &theme,
	bool onlyPalette = false,
	bool parseCurrent = true);

[[nodiscard]] QString GenerateSlug();

} // namespace Theme
} // namespace Window
