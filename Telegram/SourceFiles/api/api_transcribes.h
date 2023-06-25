/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "mtproto/sender.h"

class ApiWrap;

namespace Main {
class Session;
} // namespace Main

namespace Api {

class Transcribes final {
public:
	explicit Transcribes(not_null<ApiWrap*> api);

	struct Entry {
		QString result;
		bool shown = false;
		bool failed = false;
		bool toolong = false;
		bool pending = false;
		bool roundview = false;
		mtpRequestId requestId = 0;
	};

	void toggle(not_null<HistoryItem*> item);
	[[nodiscard]] const Entry &entry(not_null<HistoryItem*> item) const;

	void apply(const MTPDupdateTranscribedAudio &update);

private:
	void load(not_null<HistoryItem*> item);

	const not_null<Main::Session*> _session;
	MTP::Sender _api;

	base::flat_map<FullMsgId, Entry> _map;
	base::flat_map<uint64, FullMsgId> _ids;

};

} // namespace Api
