/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "ui/layers/generic_box.h"

namespace Data {
class Forum;
} // namespace Data

namespace Main {
class Session;
} // namespace Main

namespace Window {
class SessionNavigation;
} // namespace Window

void ChannelsLimitBox(
	not_null<Ui::GenericBox*> box,
	not_null<Main::Session*> session);
void PublicLinksLimitBox(
	not_null<Ui::GenericBox*> box,
	not_null<Window::SessionNavigation*> navigation,
	Fn<void()> retry);
void FilterChatsLimitBox(
	not_null<Ui::GenericBox*> box,
	not_null<Main::Session*> session,
	int currentCount);
void FilterLinksLimitBox(
	not_null<Ui::GenericBox*> box,
	not_null<Main::Session*> session);
void FiltersLimitBox(
	not_null<Ui::GenericBox*> box,
	not_null<Main::Session*> session);
void ShareableFiltersLimitBox(
	not_null<Ui::GenericBox*> box,
	not_null<Main::Session*> session);
void FilterPinsLimitBox(
	not_null<Ui::GenericBox*> box,
	not_null<Main::Session*> session,
	FilterId filterId);
void FolderPinsLimitBox(
	not_null<Ui::GenericBox*> box,
	not_null<Main::Session*> session);
void PinsLimitBox(
	not_null<Ui::GenericBox*> box,
	not_null<Main::Session*> session);
void ForumPinsLimitBox(
	not_null<Ui::GenericBox*> box,
	not_null<Data::Forum*> forum);
void CaptionLimitBox(
	not_null<Ui::GenericBox*> box,
	not_null<Main::Session*> session,
	int remove);
void CaptionLimitReachedBox(
	not_null<Ui::GenericBox*> box,
	not_null<Main::Session*> session,
	int remove);
void FileSizeLimitBox(
	not_null<Ui::GenericBox*> box,
	not_null<Main::Session*> session,
	uint64 fileSizeBytes);
void AccountsLimitBox(
	not_null<Ui::GenericBox*> box,
	not_null<Main::Session*> session);

[[nodiscard]] QString LimitsPremiumRef(const QString &addition);
