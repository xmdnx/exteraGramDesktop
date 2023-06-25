/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include <QtCore/QString>

namespace Export {
namespace Output {

struct Result {
	enum class Type : char {
		Success,
		Error,
		FatalError
	};

	Result(Type type, QString path) : path(path), type(type) {
	}

	static Result Success() {
		return Result(Type::Success, QString());
	}

	bool isSuccess() const {
		return type == Type::Success;
	}
	bool isError() const {
		return (type == Type::Error) || (type == Type::FatalError);
	}
	bool isFatalError() const {
		return (type == Type::FatalError);
	}
	explicit operator bool() const {
		return isSuccess();
	}

	QString path;
	Type type;

};

} // namespace Output
} // namespace Export
