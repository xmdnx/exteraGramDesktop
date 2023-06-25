/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

class HistoryItem;
class History;
class PeerData;

namespace Api {

struct FoundMessages {
	int total = -1;
	MessageIdsList messages;
	QString nextToken;
};

class MessagesSearch final {
public:
	explicit MessagesSearch(not_null<History*> history);
	~MessagesSearch();

	void searchMessages(const QString &query, PeerData *from);
	void searchMore();

	[[nodiscard]] rpl::producer<FoundMessages> messagesFounds() const;

private:
	using TLMessages = MTPmessages_Messages;
	void searchRequest();
	void searchReceived(
		const TLMessages &result,
		mtpRequestId requestId,
		const QString &nextToken);

	const not_null<History*> _history;

	base::flat_map<QString, TLMessages> _cacheOfStartByToken;

	QString _query;
	PeerData *_from = nullptr;
	MsgId _offsetId;

	int _searchInHistoryRequest = 0; // Not real mtpRequestId.
	mtpRequestId _requestId = 0;

	rpl::event_stream<FoundMessages> _messagesFounds;

};

} // namespace Api
