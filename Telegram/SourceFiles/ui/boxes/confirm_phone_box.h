/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "boxes/abstract_box.h"
#include "ui/widgets/sent_code_field.h"

namespace Ui {

class FlatLabel;
class RoundButton;

class ConfirmPhoneBox final : public Ui::BoxContent {
public:
	ConfirmPhoneBox(
		QWidget*,
		const QString &phone,
		int codeLength,
		const QString &openUrl,
		std::optional<int> timeout);

	[[nodiscard]] rpl::producer<QString> checkRequests() const;
	[[nodiscard]] rpl::producer<> resendRequests() const;

	void callDone();
	void showServerError(const QString &text);

protected:
	void prepare() override;
	void setInnerFocus() override;

	void paintEvent(QPaintEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;

private:
	void sendCode();
	void sendCall();
	void checkPhoneAndHash();

	[[nodiscard]] int fragmentSkip() const;

	QString getPhone() const;
	void showError(const QString &error);

	// _hash from the link for account.sendConfirmPhoneCode call.
	// _phoneHash from auth.sentCode for account.confirmPhone call.
	const QString _phone;

	// If we receive the code length, we autosubmit _code field when enough symbols is typed.
	const int _sentCodeLength = 0;

	bool _isWaitingCheck = false;

	object_ptr<Ui::FlatLabel> _about = { nullptr };
	object_ptr<Ui::SentCodeField> _code = { nullptr };
	object_ptr<Ui::RoundButton> _fragment = { nullptr };

	QString _error;
	Ui::SentCodeCall _call;

	rpl::event_stream<QString> _checkRequests;
	rpl::event_stream<> _resendRequests;

};

} // namespace Ui
