/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <system/Processes.h>

#include <model/WengoPhone.h>
#include <model/config/CommandLineParser.h>
#include <model/config/Config.h>
#include <model/config/ConfigImporter.h>
#include <model/config/ConfigManagerFileStorage.h>
#include <model/config/ConfigManager.h>
#include <model/config/StartupSettingListener.h>
#include <model/classic/ClassicExterminator.h>
#include <model/commandserver/CommandClient.h>


#include <control/CWengoPhone.h>

#ifdef GTKINTERFACE
	#include <presentation/gtk/GtkFactory.h>
#else
	#include <presentation/qt/QtFactory.h>
	#include <presentation/qt/QtLanguage.h>
#endif

#include <imwrapper/IMWrapperFactory.h>
#include <sipwrapper/SipWrapperFactory.h>
#include <WengoPhoneBuildId.h>

#ifdef PHAPIWRAPPER
	#include <PhApiFactory.h>
#elif defined(SIPXWRAPPER)
	#include <SipXFactory.h>
	#include <NullIMFactory.h>
#elif defined (MULTIIMWRAPPER)
	#include <PhApiFactory.h>
	#include <multiim/MultiIMFactory.h>
	#include <GaimIMFactory.h>
#else
	#include <NullSipFactory.h>
	#include <NullIMFactory.h>
#endif

#include <util/File.h>
#include <util/Logger.h>
#include <util/Path.h>
#include <util/SafeDelete.h>
#include <util/WebBrowser.h>

#include <system/RegisterProtocol.h>

#include <qtutil/FSResourceFileEngineHandler.h>

#include <cutil/global.h>

#if defined(CC_MSVC)
	#include <memorydump/MSVCMemoryDump.h>
#elif defined(CC_MINGW)
	#include <winsock2.h>
#elif defined(OS_LINUX) && defined(ENABLE_CRASHREPORT)
	#include <memorydump/UNIXMemoryDump.h>
#elif defined(OS_MACOSX)
	#include <memorydump/MACMemoryDump.h>
#endif

#if defined(OS_MACOSX) || defined(OS_LINUX)
	#include <signal.h>
	#include <sys/wait.h>
#endif

#include <QtGui/QtGui>

#include <sstream>

#include <buildconfig.h>

static const char* CONFIG_SUBDIR = "/config/";


/**
 * Helper class to instantiate and delete factories based on the compilation
 * flags.
 */
struct FactoryHelper {
	FactoryHelper() {
	#ifdef SIPXWRAPPER
		_sipFactory = new SipXFactory();
		_imFactory = new NullIMFactory();

	#elif defined(PHAPIWRAPPER)
		_imFactory = _sipFactory = new PhApiFactory();

	#elif defined(MULTIIMWRAPPER)
		PhApiFactory* phApiFactory = new PhApiFactory();
		_sipFactory = phApiFactory;
		_gaimIMFactory = new GaimIMFactory();
		_imFactory = new MultiIMFactory(*phApiFactory, *_gaimIMFactory);

	#else
		_sipFactory = new NullSipFactory();
		_imFactory = new NullIMFactory();
	#endif

		SipWrapperFactory::setFactory(_sipFactory);
		IMWrapperFactory::setFactory(_imFactory);
	}

	~FactoryHelper() {
	#ifdef PHAPIWRAPPER
		// In this case _imFactory == _sipFactory, so don't delete it twice
		OWSAFE_DELETE(_imFactory);
	#else
		OWSAFE_DELETE(_sipFactory);
		OWSAFE_DELETE(_imFactory);
	#endif

	#ifdef MULTIIMWRAPPER
		OWSAFE_DELETE(_gaimIMFactory);
	#endif
	}

	IMWrapperFactory * _imFactory;
	SipWrapperFactory * _sipFactory;

#if defined(MULTIIMWRAPPER)
	GaimIMFactory* _gaimIMFactory;
#endif

};


/**
 * Stub function to make GCC silent.
 *
 * @see http://www-eleves-isia.cma.fr/documentation/BoostDoc/boost_1_29_0/libs/test/doc/minimal.htm
 */
int test_main(int argc, char *argv[]) {
	return 1;
}

static void initLogger(const std::string& userConfigDir) {
	std::string logFileName = userConfigDir + BINARY_NAME + ".log";
	// create directory if it doesn't exist
	if (!File::exists(userConfigDir)) {
		File::createPath(userConfigDir);
	}
	if (!File::isDirectory(userConfigDir)) {
		LOG_FATAL("User configuration dir '" + userConfigDir + "' does not exist or is not a directory");
	}

	Logger::getInstance()->setLogFileName(logFileName);
}

static void initConfig(const CommandLineParser& cmdLineParser) {
	ConfigManagerFileStorage configManagerStorage(ConfigManager::getInstance());

	std::string resourcesDir = cmdLineParser.getResourcesDir();
	if (!File::isDirectory(resourcesDir)) {
		LOG_FATAL("Resources dir '" + resourcesDir + "' does not exist or is not a directory");
	}

	std::string userConfigDir = cmdLineParser.getUserConfigDir();
	configManagerStorage.loadSystemConfig(resourcesDir + CONFIG_SUBDIR);
	Config::setConfigDir(userConfigDir);
	ConfigImporter importer;
	importer.importConfig();
	configManagerStorage.loadUserConfig(userConfigDir);

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.setResourcesDir(resourcesDir);
	WebBrowser::setBrowser(config.getLinuxPreferedBrowser());
}

static void registerHyperlinkProtocol(const Config& config, const std::string& executableName) {
	RegisterProtocol registerProtocol(config.getHyperlinkProtocol());
	std::string executableFullName = Path::getApplicationDirPath() + executableName;
	registerProtocol.bind(executableFullName + " -c %1", executableFullName + ",0", config.getCompanyWebSiteUrl());
}

/**
 * Callback additional info for MemoryDump.
 */
std::string getAdditionalInfo() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::stringstream stream;
	stream
		<< "User: " << config.getProfileLastUsedName() << std::endl
		<< "buildid: " << WengoPhoneBuildId::getBuildId() << std::endl
		<< "revision: " << WengoPhoneBuildId::getSvnRevision() << std::endl;
	return stream.str();
}

#if defined(OS_MACOSX) || defined(OS_LINUX)
static void sigpipe_catcher(int sig) {
#ifndef NDEBUG
	// Do not use LOG_DEBUG. There is only a limited set of functions you are
	// allowed to call from withing a signal catcher. See signal man page.
	static char msg[] = "Caught signal: SIGPIPE\n";
	write(2, msg, sizeof(msg));
#endif
}
#endif

#if defined(ENABLE_CRASHREPORT)
static void crash_catcher(int sig) {
	#if defined(OS_LINUX)
		UNIXMemoryDump memoryDump(BINARY_NAME, WengoPhoneBuildId::getSvnRevision());
	#elif defined(OS_MACOSX)
		//TODO
	#endif
}
#endif

int main(int argc, char * argv[]) {
	//Init presentation factories before parsing the command line so that Qt or
	//Gtk get a chance to parse their command line options ('-style' for Qt for
	//example)
	PFactory * pFactory = NULL;
#ifdef GTKINTERFACE
	pFactory = new GtkFactory(argc, argv);
#else
	pFactory = new QtFactory(argc, argv);
#endif
	PFactory::setFactory(pFactory);

	CommandLineParser cmdLineParser(BINARY_NAME, argc, argv);
	initLogger(cmdLineParser.getUserConfigDir());
	LOG_DEBUG("Started");

#if defined(OS_MACOSX) || defined(OS_LINUX)
	signal(SIGPIPE, sigpipe_catcher);
#endif

#if defined(ENABLE_CRASHREPORT)
	#if defined(OS_MACOSX) || defined(OS_LINUX)
		signal(SIGSEGV, crash_catcher);
		signal(SIGILL, crash_catcher);
		signal(SIGTRAP, crash_catcher);
		signal(SIGFPE, crash_catcher);
		signal(SIGBUS, crash_catcher);
		signal(SIGSYS, crash_catcher);
		signal(SIGXCPU, crash_catcher);
		signal(SIGXFSZ, crash_catcher);

		#if defined(OS_MACOSX)
			signal(SIGEMT, crash_catcher);
		#endif
	#endif
#endif

	// Setup factories. These must be initialized before loading config because
	// it's used when importing configuration from the Classic version.
	FactoryHelper factoryHelper;

	initConfig(cmdLineParser);

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//Remove WengoPhone Classic from startup registry
	ClassicExterminator::killClassicExecutable();

	//Remove WengoPhone Classic from startup registry
	ClassicExterminator::removeClassicFromStartup();

#ifdef CC_MSVC
	MSVCMemoryDump * memoryDump = new MSVCMemoryDump(BINARY_NAME, WengoPhoneBuildId::getSvnRevision());
	memoryDump->setGetAdditionalInfo(getAdditionalInfo);

#ifndef GTKINTERFACE
	//Sets translation file for MemoryDump/QtCrashReport
	memoryDump->setLanguage(QtLanguage::getWengoPhoneLocaleFileName().toStdString());
#endif

#endif

	// Uncomment to test crash report
	/*
	int* pointer;
	pointer = 0;
	*pointer = 12;
	*/

	//No 2 qtwengophone at the same time
	if (Processes::isRunning(BINARY_NAME) && !cmdLineParser.isSeveralWengoPhoneAllowed()) {
		const std::string callFromCommandLine = cmdLineParser.getCommand();
		CommandClient client;
		client.connect();
		if (!callFromCommandLine.empty()) {
			client.call(callFromCommandLine);
		} else {
			client.bringToFront();
		}
		return EXIT_SUCCESS;
	}

	// Register protocol used to associate the application in HTML links
	registerHyperlinkProtocol(config, cmdLineParser.getExecutableName());

	// Make the application starts when user logs on computer
	StartupSettingListener settingListener(cmdLineParser.getExecutableName());

	// Init file system resource engine
	FSResourceFileEngineHandler handler(QString::fromStdString(config.getResourcesDir()));

	// CWengoPhone creates PWengoPhone (QtWengoPhone, GtkWengoPhone...)
	// and then starts the model thread. This way the gui is shown as soon as
	// possible
	WengoPhone & wengoPhone = WengoPhone::getInstance();
	wengoPhone.setStartupCall(cmdLineParser.getCommand());
	CWengoPhone cWengoPhone(wengoPhone, cmdLineParser.getRunInBackground());
	pFactory->exec();

	WengoPhone::deleteInstance();
	LOG_DEBUG("Ended");

	return EXIT_SUCCESS;
}
