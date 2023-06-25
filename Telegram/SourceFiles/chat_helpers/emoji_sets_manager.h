/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "boxes/abstract_box.h"

namespace Main {
class Session;
} // namespace Main

namespace Ui {
namespace Emoji {

class ManageSetsBox final : public Ui::BoxContent {
public:
	ManageSetsBox(QWidget*, not_null<Main::Session*> session);

private:
	void prepare() override;

	const not_null<Main::Session*> _session;

};

void LoadAndSwitchTo(not_null<Main::Session*> session, int id);

} // namespace Emoji
} // namespace Ui
