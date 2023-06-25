/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

class History;

namespace Support {

// Returns histories().request, not api().request.
[[nodiscard]] int SendPreloadRequest(
	not_null<History*> history,
	Fn<void()> retry);

} // namespace Support
