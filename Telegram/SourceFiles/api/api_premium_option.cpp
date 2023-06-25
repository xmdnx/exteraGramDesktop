/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#include "api/api_premium_option.h"

#include "ui/text/format_values.h"

namespace Api {

constexpr auto kDiscountDivider = 1.;

Data::SubscriptionOption CreateSubscriptionOption(
		int months,
		int monthlyAmount,
		int64 amount,
		const QString &currency,
		const QString &botUrl) {
	const auto discount = [&] {
		const auto percent = 1. - float64(amount) / (monthlyAmount * months);
		return std::round(percent * 100. / kDiscountDivider)
			* kDiscountDivider;
	}();
	return {
		.duration = Ui::FormatTTL(months * 86400 * 31),
		.discount = discount
			? QString::fromUtf8("\xe2\x88\x92%1%").arg(discount)
			: QString(),
		.costPerMonth = Ui::FillAmountAndCurrency(
			amount / float64(months),
			currency),
		.costTotal = Ui::FillAmountAndCurrency(amount, currency),
		.botUrl = botUrl,
	};
}

} // namespace Api
