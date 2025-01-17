/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace Platform {
namespace DesktopEnvironment {

enum class Type {
	Gnome,
	Cinnamon,
	KDE,
	Unity,
	MATE,
};

std::vector<Type> Get();

inline bool IsGnome() {
	return ranges::contains(Get(), Type::Gnome);
}

inline bool IsCinnamon() {
	return ranges::contains(Get(), Type::Cinnamon);
}

inline bool IsKDE() {
	return ranges::contains(Get(), Type::KDE);
}

inline bool IsUnity() {
	return ranges::contains(Get(), Type::Unity);
}

inline bool IsMATE() {
	return ranges::contains(Get(), Type::MATE);
}

} // namespace DesktopEnvironment
} // namespace Platform
