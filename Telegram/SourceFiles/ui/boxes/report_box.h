/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace Ui {

class GenericBox;

enum class ReportSource {
	Message,
	Channel,
	Group,
	Bot,
	ProfilePhoto,
	ProfileVideo,
	GroupPhoto,
	GroupVideo,
	ChannelPhoto,
	ChannelVideo,
};

enum class ReportReason {
	Spam,
	Fake,
	Violence,
	ChildAbuse,
	Pornography,
	Copyright,
	IllegalDrugs,
	PersonalDetails,
	Other,
};

void ReportReasonBox(
	not_null<GenericBox*> box,
	ReportSource source,
	Fn<void(ReportReason)> done);

void ReportDetailsBox(
	not_null<GenericBox*> box,
	Fn<void(QString)> done);

} // namespace Ui
