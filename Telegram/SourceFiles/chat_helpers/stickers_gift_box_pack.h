/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

class DocumentData;

namespace Main {
class Session;
} // namespace Main

namespace Stickers {

class GiftBoxPack final {
public:
	explicit GiftBoxPack(not_null<Main::Session*> session);
	~GiftBoxPack();

	void load();
	[[nodiscard]] DocumentData *lookup(int months) const;

private:
	using SetId = uint64;
	void applySet(const MTPDmessages_stickerSet &data);

	const not_null<Main::Session*> _session;
	const std::vector<int> _localMonths;

	std::vector<DocumentData*> _documents;
	SetId _setId = 0;
	mtpRequestId _requestId = 0;

};

} // namespace Stickers
