/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace Api {
enum class SendProgressType;
struct SendOptions;
struct SendAction;
} // namespace Api

class History;

namespace HistoryView::Controls {

struct MessageToEdit {
	FullMsgId fullId;
	Api::SendOptions options;
	TextWithTags textWithTags;
};
struct VoiceToSend {
	QByteArray bytes;
	VoiceWaveform waveform;
	int duration = 0;
	Api::SendOptions options;
};
struct SendActionUpdate {
	Api::SendProgressType type = Api::SendProgressType();
	int progress = 0;
	bool cancel = false;
};

struct SetHistoryArgs {
	required<History*> history;
	MsgId topicRootId = 0;
	Fn<bool()> showSlowmodeError;
	Fn<Api::SendAction()> sendActionFactory;
	rpl::producer<int> slowmodeSecondsLeft;
	rpl::producer<bool> sendDisabledBySlowmode;
	rpl::producer<std::optional<QString>> writeRestriction;
};

struct ReplyNextRequest {
	enum class Direction {
		Next,
		Previous,
	};
	const FullMsgId replyId;
	const Direction direction;
};

} // namespace HistoryView::Controls
