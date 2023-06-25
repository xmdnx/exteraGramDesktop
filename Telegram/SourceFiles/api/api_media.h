/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

class HistoryItem;

namespace Api {

struct RemoteFileInfo;

MTPInputMedia PrepareUploadedPhoto(
	not_null<HistoryItem*> item,
	RemoteFileInfo info);

MTPInputMedia PrepareUploadedDocument(
	not_null<HistoryItem*> item,
	RemoteFileInfo info);

bool HasAttachedStickers(MTPInputMedia media);

} // namespace Api
