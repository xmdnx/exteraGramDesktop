/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#include "data/data_premium_limits.h"

#include "main/main_account.h"
#include "main/main_app_config.h"
#include "main/main_session.h"

namespace Data {

PremiumLimits::PremiumLimits(not_null<Main::Session*> session)
: _session(session) {
}

int PremiumLimits::channelsDefault() const {
	return appConfigLimit("channels_limit_default", 500);
}
int PremiumLimits::channelsPremium() const {
	return appConfigLimit("channels_limit_premium", 1000);
}
int PremiumLimits::channelsCurrent() const {
	return isPremium()
		? channelsPremium()
		: channelsDefault();
}

int PremiumLimits::gifsDefault() const {
	return appConfigLimit("saved_gifs_limit_default", 200);
}
int PremiumLimits::gifsPremium() const {
	return appConfigLimit("saved_gifs_limit_premium", 400);
}
int PremiumLimits::gifsCurrent() const {
	return isPremium()
		? gifsPremium()
		: gifsDefault();
}

int PremiumLimits::stickersFavedDefault() const {
	return appConfigLimit("stickers_faved_limit_default", 5);
}
int PremiumLimits::stickersFavedPremium() const {
	return appConfigLimit("stickers_faved_limit_premium", 10);
}
int PremiumLimits::stickersFavedCurrent() const {
	return isPremium()
		? stickersFavedPremium()
		: stickersFavedDefault();
}

int PremiumLimits::dialogFiltersDefault() const {
	return appConfigLimit("dialog_filters_limit_default", 10);
}
int PremiumLimits::dialogFiltersPremium() const {
	return appConfigLimit("dialog_filters_limit_premium", 20);
}
int PremiumLimits::dialogFiltersCurrent() const {
	return isPremium()
		? dialogFiltersPremium()
		: dialogFiltersDefault();
}

int PremiumLimits::dialogShareableFiltersDefault() const {
	return appConfigLimit("chatlists_joined_limit_default", 2);
}
int PremiumLimits::dialogShareableFiltersPremium() const {
	return appConfigLimit("chatlists_joined_limit_premium", 20);
}
int PremiumLimits::dialogShareableFiltersCurrent() const {
	return isPremium()
		? dialogShareableFiltersPremium()
		: dialogShareableFiltersDefault();
}

int PremiumLimits::dialogFiltersChatsDefault() const {
	return appConfigLimit("dialog_filters_chats_limit_default", 100);
}
int PremiumLimits::dialogFiltersChatsPremium() const {
	return appConfigLimit("dialog_filters_chats_limit_premium", 200);
}
int PremiumLimits::dialogFiltersChatsCurrent() const {
	return isPremium()
		? dialogFiltersChatsPremium()
		: dialogFiltersChatsDefault();
}

int PremiumLimits::dialogFiltersLinksDefault() const {
	return appConfigLimit("chatlist_invites_limit_default", 3);
}
int PremiumLimits::dialogFiltersLinksPremium() const {
	return appConfigLimit("chatlist_invites_limit_premium", 20);
}
int PremiumLimits::dialogFiltersLinksCurrent() const {
	return isPremium()
		? dialogFiltersLinksPremium()
		: dialogFiltersLinksDefault();
}

int PremiumLimits::dialogsPinnedDefault() const {
	return appConfigLimit("dialogs_pinned_limit_default", 5);
}
int PremiumLimits::dialogsPinnedPremium() const {
	return appConfigLimit("dialogs_pinned_limit_premium", 10);
}
int PremiumLimits::dialogsPinnedCurrent() const {
	return isPremium()
		? dialogsPinnedPremium()
		: dialogsPinnedDefault();
}

int PremiumLimits::dialogsFolderPinnedDefault() const {
	return appConfigLimit("dialogs_folder_pinned_limit_default", 100);
}
int PremiumLimits::dialogsFolderPinnedPremium() const {
	return appConfigLimit("dialogs_folder_pinned_limit_premium", 200);
}
int PremiumLimits::dialogsFolderPinnedCurrent() const {
	return isPremium()
		? dialogsFolderPinnedPremium()
		: dialogsFolderPinnedDefault();
}

int PremiumLimits::topicsPinnedCurrent() const {
	return appConfigLimit("topics_pinned_limit", 5);
}

int PremiumLimits::channelsPublicDefault() const {
	return appConfigLimit("channels_public_limit_default", 10);
}
int PremiumLimits::channelsPublicPremium() const {
	return appConfigLimit("channels_public_limit_premium", 20);
}
int PremiumLimits::channelsPublicCurrent() const {
	return isPremium()
		? channelsPublicPremium()
		: channelsPublicDefault();
}

int PremiumLimits::captionLengthDefault() const {
	return appConfigLimit("caption_length_limit_default", 1024);
}
int PremiumLimits::captionLengthPremium() const {
	return appConfigLimit("caption_length_limit_premium", 2048);
}
int PremiumLimits::captionLengthCurrent() const {
	return isPremium()
		? captionLengthPremium()
		: captionLengthDefault();
}

int PremiumLimits::uploadMaxDefault() const {
	return appConfigLimit("upload_max_fileparts_default", 4000);
}
int PremiumLimits::uploadMaxPremium() const {
	return appConfigLimit("upload_max_fileparts_premium", 8000);
}
int PremiumLimits::uploadMaxCurrent() const {
	return isPremium()
		? uploadMaxPremium()
		: uploadMaxDefault();
}

int PremiumLimits::aboutLengthDefault() const {
	return appConfigLimit("about_length_limit_default", 70);
}
int PremiumLimits::aboutLengthPremium() const {
	return appConfigLimit("about_length_limit_premium", 140);
}
int PremiumLimits::aboutLengthCurrent() const {
	return isPremium()
		? aboutLengthPremium()
		: aboutLengthDefault();
}

int PremiumLimits::appConfigLimit(
		const QString &key,
		int fallback) const {
	return _session->account().appConfig().get<int>(key, fallback);
}

bool PremiumLimits::isPremium() const {
	return _session->premium();
}

} // namespace Data
