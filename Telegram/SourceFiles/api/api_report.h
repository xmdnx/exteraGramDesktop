/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

class PeerData;
class PhotoData;

namespace Ui {
class Show;
enum class ReportReason;
} // namespace Ui

namespace Api {

void SendReport(
	std::shared_ptr<Ui::Show> show,
	not_null<PeerData*> peer,
	Ui::ReportReason reason,
	const QString &comment,
	std::variant<v::null_t, MessageIdsList, not_null<PhotoData*>> data);

} // namespace Api
