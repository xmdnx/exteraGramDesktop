/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#include <ui/boxes/single_choice_box.h>
#include "extera/extera_settings.h"
#include "extera/extera_lang.h"
#include "extera/extera_settings_menu.h"

#include "lang_auto.h"
#include "mainwindow.h"
#include "settings/settings_common.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "boxes/connection_box.h"
#include "platform/platform_specific.h"
#include "window/window_session_controller.h"
#include "lang/lang_instance.h"
#include "core/application.h"
#include "storage/localstorage.h"
#include "data/data_session.h"
#include "main/main_session.h"
#include "styles/style_settings.h"
#include "apiwrap.h"
#include "api/api_blocked_peers.h"
#include "ui/widgets/continuous_sliders.h"

namespace Settings {

    rpl::producer<QString> Extera::title() {
        return rktr("etg_settings_extera");
    }

    Extera::Extera(
            QWidget *parent,
            not_null<Window::SessionController *> controller)
            : Section(parent) {
        setupContent(controller);
    }

    void Extera::SetupChats(not_null<Ui::VerticalLayout *> container) {
        AddSubsectionTitle(container, rktr("etg_settings_chats"));

        AddButton(
                container,
                rktr("etg_settings_adaptive_bubbles"),
                st::settingsButtonNoIcon
        )->toggleOn(
                rpl::single(::ExteraSettings::JsonSettings::GetBool("adaptive_bubbles"))
        )->toggledValue(
        ) | rpl::filter([=](bool enabled) {
            return (enabled != ::ExteraSettings::JsonSettings::GetBool("adaptive_bubbles"));
        }) | rpl::start_with_next([=](bool enabled) {
            ::ExteraSettings::JsonSettings::Set("adaptive_bubbles", enabled);
        }, container->lifetime());
    }

    void Extera::SetupExteraSettings(not_null<Ui::VerticalLayout *> container, not_null<Window::SessionController *> controller) {
        AddSkip(container);
        SetupChats(container);
    }

    void Extera::setupContent(not_null<Window::SessionController *> controller) {
        const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

        SetupExteraSettings(content, controller);

        Ui::ResizeFitChild(this, content);
    }
} // namespace Settings
