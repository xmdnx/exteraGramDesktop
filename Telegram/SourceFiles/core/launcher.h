/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "core/base_integration.h"

namespace Core {

extern const char kOptionFractionalScalingEnabled[];
extern const char kOptionFreeType[];

class Launcher {
public:
	Launcher(int argc, char *argv[]);

	static std::unique_ptr<Launcher> Create(int argc, char *argv[]);

	static Launcher &Instance() {
		Expects(InstanceSetter::Instance != nullptr);

		return *InstanceSetter::Instance;
	}

	virtual int exec();

	const QStringList &arguments() const;
	bool customWorkingDir() const;

	uint64 installationTag() const;

	bool checkPortableVersionFolder();
	void workingFolderReady();
	void writeDebugModeSetting();
	void writeInstallBetaVersionsSetting();

	virtual ~Launcher();

protected:
	enum class UpdaterLaunch {
		PerformUpdate,
		JustRelaunch,
	};

private:
	void prepareSettings();
	void initQtMessageLogging();
	void processArguments();

	QStringList readArguments(int argc, char *argv[]) const;
	virtual std::optional<QStringList> readArgumentsHook(
			int argc,
			char *argv[]) const {
		return std::nullopt;
	}

	void init();
	virtual void initHook() {
	}
	virtual void initHighDpi();

	virtual bool launchUpdater(UpdaterLaunch action) = 0;

	int executeApplication();

	struct InstanceSetter {
		InstanceSetter(not_null<Launcher*> instance) {
			Expects(Instance == nullptr);

			Instance = instance;
		}

		static Launcher *Instance;
	};
	InstanceSetter _setter = { this };

	int _argc;
	char **_argv;
	QStringList _arguments;
	BaseIntegration _baseIntegration;

	bool _customWorkingDir = false;

};

} // namespace Core
