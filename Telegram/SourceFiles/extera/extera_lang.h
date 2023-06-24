/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

namespace Extera {
namespace Lang {

struct Var {
	Var() {};
	Var(const QString &k, const QString &v) {
		key = k;
		value = v;
	}

	QString key;
	QString value;
};

struct EntVar {
	EntVar() {};
	EntVar(const QString &k, TextWithEntities v) {
		key = k;
		value = v;
	}

	QString key;
	TextWithEntities value;
};

void Load(const QString &baseLangCode, const QString &langCode);

QString Translate(
	const QString &key,
	Var var1 = Var(),
	Var var2 = Var(),
	Var var3 = Var(),
	Var var4 = Var());
QString Translate(
	const QString &key,
	float64 value,
	Var var1 = Var(),
	Var var2 = Var(),
	Var var3 = Var(),
	Var var4 = Var());

TextWithEntities TranslateWithEntities(
	const QString &key,
	EntVar var1 = EntVar(),
	EntVar var2 = EntVar(),
	EntVar var3 = EntVar(),
	EntVar var4 = EntVar());
TextWithEntities TranslateWithEntities(
	const QString &key,
	float64 value,
	EntVar var1 = EntVar(),
	EntVar var2 = EntVar(),
	EntVar var3 = EntVar(),
	EntVar var4 = EntVar());

rpl::producer<> Events();

} // namespace Lang
} // namespace Extera

// Shorthands

inline QString ktr(
	const QString &key,
	::Extera::Lang::Var var1 = ::Extera::Lang::Var(),
	::Extera::Lang::Var var2 = ::Extera::Lang::Var(),
	::Extera::Lang::Var var3 = ::Extera::Lang::Var(),
	::Extera::Lang::Var var4 = ::Extera::Lang::Var()) {
	return ::Extera::Lang::Translate(key, var1, var2, var3, var4);
}

inline QString ktr(
	const QString &key,
	float64 value,
	::Extera::Lang::Var var1 = ::Extera::Lang::Var(),
	::Extera::Lang::Var var2 = ::Extera::Lang::Var(),
	::Extera::Lang::Var var3 = ::Extera::Lang::Var(),
	::Extera::Lang::Var var4 = ::Extera::Lang::Var()) {
	return ::Extera::Lang::Translate(key, value, var1, var2, var3, var4);
}

inline TextWithEntities ktre(
	const QString &key,
	::Extera::Lang::EntVar var1 = ::Extera::Lang::EntVar(),
	::Extera::Lang::EntVar var2 = ::Extera::Lang::EntVar(),
	::Extera::Lang::EntVar var3 = ::Extera::Lang::EntVar(),
	::Extera::Lang::EntVar var4 = ::Extera::Lang::EntVar()) {
	return ::Extera::Lang::TranslateWithEntities(key, var1, var2, var3, var4);
}

inline TextWithEntities ktre(
	const QString &key,
	float64 value,
	::Extera::Lang::EntVar var1 = ::Extera::Lang::EntVar(),
	::Extera::Lang::EntVar var2 = ::Extera::Lang::EntVar(),
	::Extera::Lang::EntVar var3 = ::Extera::Lang::EntVar(),
	::Extera::Lang::EntVar var4 = ::Extera::Lang::EntVar()) {
	return ::Extera::Lang::TranslateWithEntities(key, value, var1, var2, var3, var4);
}

inline rpl::producer<QString> rktr(
	const QString &key,
	::Extera::Lang::Var var1 = ::Extera::Lang::Var(),
	::Extera::Lang::Var var2 = ::Extera::Lang::Var(),
	::Extera::Lang::Var var3 = ::Extera::Lang::Var(),
	::Extera::Lang::Var var4 = ::Extera::Lang::Var()) {
	return rpl::single(
			::Extera::Lang::Translate(key, var1, var2, var3, var4)
		) | rpl::then(
			::Extera::Lang::Events() | rpl::map(
				[=]{ return ::Extera::Lang::Translate(key, var1, var2, var3, var4); })
		);
}

inline rpl::producer<QString> rktr(
	const QString &key,
	float64 value,
	::Extera::Lang::Var var1 = ::Extera::Lang::Var(),
	::Extera::Lang::Var var2 = ::Extera::Lang::Var(),
	::Extera::Lang::Var var3 = ::Extera::Lang::Var(),
	::Extera::Lang::Var var4 = ::Extera::Lang::Var()) {
	return rpl::single(
			::Extera::Lang::Translate(key, value, var1, var2, var3, var4)
		) | rpl::then(
			::Extera::Lang::Events() | rpl::map(
				[=]{ return ::Extera::Lang::Translate(key, value, var1, var2, var3, var4); })
		);
}

inline rpl::producer<TextWithEntities> rktre(
	const QString &key,
	::Extera::Lang::EntVar var1 = ::Extera::Lang::EntVar(),
	::Extera::Lang::EntVar var2 = ::Extera::Lang::EntVar(),
	::Extera::Lang::EntVar var3 = ::Extera::Lang::EntVar(),
	::Extera::Lang::EntVar var4 = ::Extera::Lang::EntVar()) {
	return rpl::single(
			::Extera::Lang::TranslateWithEntities(key, var1, var2, var3, var4)
		) | rpl::then(
			::Extera::Lang::Events() | rpl::map(
				[=]{ return ::Extera::Lang::TranslateWithEntities(key, var1, var2, var3, var4); })
		);
}

inline rpl::producer<TextWithEntities> rktre(
	const QString &key,
	float64 value,
	::Extera::Lang::EntVar var1 = ::Extera::Lang::EntVar(),
	::Extera::Lang::EntVar var2 = ::Extera::Lang::EntVar(),
	::Extera::Lang::EntVar var3 = ::Extera::Lang::EntVar(),
	::Extera::Lang::EntVar var4 = ::Extera::Lang::EntVar()) {
	return rpl::single(
			::Extera::Lang::TranslateWithEntities(key, value, var1, var2, var3, var4)
		) | rpl::then(
			::Extera::Lang::Events() | rpl::map(
				[=]{ return ::Extera::Lang::TranslateWithEntities(key, value, var1, var2, var3, var4); })
		);
}