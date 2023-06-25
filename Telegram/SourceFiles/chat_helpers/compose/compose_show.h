/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "base/flags.h"
#include "main/session/session_show.h"

class PhotoData;
class DocumentData;

namespace Data {
struct FileOrigin;
} // namespace Data

namespace Window {
class SessionController;
} // namespace Window

namespace SendMenu {
enum class Type;
} // namespace SendMenu

namespace ChatHelpers {

struct FileChosen;

enum class PauseReason {
	Any = 0,
	InlineResults = (1 << 0),
	TabbedPanel = (1 << 1),
	Layer = (1 << 2),
	RoundPlaying = (1 << 3),
	MediaPreview = (1 << 4),
};
using PauseReasons = base::flags<PauseReason>;
inline constexpr bool is_flag_type(PauseReason) { return true; };

enum class WindowUsage {
	PremiumPromo,
};

class Show : public Main::SessionShow {
public:
	[[nodiscard]] virtual bool paused(PauseReason reason) const = 0;
	[[nodiscard]] virtual rpl::producer<> pauseChanged() const = 0;

	[[nodiscard]] virtual rpl::producer<bool> adjustShadowLeft() const;
	[[nodiscard]] virtual SendMenu::Type sendMenuType() const = 0;

	virtual bool showMediaPreview(
		Data::FileOrigin origin,
		not_null<DocumentData*> document) const = 0;
	virtual bool showMediaPreview(
		Data::FileOrigin origin,
		not_null<PhotoData*> photo) const = 0;

	virtual void processChosenSticker(FileChosen chosen) const = 0;

	[[nodiscard]] virtual Window::SessionController *resolveWindow(
		WindowUsage) const;
};

} // namespace ChatHelpers
