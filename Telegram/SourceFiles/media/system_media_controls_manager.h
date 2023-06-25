/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "data/data_audio_msg_id.h"
#include "media/player/media_player_instance.h"
#include "media/media_common.h"

namespace base::Platform {
class SystemMediaControls;
} // namespace base::Platform

namespace Data {
class DocumentMedia;
} // namespace Data

namespace Window {
class Controller;
} // namespace Window

namespace Media::Streaming {
class Instance;
} // namespace Media::Streaming

namespace Media {

class SystemMediaControlsManager {
public:
	SystemMediaControlsManager();
	~SystemMediaControlsManager();

	static bool Supported();

private:
	const std::unique_ptr<base::Platform::SystemMediaControls> _controls;

	std::vector<std::shared_ptr<Data::DocumentMedia>> _cachedMediaView;
	std::unique_ptr<Streaming::Instance> _streamed;
	AudioMsgId _lastAudioMsgId;
	OrderMode _lastOrderMode = OrderMode::Default;

	rpl::lifetime _lifetimeDownload;
	rpl::lifetime _lifetime;
};

}  // namespace Media
