/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "mtproto/sender.h"

class ApiWrap;
class PeerData;
class UserData;

namespace Data {
struct FileOrigin;
} // namespace Data

namespace Main {
class Session;
} // namespace Main

namespace Api {

class PeerPhoto final {
public:
	using UserPhotoId = PhotoId;
	explicit PeerPhoto(not_null<ApiWrap*> api);

	enum class EmojiListType {
		Profile,
		Group,
	};

	struct UserPhoto {
		QImage image;
		DocumentId markupDocumentId = 0;
		std::vector<QColor> markupColors;
	};

	void upload(
		not_null<PeerData*> peer,
		UserPhoto &&photo,
		Fn<void()> done = nullptr);
	void uploadFallback(not_null<PeerData*> peer, UserPhoto &&photo);
	void updateSelf(
		not_null<PhotoData*> photo,
		Data::FileOrigin origin,
		Fn<void()> done = nullptr);
	void suggest(not_null<PeerData*> peer, UserPhoto &&photo);
	void clear(not_null<PhotoData*> photo);
	void clearPersonal(not_null<UserData*> user);
	void set(not_null<PeerData*> peer, not_null<PhotoData*> photo);

	void requestUserPhotos(not_null<UserData*> user, UserPhotoId afterId);

	void requestEmojiList(EmojiListType type);
	using EmojiList = std::vector<DocumentId>;
	[[nodiscard]] rpl::producer<EmojiList> emojiListValue(EmojiListType type);

	// Non-personal photo in case a personal photo is set.
	void registerNonPersonalPhoto(
		not_null<UserData*> user,
		not_null<PhotoData*> photo);
	void unregisterNonPersonalPhoto(not_null<UserData*> user);
	[[nodiscard]] PhotoData *nonPersonalPhoto(
		not_null<UserData*> user) const;

private:
	enum class UploadType {
		Default,
		Suggestion,
		Fallback,
	};

	void ready(
		const FullMsgId &msgId,
		std::optional<MTPInputFile> file,
		std::optional<MTPVideoSize> videoSize);
	void upload(
		not_null<PeerData*> peer,
		UserPhoto &&photo,
		UploadType type,
		Fn<void()> done);

	const not_null<Main::Session*> _session;
	MTP::Sender _api;

	struct UploadValue {
		not_null<PeerData*> peer;
		UploadType type = UploadType::Default;
		Fn<void()> done;
	};

	base::flat_map<FullMsgId, UploadValue> _uploads;

	base::flat_map<not_null<UserData*>, mtpRequestId> _userPhotosRequests;

	base::flat_map<
		not_null<UserData*>,
		not_null<PhotoData*>> _nonPersonalPhotos;

	mtpRequestId _requestIdEmojiList = 0;
	rpl::variable<EmojiList> _profileEmojiList;
	rpl::variable<EmojiList> _groupEmojiList;

};

} // namespace Api
