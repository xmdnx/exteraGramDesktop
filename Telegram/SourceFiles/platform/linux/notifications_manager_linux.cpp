
/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#include "platform/linux/notifications_manager_linux.h"

#include "base/options.h"
#include "base/platform/base_platform_info.h"
#include "base/platform/linux/base_linux_glibmm_helper.h"
#include "base/platform/linux/base_linux_dbus_utilities.h"
#include "core/application.h"
#include "core/sandbox.h"
#include "core/core_settings.h"
#include "data/data_forum_topic.h"
#include "history/history.h"
#include "history/history_item.h"
#include "ui/empty_userpic.h"
#include "main/main_session.h"
#include "lang/lang_keys.h"
#include "base/weak_ptr.h"
#include "window/notifications_utilities.h"
#include "styles/style_window.h"

#include <QtCore/QBuffer>
#include <QtCore/QVersionNumber>
#include <QtGui/QGuiApplication>

#include <glibmm.h>
#include <giomm.h>

#include <dlfcn.h>

namespace Platform {
namespace Notifications {
namespace {

constexpr auto kService = "org.freedesktop.Notifications"_cs;
constexpr auto kObjectPath = "/org/freedesktop/Notifications"_cs;
constexpr auto kInterface = kService;
constexpr auto kPropertiesInterface = "org.freedesktop.DBus.Properties"_cs;

using namespace base::Platform;

struct ServerInformation {
	QString name;
	QString vendor;
	QVersionNumber version;
	QVersionNumber specVersion;
};

bool ServiceRegistered = false;
std::optional<ServerInformation> CurrentServerInformation;
QStringList CurrentCapabilities;

void Noexcept(Fn<void()> callback, Fn<void()> failed = nullptr) noexcept {
	try {
		callback();
		return;
	} catch (const std::exception &e) {
		LOG(("Native Notification Error: %1").arg(
			QString::fromStdString(e.what())));
	}

	if (failed) {
		failed();
	}
}

std::unique_ptr<base::Platform::DBus::ServiceWatcher> CreateServiceWatcher() {
	try {
		const auto connection = Gio::DBus::Connection::get_sync(
			Gio::DBus::BusType::SESSION);

		const auto activatable = [&] {
			try {
				return ranges::contains(
					base::Platform::DBus::ListActivatableNames(connection),
					std::string(kService),
					&Glib::ustring::raw);
			} catch (...) {
				// avoid service restart loop in sandboxed environments
				return true;
			}
		}();

		return std::make_unique<base::Platform::DBus::ServiceWatcher>(
			connection,
			std::string(kService),
			[=](
				const Glib::ustring &service,
				const Glib::ustring &oldOwner,
				const Glib::ustring &newOwner) {
				if (activatable && newOwner.empty()) {
					crl::on_main([] {
						Core::App().notifications().clearAll();
					});
					return;
				}

				crl::on_main([] {
					Core::App().notifications().createManager();
				});
			});
	} catch (...) {
	}

	return nullptr;
}

void StartServiceAsync(Fn<void()> callback) {
	try {
		const auto connection = Gio::DBus::Connection::get_sync(
			Gio::DBus::BusType::SESSION);

		DBus::StartServiceByNameAsync(
			connection,
			std::string(kService),
			[=](Fn<DBus::StartReply()> result) {
				Noexcept([&] {
					try {
						result(); // get the error if any
					} catch (const Glib::Error &e) {
						static const auto NotSupportedErrors = {
							"org.freedesktop.DBus.Error.ServiceUnknown",
						};

						const auto errorName =
							Gio::DBus::ErrorUtils::get_remote_error(e).raw();

						if (!ranges::contains(
								NotSupportedErrors,
								errorName)) {
							throw e;
						}
					}
				});

				crl::on_main(callback);
			});

			return;
	} catch (...) {
	}

	crl::on_main(callback);
}

bool GetServiceRegistered() {
	try {
		const auto connection = Gio::DBus::Connection::get_sync(
			Gio::DBus::BusType::SESSION);

		const auto hasOwner = [&] {
			try {
				return DBus::NameHasOwner(
					connection,
					std::string(kService));
			} catch (...) {
				return false;
			}
		}();

		static const auto activatable = [&] {
			try {
				return ranges::contains(
					DBus::ListActivatableNames(connection),
					std::string(kService),
					&Glib::ustring::raw);
			} catch (...) {
				return false;
			}
		}();

		return hasOwner || activatable;
	} catch (...) {
	}

	return false;
}

void GetServerInformation(
		Fn<void(const std::optional<ServerInformation> &)> callback) {
	Noexcept([&] {
		const auto connection = Gio::DBus::Connection::get_sync(
			Gio::DBus::BusType::SESSION);

		connection->call(
			std::string(kObjectPath),
			std::string(kInterface),
			"GetServerInformation",
			{},
			[=](const Glib::RefPtr<Gio::AsyncResult> &result) {
				Noexcept([&] {
					auto reply = connection->call_finish(result);

					const auto name = GlibVariantCast<Glib::ustring>(
						reply.get_child(0));

					const auto vendor = GlibVariantCast<Glib::ustring>(
						reply.get_child(1));

					const auto version = GlibVariantCast<Glib::ustring>(
						reply.get_child(2));

					const auto specVersion = GlibVariantCast<Glib::ustring>(
						reply.get_child(3));

					crl::on_main([=] {
						callback(ServerInformation{
							QString::fromStdString(name),
							QString::fromStdString(vendor),
							QVersionNumber::fromString(
								QString::fromStdString(version)),
							QVersionNumber::fromString(
								QString::fromStdString(specVersion)),
						});
					});
				}, [&] {
					crl::on_main([=] { callback(std::nullopt); });
				});
			},
			std::string(kService));
	}, [&] {
		crl::on_main([=] { callback(std::nullopt); });
	});
}

void GetCapabilities(Fn<void(const QStringList &)> callback) {
	Noexcept([&] {
		const auto connection = Gio::DBus::Connection::get_sync(
			Gio::DBus::BusType::SESSION);

		connection->call(
			std::string(kObjectPath),
			std::string(kInterface),
			"GetCapabilities",
			{},
			[=](const Glib::RefPtr<Gio::AsyncResult> &result) {
				Noexcept([&] {
					auto reply = connection->call_finish(result);

					QStringList value;
					ranges::transform(
						GlibVariantCast<std::vector<Glib::ustring>>(
							reply.get_child(0)),
						ranges::back_inserter(value),
						QString::fromStdString);

					crl::on_main([=] {
						callback(value);
					});
				}, [&] {
					crl::on_main([=] { callback({}); });
				});
			},
			std::string(kService));
	}, [&] {
		crl::on_main([=] { callback({}); });
	});
}

void GetInhibited(Fn<void(bool)> callback) {
	Noexcept([&] {
		const auto connection = Gio::DBus::Connection::get_sync(
			Gio::DBus::BusType::SESSION);

		connection->call(
			std::string(kObjectPath),
			std::string(kPropertiesInterface),
			"Get",
			MakeGlibVariant(std::tuple{
				Glib::ustring(std::string(kInterface)),
				Glib::ustring("Inhibited"),
			}),
			[=](const Glib::RefPtr<Gio::AsyncResult> &result) {
				Noexcept([&] {
					auto reply = connection->call_finish(result);

					const auto value = GlibVariantCast<bool>(
						GlibVariantCast<Glib::VariantBase>(
							reply.get_child(0)));

					crl::on_main([=] {
						callback(value);
					});
				}, [&] {
					crl::on_main([=] { callback(false); });
				});
			},
			std::string(kService));
	}, [&] {
		crl::on_main([=] { callback(false); });
	});
}

ServerInformation CurrentServerInformationValue() {
	return CurrentServerInformation.value_or(ServerInformation{});
}

Glib::ustring GetImageKey(const QVersionNumber &specificationVersion) {
	const auto normalizedVersion = specificationVersion.normalized();

	if (normalizedVersion.isNull()) {
		LOG(("Native Notification Error: specification version is null"));
		return {};
	}

	if (normalizedVersion >= QVersionNumber(1, 2)) {
		return "image-data";
	} else if (normalizedVersion == QVersionNumber(1, 1)) {
		return "image_data";
	}

	return "icon_data";
}

bool UseGNotification() {
	if (!Gio::Application::get_default()) {
		return false;
	}

	if (Window::Notifications::OptionGNotification.value()) {
		return true;
	}

	return KSandbox::isFlatpak() && !ServiceRegistered;
}

class NotificationData final : public base::has_weak_ptr {
public:
	using NotificationId = Window::Notifications::Manager::NotificationId;

	NotificationData(
		not_null<Manager*> manager,
		NotificationId id);

	[[nodiscard]] bool init(
		const QString &title,
		const QString &subtitle,
		const QString &msg,
		Window::Notifications::Manager::DisplayOptions options);

	NotificationData(const NotificationData &other) = delete;
	NotificationData &operator=(const NotificationData &other) = delete;
	NotificationData(NotificationData &&other) = delete;
	NotificationData &operator=(NotificationData &&other) = delete;

	~NotificationData();

	void show();
	void close();
	void setImage(const QImage &image);

private:
	const not_null<Manager*> _manager;
	NotificationId _id;

	Glib::RefPtr<Gio::Application> _application;
	Glib::RefPtr<Gio::Notification> _notification;

	Glib::RefPtr<Gio::DBus::Connection> _dbusConnection;
	Glib::ustring _title;
	Glib::ustring _body;
	std::vector<Glib::ustring> _actions;
	std::map<Glib::ustring, Glib::VariantBase> _hints;
	Glib::ustring _imageKey;

	uint _notificationId = 0;
	uint _actionInvokedSignalId = 0;
	uint _activationTokenSignalId = 0;
	uint _notificationRepliedSignalId = 0;
	uint _notificationClosedSignalId = 0;

	void notificationClosed(uint id, uint reason);
	void actionInvoked(uint id, const Glib::ustring &actionName);
	void activationToken(uint id, const Glib::ustring &token);
	void notificationReplied(uint id, const Glib::ustring &text);

};

using Notification = std::unique_ptr<NotificationData>;

NotificationData::NotificationData(
	not_null<Manager*> manager,
	NotificationId id)
: _manager(manager)
, _id(id)
, _application(UseGNotification()
		? Gio::Application::get_default()
		: nullptr) {
}

bool NotificationData::init(
		const QString &title,
		const QString &subtitle,
		const QString &msg,
		Window::Notifications::Manager::DisplayOptions options) {
	if (_application) {
		_notification = Gio::Notification::create(
			subtitle.isEmpty()
				? title.toStdString()
				: subtitle.toStdString() + " (" + title.toStdString() + ')');

		_notification->set_body(msg.toStdString());

		_notification->set_icon(
			Gio::ThemedIcon::create(base::IconName().toStdString()));

		// glib 2.42+, we keep glib 2.40+ compatibility
		static const auto set_priority = [] {
			// reset dlerror after dlsym call
			const auto guard = gsl::finally([] { dlerror(); });
			return reinterpret_cast<decltype(&g_notification_set_priority)>(
				dlsym(RTLD_DEFAULT, "g_notification_set_priority"));
		}();

		if (set_priority) {
			// for chat messages, according to
			// https://docs.gtk.org/gio/enum.NotificationPriority.html
			set_priority(_notification->gobj(), G_NOTIFICATION_PRIORITY_HIGH);
		}

		// glib 2.70+, we keep glib 2.40+ compatibility
		static const auto set_category = [] {
			// reset dlerror after dlsym call
			const auto guard = gsl::finally([] { dlerror(); });
			return reinterpret_cast<decltype(&g_notification_set_category)>(
				dlsym(RTLD_DEFAULT, "g_notification_set_category"));
		}();

		if (set_category) {
			set_category(_notification->gobj(), "im.received");
		}

		const auto idTuple = _id.toTuple();

		_notification->set_default_action(
			"app.notification-activate",
			idTuple);

		if (!options.hideMarkAsRead) {
			_notification->add_button(
				tr::lng_context_mark_read(tr::now).toStdString(),
				"app.notification-mark-as-read",
				idTuple);
		}

		return true;
	}

	Noexcept([&] {
		_dbusConnection = Gio::DBus::Connection::get_sync(
			Gio::DBus::BusType::SESSION);
	});

	if (!_dbusConnection) {
		return false;
	}

	const auto weak = base::make_weak(this);
	const auto capabilities = CurrentCapabilities;

	const auto signalEmitted = [=](
			const Glib::RefPtr<Gio::DBus::Connection> &connection,
			const Glib::ustring &sender_name,
			const Glib::ustring &object_path,
			const Glib::ustring &interface_name,
			const Glib::ustring &signal_name,
			Glib::VariantContainerBase parameters) {
		Noexcept([&] {
			if (signal_name == "ActionInvoked") {
				const auto id = GlibVariantCast<uint>(
					parameters.get_child(0));

				const auto actionName = GlibVariantCast<Glib::ustring>(
					parameters.get_child(1));

				crl::on_main(weak, [=] { actionInvoked(id, actionName); });
			} else if (signal_name == "ActivationToken") {
				const auto id = GlibVariantCast<uint>(
					parameters.get_child(0));

				const auto token = GlibVariantCast<Glib::ustring>(
					parameters.get_child(1));

				crl::on_main(weak, [=] { activationToken(id, token); });
			} else if (signal_name == "NotificationReplied") {
				const auto id = GlibVariantCast<uint>(
					parameters.get_child(0));

				const auto text = GlibVariantCast<Glib::ustring>(
					parameters.get_child(1));

				crl::on_main(weak, [=] { notificationReplied(id, text); });
			} else if (signal_name == "NotificationClosed") {
				const auto id = GlibVariantCast<uint>(
					parameters.get_child(0));

				const auto reason = GlibVariantCast<uint>(
					parameters.get_child(1));

				crl::on_main(weak, [=] { notificationClosed(id, reason); });
			}
		});
	};

	_imageKey = GetImageKey(CurrentServerInformationValue().specVersion);

	if (capabilities.contains(u"body-markup"_q)) {
		_title = title.toStdString();

		_body = subtitle.isEmpty()
			? msg.toHtmlEscaped().toStdString()
			: u"<b>%1</b>\n%2"_q.arg(
				subtitle.toHtmlEscaped(),
				msg.toHtmlEscaped()).toStdString();
	} else {
		_title = subtitle.isEmpty()
			? title.toStdString()
			: subtitle.toStdString() + " (" + title.toStdString() + ')';

		_body = msg.toStdString();
	}

	if (capabilities.contains("actions")) {
		_actions.push_back("default");
		_actions.push_back(tr::lng_open_link(tr::now).toStdString());

		if (!options.hideMarkAsRead) {
			// icon name according to https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
			_actions.push_back("mail-mark-read");
			_actions.push_back(
				tr::lng_context_mark_read(tr::now).toStdString());
		}

		if (capabilities.contains("inline-reply")
			&& !options.hideReplyButton) {
			_actions.push_back("inline-reply");
			_actions.push_back(
				tr::lng_notification_reply(tr::now).toStdString());

			_notificationRepliedSignalId =
				_dbusConnection->signal_subscribe(
					signalEmitted,
					std::string(kService),
					std::string(kInterface),
					"NotificationReplied",
					std::string(kObjectPath));
		}

		_actionInvokedSignalId = _dbusConnection->signal_subscribe(
			signalEmitted,
			std::string(kService),
			std::string(kInterface),
			"ActionInvoked",
			std::string(kObjectPath));

		_activationTokenSignalId = _dbusConnection->signal_subscribe(
			signalEmitted,
			std::string(kService),
			std::string(kInterface),
			"ActivationToken",
			std::string(kObjectPath));
	}

	if (capabilities.contains("action-icons")) {
		_hints["action-icons"] = Glib::Variant<bool>::create(true);
	}

	// suppress system sound if telegram sound activated,
	// otherwise use system sound
	if (capabilities.contains("sound")) {
		if (Core::App().settings().soundNotify()) {
			_hints["suppress-sound"] = Glib::Variant<bool>::create(true);
		} else {
			// sound name according to http://0pointer.de/public/sound-naming-spec.html
			_hints["sound-name"] = Glib::Variant<Glib::ustring>::create(
				"message-new-instant");
		}
	}

	if (capabilities.contains("x-canonical-append")) {
		_hints["x-canonical-append"] = Glib::Variant<Glib::ustring>::create(
			"true");
	}

	_hints["category"] = Glib::Variant<Glib::ustring>::create("im.received");

	_hints["desktop-entry"] = Glib::Variant<Glib::ustring>::create(
		QGuiApplication::desktopFileName().chopped(8).toStdString());

	_notificationClosedSignalId = _dbusConnection->signal_subscribe(
		signalEmitted,
		std::string(kService),
		std::string(kInterface),
		"NotificationClosed",
		std::string(kObjectPath));
	return true;
}

NotificationData::~NotificationData() {
	if (_dbusConnection) {
		if (_actionInvokedSignalId != 0) {
			_dbusConnection->signal_unsubscribe(_actionInvokedSignalId);
		}

		if (_activationTokenSignalId != 0) {
			_dbusConnection->signal_unsubscribe(_activationTokenSignalId);
		}

		if (_notificationRepliedSignalId != 0) {
			_dbusConnection->signal_unsubscribe(_notificationRepliedSignalId);
		}

		if (_notificationClosedSignalId != 0) {
			_dbusConnection->signal_unsubscribe(_notificationClosedSignalId);
		}
	}
}

void NotificationData::show() {
	if (_application && _notification) {
		_application->send_notification(
			std::to_string(_id.contextId.sessionId)
				+ '-'
				+ std::to_string(_id.contextId.peerId.value)
				+ '-'
				+ std::to_string(_id.msgId.bare),
			_notification);
		return;
	}

	// a hack for snap's activation restriction
	const auto weak = base::make_weak(this);
	StartServiceAsync(crl::guard(weak, [=] {
		const auto iconName = _imageKey.empty()
			|| _hints.find(_imageKey) == end(_hints)
				? Glib::ustring(base::IconName().toStdString())
				: Glib::ustring();
		const auto connection = _dbusConnection;

		connection->call(
			std::string(kObjectPath),
			std::string(kInterface),
			"Notify",
			MakeGlibVariant(std::tuple{
				Glib::ustring(std::string(AppName)),
				uint(0),
				iconName,
				_title,
				_body,
				_actions,
				_hints,
				-1,
			}),
			[=](const Glib::RefPtr<Gio::AsyncResult> &result) {
				Noexcept([&] {
					auto reply = connection->call_finish(result);
					const auto notificationId = GlibVariantCast<uint>(
						reply.get_child(0));
					crl::on_main(weak, [=] {
						_notificationId = notificationId;
					});
				}, [&] {
					crl::on_main(weak, [=] {
						_manager->clearNotification(_id);
					});
				});
			},
			std::string(kService));
	}));
}

void NotificationData::close() {
	if (_application) {
		_application->withdraw_notification(
			std::to_string(_id.contextId.sessionId)
				+ '-'
				+ std::to_string(_id.contextId.peerId.value)
				+ '-'
				+ std::to_string(_id.msgId.bare));
		_manager->clearNotification(_id);
		return;
	}

	_dbusConnection->call(
		std::string(kObjectPath),
		std::string(kInterface),
		"CloseNotification",
		MakeGlibVariant(std::tuple{
			_notificationId,
		}),
		{},
		std::string(kService),
		-1,
		Gio::DBus::CallFlags::NO_AUTO_START);
	_manager->clearNotification(_id);
}

void NotificationData::setImage(const QImage &image) {
	if (_notification) {
		const auto imageData = [&] {
			QByteArray ba;
			QBuffer buffer(&ba);
			buffer.open(QIODevice::WriteOnly);
			image.save(&buffer, "PNG");
			return ba;
		}();

		_notification->set_icon(
			Gio::BytesIcon::create(
				Glib::Bytes::create(
					imageData.constData(),
					imageData.size())));

		return;
	}

	if (_imageKey.empty()) {
		return;
	}

	const auto convertedImage = image.hasAlphaChannel()
		? image.convertToFormat(QImage::Format_RGBA8888)
		: image.convertToFormat(QImage::Format_RGB888);

	_hints[_imageKey] = MakeGlibVariant(std::tuple{
		convertedImage.width(),
		convertedImage.height(),
		int(convertedImage.bytesPerLine()),
		convertedImage.hasAlphaChannel(),
		8,
		convertedImage.hasAlphaChannel() ? 4 : 3,
		std::vector<uchar>(
			convertedImage.constBits(),
			convertedImage.constBits() + convertedImage.sizeInBytes()),
	});
}

void NotificationData::notificationClosed(uint id, uint reason) {
	/*
	 * From: https://specifications.freedesktop.org/notification-spec/latest/ar01s09.html
	 * The reason the notification was closed
	 * 1 - The notification expired.
	 * 2 - The notification was dismissed by the user.
	 * 3 - The notification was closed by a call to CloseNotification.
	 * 4 - Undefined/reserved reasons.
	 *
	 * If the notification was dismissed by the user (reason == 2), the notification is not kept in notification history.
	 * We do not need to send a "CloseNotification" call later to clear it from history.
	 * Therefore we can drop the notification reference now.
	 * In all other cases we keep the notification reference so that we may clear the notification later from history,
	 * if the message for that notification is read (e.g. chat is opened or read from another device).
	*/
	if (id == _notificationId && reason == 2) {
		_manager->clearNotification(_id);
	}
}

void NotificationData::actionInvoked(
		uint id,
		const Glib::ustring &actionName) {
	if (id != _notificationId) {
		return;
	}

	if (actionName == "default") {
		_manager->notificationActivated(_id);
	} else if (actionName == "mail-mark-read") {
		_manager->notificationReplied(_id, {});
	}
}

void NotificationData::activationToken(uint id, const Glib::ustring &token) {
	if (id == _notificationId) {
		qputenv("XDG_ACTIVATION_TOKEN", QByteArray::fromStdString(token));
	}
}

void NotificationData::notificationReplied(
		uint id,
		const Glib::ustring &text) {
	if (id == _notificationId) {
		_manager->notificationReplied(
			_id,
			{ QString::fromStdString(text), {} });
	}
}

} // namespace

bool SkipToastForCustom() {
	return false;
}

void MaybePlaySoundForCustom(Fn<void()> playSound) {
	playSound();
}

void MaybeFlashBounceForCustom(Fn<void()> flashBounce) {
	flashBounce();
}

bool WaitForInputForCustom() {
	return true;
}

bool Supported() {
	return ServiceRegistered || UseGNotification();
}

bool Enforced() {
	// Wayland doesn't support positioning
	// and custom notifications don't work here
	return IsWayland()
		|| (Gio::Application::get_default()
			&& Window::Notifications::OptionGNotification.value());
}

bool ByDefault() {
	// The capabilities are static, equivalent to 'body' and 'actions' only
	if (UseGNotification()) {
		return false;
	}

	// A list of capabilities that offer feature parity
	// with custom notifications
	static const auto NeededCapabilities = {
		// To show message content
		u"body"_q,
		// To have buttons on notifications
		u"actions"_q,
		// To have quick reply
		u"inline-reply"_q,
		// To not to play sound with Don't Disturb activated
		// (no, using sound capability is not a way)
		u"inhibitions"_q,
	};

	return ranges::all_of(NeededCapabilities, [&](const auto &capability) {
		return CurrentCapabilities.contains(capability);
	});
}

void Create(Window::Notifications::System *system) {
	static const auto ServiceWatcher = CreateServiceWatcher();

	const auto managerSetter = [=] {
		using ManagerType = Window::Notifications::ManagerType;
		if ((Core::App().settings().nativeNotifications() || Enforced())
			&& Supported()) {
			if (system->manager().type() != ManagerType::Native) {
				system->setManager(std::make_unique<Manager>(system));
			}
		} else if (Enforced()) {
			if (system->manager().type() != ManagerType::Dummy) {
				using DummyManager = Window::Notifications::DummyManager;
				system->setManager(std::make_unique<DummyManager>(system));
			}
		} else if (system->manager().type() != ManagerType::Default) {
			system->setManager(nullptr);
		}
	};

	const auto counter = std::make_shared<int>(2);
	const auto oneReady = [=] {
		if (!--*counter) {
			managerSetter();
		}
	};

	// snap doesn't allow access when the daemon is not running :(
	StartServiceAsync([=] {
		ServiceRegistered = GetServiceRegistered();

		if (!ServiceRegistered) {
			CurrentServerInformation = std::nullopt;
			CurrentCapabilities = QStringList{};
			managerSetter();
			return;
		}

		GetServerInformation([=](
				const std::optional<ServerInformation> &result) {
			CurrentServerInformation = result;
			oneReady();
		});

		GetCapabilities([=](const QStringList &result) {
			CurrentCapabilities = result;
			oneReady();
		});
	});
}

class Manager::Private : public base::has_weak_ptr {
public:
	explicit Private(not_null<Manager*> manager);

	void showNotification(
		not_null<PeerData*> peer,
		MsgId topicRootId,
		Ui::PeerUserpicView &userpicView,
		MsgId msgId,
		const QString &title,
		const QString &subtitle,
		const QString &msg,
		DisplayOptions options);
	void clearAll();
	void clearFromItem(not_null<HistoryItem*> item);
	void clearFromTopic(not_null<Data::ForumTopic*> topic);
	void clearFromHistory(not_null<History*> history);
	void clearFromSession(not_null<Main::Session*> session);
	void clearNotification(NotificationId id);
	void invokeIfNotInhibited(Fn<void()> callback);

	~Private();

private:
	const not_null<Manager*> _manager;

	base::flat_map<
		ContextId,
		base::flat_map<MsgId, Notification>> _notifications;

	Glib::RefPtr<Gio::DBus::Connection> _dbusConnection;
	bool _inhibited = false;
	uint _inhibitedSignalId = 0;

};

Manager::Private::Private(not_null<Manager*> manager)
: _manager(manager) {
	const auto serverInformation = CurrentServerInformation;
	const auto capabilities = CurrentCapabilities;

	if (serverInformation.has_value()) {
		LOG(("Notification daemon product name: %1")
			.arg(serverInformation->name));

		LOG(("Notification daemon vendor name: %1")
			.arg(serverInformation->vendor));

		LOG(("Notification daemon version: %1")
			.arg(serverInformation->version.toString()));

		LOG(("Notification daemon specification version: %1")
			.arg(serverInformation->specVersion.toString()));
	}

	if (!capabilities.isEmpty()) {
		LOG(("Notification daemon capabilities: %1")
			.arg(capabilities.join(", ")));
	}

	if (capabilities.contains(u"inhibitions"_q)) {
		Noexcept([&] {
			_dbusConnection = Gio::DBus::Connection::get_sync(
				Gio::DBus::BusType::SESSION);
		});

		if (!_dbusConnection) {
			return;
		}

		const auto weak = base::make_weak(this);
		GetInhibited(crl::guard(weak, [=](bool result) {
			_inhibited = result;
		}));

		_inhibitedSignalId = _dbusConnection->signal_subscribe(
			[=](
					const Glib::RefPtr<Gio::DBus::Connection> &connection,
					const Glib::ustring &sender_name,
					const Glib::ustring &object_path,
					const Glib::ustring &interface_name,
					const Glib::ustring &signal_name,
					Glib::VariantContainerBase parameters) {
				Noexcept([&] {
					const auto interface = GlibVariantCast<Glib::ustring>(
						parameters.get_child(0));

					if (interface != kInterface.data()) {
						return;
					}

					const auto inhibited = GlibVariantCast<bool>(
						GlibVariantCast<
							std::map<Glib::ustring, Glib::VariantBase>
					>(parameters.get_child(1)).at("Inhibited"));

					crl::on_main(weak, [=] {
						_inhibited = inhibited;
					});
				});
			},
			std::string(kService),
			std::string(kPropertiesInterface),
			"PropertiesChanged",
			std::string(kObjectPath));
	}
}

void Manager::Private::showNotification(
		not_null<PeerData*> peer,
		MsgId topicRootId,
		Ui::PeerUserpicView &userpicView,
		MsgId msgId,
		const QString &title,
		const QString &subtitle,
		const QString &msg,
		DisplayOptions options) {
	const auto key = ContextId{
		.sessionId = peer->session().uniqueId(),
		.peerId = peer->id,
		.topicRootId = topicRootId,
	};
	const auto notificationId = NotificationId{
		.contextId = key,
		.msgId = msgId,
	};
	auto notification = std::make_unique<NotificationData>(
		_manager,
		notificationId);
	const auto inited = notification->init(
		title,
		subtitle,
		msg,
		options);
	if (!inited) {
		return;
	}

	if (!options.hideNameAndPhoto) {
		notification->setImage(
			Window::Notifications::GenerateUserpic(peer, userpicView));
	}

	auto i = _notifications.find(key);
	if (i != end(_notifications)) {
		auto j = i->second.find(msgId);
		if (j != end(i->second)) {
			auto oldNotification = std::move(j->second);
			i->second.erase(j);
			oldNotification->close();
			i = _notifications.find(key);
		}
	}
	if (i == end(_notifications)) {
		i = _notifications.emplace(
			key,
			base::flat_map<MsgId, Notification>()).first;
	}
	const auto j = i->second.emplace(
		msgId,
		std::move(notification)).first;
	j->second->show();
}

void Manager::Private::clearAll() {
	for (const auto &[key, notifications] : base::take(_notifications)) {
		for (const auto &[msgId, notification] : notifications) {
			notification->close();
		}
	}
}

void Manager::Private::clearFromItem(not_null<HistoryItem*> item) {
	const auto key = ContextId{
		.sessionId = item->history()->session().uniqueId(),
		.peerId = item->history()->peer->id,
		.topicRootId = item->topicRootId(),
	};
	const auto i = _notifications.find(key);
	if (i == _notifications.cend()) {
		return;
	}
	const auto j = i->second.find(item->id);
	if (j == i->second.end()) {
		return;
	}
	const auto taken = base::take(j->second);
	i->second.erase(j);
	if (i->second.empty()) {
		_notifications.erase(i);
	}
	taken->close();
}

void Manager::Private::clearFromTopic(not_null<Data::ForumTopic*> topic) {
	const auto key = ContextId{
		.sessionId = topic->session().uniqueId(),
		.peerId = topic->history()->peer->id
	};
	const auto i = _notifications.find(key);
	if (i != _notifications.cend()) {
		const auto temp = base::take(i->second);
		_notifications.erase(i);

		for (const auto &[msgId, notification] : temp) {
			notification->close();
		}
	}
}

void Manager::Private::clearFromHistory(not_null<History*> history) {
	const auto sessionId = history->session().uniqueId();
	const auto peerId = history->peer->id;
	auto i = _notifications.lower_bound(ContextId{
		.sessionId = sessionId,
		.peerId = peerId,
	});
	while (i != _notifications.cend()
		&& i->first.sessionId == sessionId
		&& i->first.peerId == peerId) {
		const auto temp = base::take(i->second);
		i = _notifications.erase(i);

		for (const auto &[msgId, notification] : temp) {
			notification->close();
		}
	}
}

void Manager::Private::clearFromSession(not_null<Main::Session*> session) {
	const auto sessionId = session->uniqueId();
	auto i = _notifications.lower_bound(ContextId{
		.sessionId = sessionId,
	});
	while (i != _notifications.cend() && i->first.sessionId == sessionId) {
		const auto temp = base::take(i->second);
		i = _notifications.erase(i);

		for (const auto &[msgId, notification] : temp) {
			notification->close();
		}
	}
}

void Manager::Private::clearNotification(NotificationId id) {
	auto i = _notifications.find(id.contextId);
	if (i != _notifications.cend()) {
		if (i->second.remove(id.msgId) && i->second.empty()) {
			_notifications.erase(i);
		}
	}
}

void Manager::Private::invokeIfNotInhibited(Fn<void()> callback) {
	if (!_inhibited) {
		callback();
	}
}

Manager::Private::~Private() {
	clearAll();

	if (_dbusConnection) {
		if (_inhibitedSignalId != 0) {
			_dbusConnection->signal_unsubscribe(_inhibitedSignalId);
		}
	}
}

Manager::Manager(not_null<Window::Notifications::System*> system)
: NativeManager(system)
, _private(std::make_unique<Private>(this)) {
}

void Manager::clearNotification(NotificationId id) {
	_private->clearNotification(id);
}

Manager::~Manager() = default;

void Manager::doShowNativeNotification(
		not_null<PeerData*> peer,
		MsgId topicRootId,
		Ui::PeerUserpicView &userpicView,
		MsgId msgId,
		const QString &title,
		const QString &subtitle,
		const QString &msg,
		DisplayOptions options) {
	_private->showNotification(
		peer,
		topicRootId,
		userpicView,
		msgId,
		title,
		subtitle,
		msg,
		options);
}

void Manager::doClearAllFast() {
	_private->clearAll();
}

void Manager::doClearFromItem(not_null<HistoryItem*> item) {
	_private->clearFromItem(item);
}

void Manager::doClearFromTopic(not_null<Data::ForumTopic*> topic) {
	_private->clearFromTopic(topic);
}

void Manager::doClearFromHistory(not_null<History*> history) {
	_private->clearFromHistory(history);
}

void Manager::doClearFromSession(not_null<Main::Session*> session) {
	_private->clearFromSession(session);
}

bool Manager::doSkipToast() const {
	return false;
}

void Manager::doMaybePlaySound(Fn<void()> playSound) {
	_private->invokeIfNotInhibited(std::move(playSound));
}

void Manager::doMaybeFlashBounce(Fn<void()> flashBounce) {
	_private->invokeIfNotInhibited(std::move(flashBounce));
}

} // namespace Notifications
} // namespace Platform
