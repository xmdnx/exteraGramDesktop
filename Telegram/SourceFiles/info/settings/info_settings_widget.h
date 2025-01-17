/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "info/info_content_widget.h"
#include "info/info_controller.h"

namespace Settings {
class AbstractSection;
} // namespace Settings

namespace Info {
namespace Settings {

using Type = Section::SettingsType;

struct Tag;

struct SectionCustomTopBarData {
	rpl::producer<> backButtonEnables;
	rpl::producer<Info::Wrap> wrapValue;
};

class Memento final : public ContentMemento {
public:
	Memento(not_null<UserData*> self, Type type);

	object_ptr<ContentWidget> createWidget(
		QWidget *parent,
		not_null<Controller*> controller,
		const QRect &geometry) override;

	Section section() const override;

	Type type() const {
		return _type;
	}

	not_null<UserData*> self() const {
		return settingsSelf();
	}

	~Memento();

private:
	Type _type = Type();

};

class Widget final : public ContentWidget {
public:
	Widget(
		QWidget *parent,
		not_null<Controller*> controller);
	~Widget();

	not_null<UserData*> self() const;

	bool showInternal(
		not_null<ContentMemento*> memento) override;

	void setInternalState(
		const QRect &geometry,
		not_null<Memento*> memento);

	void saveChanges(FnMut<void()> done) override;

	void showFinished() override;
	void setInnerFocus() override;
	const Ui::RoundRect *bottomSkipRounding() const override;

	rpl::producer<bool> desiredShadowVisibility() const override;

	rpl::producer<QString> title() override;

	void enableBackButton() override;

private:
	void saveState(not_null<Memento*> memento);
	void restoreState(not_null<Memento*> memento);

	std::shared_ptr<ContentMemento> doCreateMemento() override;

	not_null<UserData*> _self;
	Type _type = Type();

	struct {
		rpl::event_stream<int> contentHeightValue;
		rpl::event_stream<int> fillerWidthValue;
		rpl::event_stream<> backButtonEnables;
	} _flexibleScroll;
	not_null<::Settings::AbstractSection*> _inner;
	QPointer<Ui::RpWidget> _pinnedToTop;
	QPointer<Ui::RpWidget> _pinnedToBottom;

	rpl::event_stream<std::vector<Type>> _removesFromStack;

};

} // namespace Settings
} // namespace Info
