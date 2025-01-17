/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "settings/settings_type.h"

enum class PremiumPreview;

namespace Ui {
class RpWidget;
class GradientButton;
} // namespace Ui

namespace Main {
class Session;
} // namespace Main

namespace Window {
class SessionController;
} // namespace Window

namespace Settings {

[[nodiscard]] Type PremiumId();

void ShowPremium(not_null<::Main::Session*> session, const QString &ref);
void ShowPremium(
	not_null<Window::SessionController*> controller,
	const QString &ref);
void ShowGiftPremium(
	not_null<Window::SessionController*> controller,
	not_null<PeerData*> peer,
	int months,
	bool me);
void ShowEmojiStatusPremium(
	not_null<Window::SessionController*> controller,
	not_null<PeerData*> peer);

void StartPremiumPayment(
	not_null<Window::SessionController*> controller,
	const QString &ref);

[[nodiscard]] QString LookupPremiumRef(PremiumPreview section);

struct SubscribeButtonArgs final {
	not_null<Window::SessionController*> controller;
	not_null<Ui::RpWidget*> parent;
	Fn<QString()> computeRef;
	std::optional<rpl::producer<QString>> text;
	std::optional<QGradientStops> gradientStops;
	Fn<QString()> computeBotUrl; // nullable
};

[[nodiscard]] not_null<Ui::GradientButton*> CreateSubscribeButton(
	SubscribeButtonArgs &&args);

[[nodiscard]] std::vector<PremiumPreview> PremiumPreviewOrder(
	not_null<::Main::Session*> session);


} // namespace Settings

