/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "base/binary_guard.h"

class DocumentData;
class HistoryItem;

namespace Window {
class SessionController;
} // namespace Window

namespace Data {

class DocumentMedia;

extern const char kOptionExternalVideoPlayer[];

[[nodiscard]] QString FileExtension(const QString &filepath);
// [[nodiscard]] bool IsValidMediaFile(const QString &filepath);
[[nodiscard]] bool IsExecutableName(const QString &filepath);
[[nodiscard]] bool IsIpRevealingName(const QString &filepath);
base::binary_guard ReadBackgroundImageAsync(
	not_null<Data::DocumentMedia*> media,
	FnMut<QImage(QImage)> postprocess,
	FnMut<void(QImage&&)> done);

void ResolveDocument(
	Window::SessionController *controller,
	not_null<DocumentData*> document,
	HistoryItem *item,
	MsgId topicRootId);

} // namespace Data
