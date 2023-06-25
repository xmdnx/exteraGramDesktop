/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

class History;

namespace Api {
struct SendOptions;
struct SendAction;
} // namespace Api

namespace Data {
class Thread;
} // namespace Data

namespace Main {
class Session;
} // namespace Main

struct PreparedServiceText {
	TextWithEntities text;
	std::vector<ClickHandlerPtr> links;
};

[[nodiscard]] MessageFlags FlagsFromMTP(
	MsgId id,
	MTPDmessage::Flags flags,
	MessageFlags localFlags);
[[nodiscard]] MessageFlags FlagsFromMTP(
	MsgId id,
	MTPDmessageService::Flags flags,
	MessageFlags localFlags);
[[nodiscard]] MTPMessageReplyHeader NewMessageReplyHeader(
	const Api::SendAction &action);

enum class MediaCheckResult {
	Good,
	Unsupported,
	Empty,
	HasTimeToLive,
};
[[nodiscard]] MediaCheckResult CheckMessageMedia(
	const MTPMessageMedia &media);
[[nodiscard]] CallId CallIdFromInput(const MTPInputGroupCall &data);

[[nodiscard]] std::vector<not_null<UserData*>> ParseInvitedToCallUsers(
	not_null<HistoryItem*> item,
	const QVector<MTPlong> &users);

inline constexpr auto kMaxUnreadReactions = 5; // Now 3, but just in case.
using OnStackUsers = std::array<UserData*, kMaxUnreadReactions>;
[[nodiscard]] OnStackUsers LookupRecentUnreadReactedUsers(
	not_null<HistoryItem*> item);
void CheckReactionNotificationSchedule(
	not_null<HistoryItem*> item,
	const OnStackUsers &wasUsers);
[[nodiscard]] MessageFlags NewForwardedFlags(
	not_null<PeerData*> peer,
	PeerId from,
	not_null<HistoryItem*> fwd);
[[nodiscard]] MessageFlags FinalizeMessageFlags(MessageFlags flags);
[[nodiscard]] bool CopyMarkupToForward(not_null<const HistoryItem*> item);
[[nodiscard]] TextWithEntities EnsureNonEmpty(
	const TextWithEntities &text = TextWithEntities());
[[nodiscard]] TextWithEntities UnsupportedMessageText();

void RequestDependentMessageData(
	not_null<HistoryItem*> item,
	PeerId peerId,
	MsgId msgId);
[[nodiscard]] MessageFlags NewMessageFlags(not_null<PeerData*> peer);
[[nodiscard]] bool ShouldSendSilent(
	not_null<PeerData*> peer,
	const Api::SendOptions &options);
[[nodiscard]] HistoryItem *LookupReplyTo(
	not_null<History*> history,
	MsgId replyToId);
[[nodiscard]] MsgId LookupReplyToTop(HistoryItem *replyTo);
[[nodiscard]] bool LookupReplyIsTopicPost(HistoryItem *replyTo);

struct SendingErrorRequest {
	MsgId topicRootId = 0;
	const HistoryItemsList *forward = nullptr;
	const TextWithTags *text = nullptr;
	bool ignoreSlowmodeCountdown = false;
};
[[nodiscard]] QString GetErrorTextForSending(
	not_null<PeerData*> peer,
	SendingErrorRequest request);
[[nodiscard]] QString GetErrorTextForSending(
	not_null<Data::Thread*> thread,
	SendingErrorRequest request);

[[nodiscard]] TextWithEntities DropCustomEmoji(TextWithEntities text);

[[nodiscard]] Main::Session *SessionByUniqueId(uint64 sessionUniqueId);
[[nodiscard]] HistoryItem *MessageByGlobalId(GlobalMsgId globalId);

[[nodiscard]] QDateTime ItemDateTime(not_null<const HistoryItem*> item);
[[nodiscard]] QString ItemDateText(
	not_null<const HistoryItem*> item,
	bool isUntilOnline);
[[nodiscard]] bool IsItemScheduledUntilOnline(
	not_null<const HistoryItem*> item);

[[nodiscard]] ClickHandlerPtr JumpToMessageClickHandler(
	not_null<PeerData*> peer,
	MsgId msgId,
	FullMsgId returnToId = FullMsgId());
[[nodiscard]] ClickHandlerPtr JumpToMessageClickHandler(
	not_null<HistoryItem*> item,
	FullMsgId returnToId = FullMsgId());

[[nodiscard]] not_null<HistoryItem*> GenerateJoinedMessage(
	not_null<History*> history,
	TimeId inviteDate,
	not_null<UserData*> inviter,
	bool viaRequest);

[[nodiscard]] std::optional<bool> PeerHasThisCall(
	not_null<PeerData*> peer,
	CallId id);
[[nodiscard]] rpl::producer<bool> PeerHasThisCallValue(
	not_null<PeerData*> peer,
	CallId id);
[[nodiscard]] ClickHandlerPtr GroupCallClickHandler(
	not_null<PeerData*> peer,
	CallId callId);
