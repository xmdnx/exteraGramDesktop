/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#include "boxes/peers/edit_members_visible.h"

#include "boxes/peers/edit_peer_info_box.h"
#include "data/data_channel.h"
#include "ui/rp_widget.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/widgets/buttons.h"
#include "settings/settings_common.h"
#include "main/main_account.h"
#include "main/main_app_config.h"
#include "main/main_session.h"
#include "apiwrap.h"
#include "lang/lang_keys.h"
#include "styles/style_info.h"

namespace {

[[nodiscard]] int EnableHideMembersMin(not_null<ChannelData*> channel) {
	return channel->session().account().appConfig().get<int>(
		u"hidden_members_group_size_min"_q,
		100);
}

} // namespace

[[nodiscard]] object_ptr<Ui::RpWidget> CreateMembersVisibleButton(
		not_null<ChannelData*> megagroup) {
	auto result = object_ptr<Ui::VerticalLayout>((QWidget*)nullptr);
	const auto container = result.data();

	const auto min = EnableHideMembersMin(megagroup);
	if (!megagroup->canBanMembers() || megagroup->membersCount() < min) {
		return { nullptr };
	}

	struct State {
		rpl::event_stream<bool> toggled;
	};
	Settings::AddSkip(container);
	const auto state = container->lifetime().make_state<State>();
	const auto button = container->add(
		EditPeerInfoBox::CreateButton(
			container,
			tr::lng_profile_hide_participants(),
			rpl::single(QString()),
			[] {},
			st::manageGroupTopicsButton,
			{ &st::infoRoundedIconHideMembers, Settings::kIconDarkBlue }
	))->toggleOn(rpl::single(
		(megagroup->flags() & ChannelDataFlag::ParticipantsHidden) != 0
	) | rpl::then(state->toggled.events()));
	Settings::AddSkip(container);
	Settings::AddDividerText(
		container,
		tr::lng_profile_hide_participants_about());

	button->toggledValue(
	) | rpl::start_with_next([=](bool toggled) {
		megagroup->session().api().request(
			MTPchannels_ToggleParticipantsHidden(
				megagroup->inputChannel,
				MTP_bool(toggled)
			)
		).done([=](const MTPUpdates &result) {
			megagroup->session().api().applyUpdates(result);
		}).send();
	}, button->lifetime());

	return result;
}
