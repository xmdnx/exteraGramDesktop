/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#include "history/history_item_helpers.h"

#include "calls/calls_instance.h"
#include "data/notify/data_notify_settings.h"
#include "data/data_chat_participant_status.h"
#include "data/data_channel.h"
#include "data/data_chat.h"
#include "data/data_changes.h"
#include "data/data_group_call.h"
#include "data/data_forum.h"
#include "data/data_forum_topic.h"
#include "data/data_media_types.h"
#include "data/data_message_reactions.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/history_item_components.h"
#include "main/main_account.h"
#include "main/main_domain.h"
#include "main/main_session.h"
#include "main/main_session_settings.h"
#include "window/window_controller.h"
#include "window/window_session_controller.h"
#include "apiwrap.h"
#include "base/unixtime.h"
#include "core/application.h"
#include "ui/text/format_values.h"
#include "ui/text/text_utilities.h"
#include "ui/text/text_entity.h"
#include "ui/item_text_options.h"
#include "lang/lang_keys.h"

namespace {

bool PeerCallKnown(not_null<PeerData*> peer) {
	if (peer->groupCall() != nullptr) {
		return true;
	} else if (const auto chat = peer->asChat()) {
		return !(chat->flags() & ChatDataFlag::CallActive);
	} else if (const auto channel = peer->asChannel()) {
		return !(channel->flags() & ChannelDataFlag::CallActive);
	}
	return true;
}

} // namespace

QString GetErrorTextForSending(
		not_null<PeerData*> peer,
		SendingErrorRequest request) {
	const auto forum = request.topicRootId ? peer->forum() : nullptr;
	const auto topic = forum
		? forum->topicFor(request.topicRootId)
		: nullptr;
	const auto thread = topic
		? not_null<Data::Thread*>(topic)
		: peer->owner().history(peer);
	if (request.forward) {
		for (const auto &item : *request.forward) {
			if (const auto error = item->errorTextForForward(thread)) {
				return *error;
			}
		}
	}
	const auto hasText = (request.text && !request.text->empty());
	if (hasText) {
		const auto error = Data::RestrictionError(
			peer,
			ChatRestriction::SendOther);
		if (error) {
			return *error;
		} else if (!Data::CanSendTexts(thread)) {
			return tr::lng_forward_cant(tr::now);
		}
	}
	if (peer->slowmodeApplied()) {
		const auto count = (hasText ? 1 : 0)
			+ (request.forward ? int(request.forward->size()) : 0);
		if (const auto history = peer->owner().historyLoaded(peer)) {
			if (!request.ignoreSlowmodeCountdown
				&& (history->latestSendingMessage() != nullptr)
				&& (count > 0)) {
				return tr::lng_slowmode_no_many(tr::now);
			}
		}
		if (request.text && request.text->text.size() > MaxMessageSize) {
			return tr::lng_slowmode_too_long(tr::now);
		} else if (hasText && count > 1) {
			return tr::lng_slowmode_no_many(tr::now);
		} else if (count > 1) {
			const auto albumForward = [&] {
				const auto first = request.forward->front();
				if (const auto groupId = first->groupId()) {
					for (const auto &item : *request.forward) {
						if (item->groupId() != groupId) {
							return false;
						}
					}
					return true;
				}
				return false;
			}();
			if (!albumForward) {
				return tr::lng_slowmode_no_many(tr::now);
			}
		}
	}
	if (const auto left = peer->slowmodeSecondsLeft()) {
		if (!request.ignoreSlowmodeCountdown) {
			return tr::lng_slowmode_enabled(
				tr::now,
				lt_left,
				Ui::FormatDurationWordsSlowmode(left));
		}
	}

	return QString();
}

QString GetErrorTextForSending(
		not_null<Data::Thread*> thread,
		SendingErrorRequest request) {
	request.topicRootId = thread->topicRootId();
	return GetErrorTextForSending(thread->peer(), std::move(request));
}

void RequestDependentMessageData(
		not_null<HistoryItem*> item,
		PeerId peerId,
		MsgId msgId) {
	if (!IsServerMsgId(msgId)) {
		return;
	}
	const auto fullId = item->fullId();
	const auto history = item->history();
	const auto session = &history->session();
	const auto done = [=] {
		if (const auto item = session->data().message(fullId)) {
			item->updateDependencyItem();
		}
	};
	history->session().api().requestMessageData(
		(peerId ? history->owner().peer(peerId) : history->peer),
		msgId,
		done);
}

MessageFlags NewMessageFlags(not_null<PeerData*> peer) {
	return MessageFlag::BeingSent
		| (peer->isSelf() ? MessageFlag() : MessageFlag::Outgoing);
}

bool ShouldSendSilent(
		not_null<PeerData*> peer,
		const Api::SendOptions &options) {
	return options.silent
		|| (peer->isBroadcast()
			&& peer->owner().notifySettings().silentPosts(peer))
		|| (peer->session().supportMode()
			&& peer->session().settings().supportAllSilent());
}

HistoryItem *LookupReplyTo(not_null<History*> history, MsgId replyToId) {
	const auto &owner = history->owner();
	return owner.message(history->peer, replyToId);
}

MsgId LookupReplyToTop(HistoryItem *replyTo) {
	return replyTo ? replyTo->replyToTop() : 0;
}

bool LookupReplyIsTopicPost(HistoryItem *replyTo) {
	return replyTo
		&& (replyTo->topicRootId() != Data::ForumTopic::kGeneralId);
}

TextWithEntities DropCustomEmoji(TextWithEntities text) {
	text.entities.erase(
		ranges::remove(
			text.entities,
			EntityType::CustomEmoji,
			&EntityInText::type),
		text.entities.end());
	return text;
}

Main::Session *SessionByUniqueId(uint64 sessionUniqueId) {
	if (!sessionUniqueId) {
		return nullptr;
	}
	for (const auto &[index, account] : Core::App().domain().accounts()) {
		if (const auto session = account->maybeSession()) {
			if (session->uniqueId() == sessionUniqueId) {
				return session;
			}
		}
	}
	return nullptr;
}

HistoryItem *MessageByGlobalId(GlobalMsgId globalId) {
	const auto sessionId = globalId.itemId ? globalId.sessionUniqueId : 0;
	if (const auto session = SessionByUniqueId(sessionId)) {
		return session->data().message(globalId.itemId);
	}
	return nullptr;
}

QDateTime ItemDateTime(not_null<const HistoryItem*> item) {
	return base::unixtime::parse(item->date());
}

QString ItemDateText(not_null<const HistoryItem*> item, bool isUntilOnline) {
	const auto dateText = langDayOfMonthFull(ItemDateTime(item).date());
	return !item->isScheduled()
		? dateText
		: isUntilOnline
			? tr::lng_scheduled_date_until_online(tr::now)
			: tr::lng_scheduled_date(tr::now, lt_date, dateText);
}

bool IsItemScheduledUntilOnline(not_null<const HistoryItem*> item) {
	return item->isScheduled()
		&& (item->date() == Api::kScheduledUntilOnlineTimestamp);
}

ClickHandlerPtr JumpToMessageClickHandler(
		not_null<HistoryItem*> item,
		FullMsgId returnToId) {
	return JumpToMessageClickHandler(
		item->history()->peer,
		item->id,
		returnToId);
}

ClickHandlerPtr JumpToMessageClickHandler(
		not_null<PeerData*> peer,
		MsgId msgId,
		FullMsgId returnToId) {
	return std::make_shared<LambdaClickHandler>([=] {
		const auto separate = Core::App().separateWindowForPeer(peer);
		const auto controller = separate
			? separate->sessionController()
			: peer->session().tryResolveWindow();
		if (controller) {
			auto params = Window::SectionShow{
				Window::SectionShow::Way::Forward
			};
			params.origin = Window::SectionShow::OriginMessage{
				returnToId
			};
			if (const auto item = peer->owner().message(peer, msgId)) {
				controller->showMessage(item, params);
			} else {
				controller->showPeerHistory(peer, params, msgId);
			}
		}
	});
}

MessageFlags FlagsFromMTP(
		MsgId id,
		MTPDmessage::Flags flags,
		MessageFlags localFlags) {
	using Flag = MessageFlag;
	using MTP = MTPDmessage::Flag;
	return localFlags
		| (IsServerMsgId(id) ? Flag::HistoryEntry : Flag())
		| ((flags & MTP::f_out) ? Flag::Outgoing : Flag())
		| ((flags & MTP::f_mentioned) ? Flag::MentionsMe : Flag())
		| ((flags & MTP::f_media_unread) ? Flag::MediaIsUnread : Flag())
		| ((flags & MTP::f_silent) ? Flag::Silent : Flag())
		| ((flags & MTP::f_post) ? Flag::Post : Flag())
		| ((flags & MTP::f_legacy) ? Flag::Legacy : Flag())
		| ((flags & MTP::f_edit_hide) ? Flag::HideEdited : Flag())
		| ((flags & MTP::f_pinned) ? Flag::Pinned : Flag())
		| ((flags & MTP::f_from_id) ? Flag::HasFromId : Flag())
		| ((flags & MTP::f_reply_to) ? Flag::HasReplyInfo : Flag())
		| ((flags & MTP::f_reply_markup) ? Flag::HasReplyMarkup : Flag())
		| ((flags & MTP::f_from_scheduled) ? Flag::IsOrWasScheduled : Flag())
		| ((flags & MTP::f_views) ? Flag::HasViews : Flag())
		| ((flags & MTP::f_noforwards) ? Flag::NoForwards : Flag());
}

MessageFlags FlagsFromMTP(
		MsgId id,
		MTPDmessageService::Flags flags,
		MessageFlags localFlags) {
	using Flag = MessageFlag;
	using MTP = MTPDmessageService::Flag;
	return localFlags
		| (IsServerMsgId(id) ? Flag::HistoryEntry : Flag())
		| ((flags & MTP::f_out) ? Flag::Outgoing : Flag())
		| ((flags & MTP::f_mentioned) ? Flag::MentionsMe : Flag())
		| ((flags & MTP::f_media_unread) ? Flag::MediaIsUnread : Flag())
		| ((flags & MTP::f_silent) ? Flag::Silent : Flag())
		| ((flags & MTP::f_post) ? Flag::Post : Flag())
		| ((flags & MTP::f_legacy) ? Flag::Legacy : Flag())
		| ((flags & MTP::f_from_id) ? Flag::HasFromId : Flag())
		| ((flags & MTP::f_reply_to) ? Flag::HasReplyInfo : Flag());
}

MTPMessageReplyHeader NewMessageReplyHeader(const Api::SendAction &action) {
	if (const auto id = action.replyTo) {
		const auto to = LookupReplyTo(action.history, id);
		if (const auto replyToTop = LookupReplyToTop(to)) {
			using Flag = MTPDmessageReplyHeader::Flag;
			return MTP_messageReplyHeader(
				MTP_flags(Flag::f_reply_to_top_id
					| (LookupReplyIsTopicPost(to)
						? Flag::f_forum_topic
						: Flag(0))),
				MTP_int(id),
				MTPPeer(),
				MTP_int(replyToTop));
		}
		return MTP_messageReplyHeader(
			MTP_flags(0),
			MTP_int(id),
			MTPPeer(),
			MTPint());
	}
	return MTPMessageReplyHeader();
}

MediaCheckResult CheckMessageMedia(const MTPMessageMedia &media) {
	using Result = MediaCheckResult;
	return media.match([](const MTPDmessageMediaEmpty &) {
		return Result::Good;
	}, [](const MTPDmessageMediaContact &) {
		return Result::Good;
	}, [](const MTPDmessageMediaGeo &data) {
		return data.vgeo().match([](const MTPDgeoPoint &) {
			return Result::Good;
		}, [](const MTPDgeoPointEmpty &) {
			return Result::Empty;
		});
	}, [](const MTPDmessageMediaVenue &data) {
		return data.vgeo().match([](const MTPDgeoPoint &) {
			return Result::Good;
		}, [](const MTPDgeoPointEmpty &) {
			return Result::Empty;
		});
	}, [](const MTPDmessageMediaGeoLive &data) {
		return data.vgeo().match([](const MTPDgeoPoint &) {
			return Result::Good;
		}, [](const MTPDgeoPointEmpty &) {
			return Result::Empty;
		});
	}, [](const MTPDmessageMediaPhoto &data) {
		const auto photo = data.vphoto();
		if (data.vttl_seconds()) {
			return Result::HasTimeToLive;
		} else if (!photo) {
			return Result::Empty;
		}
		return photo->match([](const MTPDphoto &) {
			return Result::Good;
		}, [](const MTPDphotoEmpty &) {
			return Result::Empty;
		});
	}, [](const MTPDmessageMediaDocument &data) {
		const auto document = data.vdocument();
		if (data.vttl_seconds()) {
			return Result::HasTimeToLive;
		} else if (!document) {
			return Result::Empty;
		}
		return document->match([](const MTPDdocument &) {
			return Result::Good;
		}, [](const MTPDdocumentEmpty &) {
			return Result::Empty;
		});
	}, [](const MTPDmessageMediaWebPage &data) {
		return data.vwebpage().match([](const MTPDwebPage &) {
			return Result::Good;
		}, [](const MTPDwebPageEmpty &) {
			return Result::Good;
		}, [](const MTPDwebPagePending &) {
			return Result::Good;
		}, [](const MTPDwebPageNotModified &) {
			return Result::Unsupported;
		});
	}, [](const MTPDmessageMediaGame &data) {
		return data.vgame().match([](const MTPDgame &) {
			return Result::Good;
		});
	}, [](const MTPDmessageMediaInvoice &) {
		return Result::Good;
	}, [](const MTPDmessageMediaPoll &) {
		return Result::Good;
	}, [](const MTPDmessageMediaDice &) {
		return Result::Good;
	}, [](const MTPDmessageMediaUnsupported &) {
		return Result::Unsupported;
	});
}

[[nodiscard]] CallId CallIdFromInput(const MTPInputGroupCall &data) {
	return data.match([&](const MTPDinputGroupCall &data) {
		return data.vid().v;
	});
}

std::vector<not_null<UserData*>> ParseInvitedToCallUsers(
		not_null<HistoryItem*> item,
		const QVector<MTPlong> &users) {
	auto &owner = item->history()->owner();
	return ranges::views::all(
		users
	) | ranges::views::transform([&](const MTPlong &id) {
		return owner.user(id.v);
	}) | ranges::to_vector;
}

PreparedServiceText GenerateJoinedText(
		not_null<History*> history,
		not_null<UserData*> inviter,
		bool viaRequest) {
	if (inviter->id != history->session().userPeerId()) {
		auto result = PreparedServiceText();
		result.links.push_back(inviter->createOpenLink());
		result.text = (history->peer->isMegagroup()
			? tr::lng_action_add_you_group
			: tr::lng_action_add_you)(
				tr::now,
				lt_from,
				Ui::Text::Link(inviter->name(), QString()),
				Ui::Text::WithEntities);
		return result;
	} else if (history->peer->isMegagroup()) {
		if (viaRequest) {
			return { tr::lng_action_you_joined_by_request(
				tr::now,
				Ui::Text::WithEntities) };
		}
		auto self = history->session().user();
		auto result = PreparedServiceText();
		result.links.push_back(self->createOpenLink());
		result.text = tr::lng_action_user_joined(
			tr::now,
			lt_from,
			Ui::Text::Link(self->name(), QString()),
			Ui::Text::WithEntities);
		return result;
	}
	return { viaRequest
		? tr::lng_action_you_joined_by_request_channel(
			tr::now,
			Ui::Text::WithEntities)
		: tr::lng_action_you_joined(tr::now, Ui::Text::WithEntities) };
}

not_null<HistoryItem*> GenerateJoinedMessage(
		not_null<History*> history,
		TimeId inviteDate,
		not_null<UserData*> inviter,
		bool viaRequest) {
	return history->makeMessage(
		history->owner().nextLocalMessageId(),
		MessageFlag::Local,
		inviteDate,
		GenerateJoinedText(history, inviter, viaRequest));
}

std::optional<bool> PeerHasThisCall(
		not_null<PeerData*> peer,
		CallId id) {
	const auto call = peer->groupCall();
	return call
		? std::make_optional(call->id() == id)
		: PeerCallKnown(peer)
		? std::make_optional(false)
		: std::nullopt;
}
[[nodiscard]] rpl::producer<bool> PeerHasThisCallValue(
		not_null<PeerData*> peer,
		CallId id) {
	return peer->session().changes().peerFlagsValue(
		peer,
		Data::PeerUpdate::Flag::GroupCall
	) | rpl::filter([=] {
		return PeerCallKnown(peer);
	}) | rpl::map([=] {
		const auto call = peer->groupCall();
		return (call && call->id() == id);
	}) | rpl::distinct_until_changed(
	) | rpl::take_while([=](bool hasThisCall) {
		return hasThisCall;
	}) | rpl::then(
		rpl::single(false)
	);
}

[[nodiscard]] ClickHandlerPtr GroupCallClickHandler(
		not_null<PeerData*> peer,
		CallId callId) {
	return std::make_shared<LambdaClickHandler>([=] {
		const auto call = peer->groupCall();
		if (call && call->id() == callId) {
			const auto &windows = peer->session().windows();
			if (windows.empty()) {
				Core::App().domain().activate(&peer->session().account());
				if (windows.empty()) {
					return;
				}
			}
			windows.front()->startOrJoinGroupCall(peer, {});
		}
	});
}

[[nodiscard]] MessageFlags FinalizeMessageFlags(MessageFlags flags) {
	if (!(flags & MessageFlag::FakeHistoryItem)
		&& !(flags & MessageFlag::IsOrWasScheduled)
		&& !(flags & MessageFlag::AdminLogEntry)) {
		flags |= MessageFlag::HistoryEntry;
	}
	return flags;
}

using OnStackUsers = std::array<UserData*, kMaxUnreadReactions>;
[[nodiscard]] OnStackUsers LookupRecentUnreadReactedUsers(
		not_null<HistoryItem*> item) {
	auto result = OnStackUsers();
	auto index = 0;
	for (const auto &[emoji, reactions] : item->recentReactions()) {
		for (const auto &reaction : reactions) {
			if (!reaction.unread) {
				continue;
			}
			if (const auto user = reaction.peer->asUser()) {
				result[index++] = user;
				if (index == result.size()) {
					return result;
				}
			}
		}
	}
	return result;
}

void CheckReactionNotificationSchedule(
		not_null<HistoryItem*> item,
		const OnStackUsers &wasUsers) {
	// Call to addToUnreadThings may have read the reaction already.
	if (!item->hasUnreadReaction()) {
		return;
	}
	for (const auto &[emoji, reactions] : item->recentReactions()) {
		for (const auto &reaction : reactions) {
			if (!reaction.unread) {
				continue;
			}
			const auto user = reaction.peer->asUser();
			if (!user
				|| !user->isContact()
				|| ranges::contains(wasUsers, user)) {
				continue;
			}
			using Status = PeerData::BlockStatus;
			if (user->blockStatus() == Status::Unknown) {
				user->updateFull();
			}
			const auto notification = Data::ItemNotification{
				.item = item,
				.reactionSender = user,
				.type = Data::ItemNotificationType::Reaction,
			};
			item->notificationThread()->pushNotification(notification);
			Core::App().notifications().schedule(notification);
			return;
		}
	}
}

[[nodiscard]] MessageFlags NewForwardedFlags(
		not_null<PeerData*> peer,
		PeerId from,
		not_null<HistoryItem*> fwd) {
	auto result = NewMessageFlags(peer);
	if (from) {
		result |= MessageFlag::HasFromId;
	}
	if (const auto media = fwd->media()) {
		if ((!peer->isChannel() || peer->isMegagroup())
			&& media->forwardedBecomesUnread()) {
			result |= MessageFlag::MediaIsUnread;
		}
	}
	if (fwd->hasViews()) {
		result |= MessageFlag::HasViews;
	}
	return result;
}

[[nodiscard]] bool CopyMarkupToForward(not_null<const HistoryItem*> item) {
	auto mediaOriginal = item->media();
	if (mediaOriginal && mediaOriginal->game()) {
		// Copy inline keyboard when forwarding messages with a game.
		return true;
	}
	const auto markup = item->inlineReplyMarkup();
	if (!markup) {
		return false;
	}
	using Type = HistoryMessageMarkupButton::Type;
	for (const auto &row : markup->data.rows) {
		for (const auto &button : row) {
			const auto switchInline = (button.type == Type::SwitchInline)
				|| (button.type == Type::SwitchInlineSame);
			const auto url = (button.type == Type::Url)
				|| (button.type == Type::Auth);
			if ((!switchInline || !item->viaBot()) && !url) {
				return false;
			}
		}
	}
	return true;
}

[[nodiscard]] TextWithEntities EnsureNonEmpty(
		const TextWithEntities &text) {
	return !text.text.isEmpty() ? text : TextWithEntities{ u":-("_q };
}

[[nodiscard]] TextWithEntities UnsupportedMessageText() {
	const auto siteLink = u"https://desktop.telegram.org"_q;
	auto result = TextWithEntities{
		tr::lng_message_unsupported(tr::now, lt_link, siteLink)
	};
	TextUtilities::ParseEntities(result, Ui::ItemTextNoMonoOptions().flags);
	result.entities.push_front(
		EntityInText(EntityType::Italic, 0, result.text.size()));
	return result;
}
