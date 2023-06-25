/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "base/object_ptr.h"

class History;
class PeerData;
struct LanguageId;

namespace Ui {

class BoxContent;
class GenericBox;

void TranslateBox(
	not_null<GenericBox*> box,
	not_null<PeerData*> peer,
	MsgId msgId,
	TextWithEntities text,
	bool hasCopyRestriction);

[[nodiscard]] bool SkipTranslate(TextWithEntities textWithEntities);

[[nodiscard]] object_ptr<BoxContent> EditSkipTranslationLanguages();
[[nodiscard]] object_ptr<BoxContent> ChooseTranslateToBox(
	LanguageId bringUp,
	Fn<void(LanguageId)> callback);

[[nodiscard]] LanguageId ChooseTranslateTo(not_null<History*> history);
[[nodiscard]] LanguageId ChooseTranslateTo(LanguageId offeredFrom);
[[nodiscard]] LanguageId ChooseTranslateTo(
	not_null<History*> history,
	LanguageId savedTo,
	const std::vector<LanguageId> &skip);
[[nodiscard]] LanguageId ChooseTranslateTo(
	LanguageId offeredFrom,
	LanguageId savedTo,
	const std::vector<LanguageId> &skip);

} // namespace Ui
