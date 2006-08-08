/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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
#include <model/config/ConfigManagerFileStorage.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/config/CommandLineParser.h>
#include <control/CWengoPhone.h>

#ifdef GTKINTERFACE
	#include <presentation/gtk/GtkFactory.h>
#else
	#include <presentation/qt/QtFactory.h>
#endif

#include <imwrapper/IMWrapperFactory.h>
#include <sipwrapper/SipWrapperFactory.h>
#include <owsocket/OWClientSocket.h>
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

#include <util/Logger.h>

#include <cutil/global.h>

#if defined(CC_MSVC)
	#include <memorydump/MemoryDump.h>
#endif

#include <QtGui>

/**
 * Stub function to make GCC silent.
 *
 * @see http://www-eleves-isia.cma.fr/documentation/BoostDoc/boost_1_29_0/libs/test/doc/minimal.htm
 */
int test_main(int argc, char *argv[]) {
	return 1;
}

std::string getAddionnalInfo() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string info = "User: " + config.getProfileLastUsedName() + String::EOL;
	info += "buildid: " + String::fromUnsignedLongLong(WengoPhoneBuildId::BUILDID) + String::EOL;
	info += "revision: " + String::fromUnsignedLongLong(WengoPhoneBuildId::REVISION) + String::EOL;
	return info;
}

int main(int argc, char * argv[]) {

	//Todo before anything else: initializes the logger system
	LOG_DEBUG("WengoPhone started");

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//Loads the configuration: this is the first thing to do before anything else
	ConfigManagerFileStorage configManagerStorage(ConfigManager::getInstance());
	configManagerStorage.load(config.getConfigDir());
	////

	CommandLineParser cmdLineParser(argc, argv);

	LOG_DEBUG(String::EOL + getAddionnalInfo());

#ifdef CC_MSVC
	MemoryDump * memoryDump = new MemoryDump("WengoPhoneNG", String::fromUnsignedLongLong(WengoPhoneBuildId::REVISION).c_str());

	memoryDump->setLanguage(config.getLanguage().c_str());
	memoryDump->SetGetAdditionnalInfo(getAddionnalInfo);
#endif

	//Graphical interface implementation
	PFactory * pFactory = NULL;
#ifdef GTKINTERFACE
	pFactory = new GtkFactory(argc, argv);
#else
	pFactory = new QtFactory(argc, argv);
#endif
	PFactory::setFactory(pFactory);

	//No 2 qtwengophone at the same time
	if (Processes::isRunning("qtwengophone.exe")) {

		const std::string callFromCommandLine = config.getCmdLinePlaceCall();
		if (!callFromCommandLine.empty()) {
			//sends a command via socket to the existing instance.
			OWClientSocket sock;
			sock.connect("127.0.0.1", 25902);
			sock.write("1|o|call/" + callFromCommandLine);
#ifdef OS_WINDOWS
			Sleep(1000);
			WSACleanup();
#endif
		} else {
			QMessageBox::warning(NULL, QObject::tr("WengoPhone"), QObject::tr("WengoPhone is already running."), QMessageBox::Ok, 0);
		}
		return EXIT_SUCCESS;
	}

	//CWengoPhone creates PWengoPhone (QtWengoPhone, GtkWengoPhone...)
	//thus creating CWengoPhone at the very beginning makes the gui
	//to be shown before everything is fully loaded
	WengoPhone wengoPhone;
	CWengoPhone cWengoPhone(wengoPhone);
	pFactory->processEvents();

	//IM implementation
	IMWrapperFactory * imFactory = NULL;

	//SIP implementation
	SipWrapperFactory * sipFactory = NULL;

#ifdef SIPXWRAPPER
	sipFactory = new SipXFactory();
	imFactory = new NullIMFactory();
#elif defined(PHAPIWRAPPER)
	PhApiFactory * phApiFactory = new PhApiFactory();
	sipFactory = phApiFactory;
	imFactory = phApiFactory;
#elif defined(MULTIIMWRAPPER)
	PhApiFactory * phApiFactory = new PhApiFactory();
	GaimIMFactory * gaimIMFactory = new GaimIMFactory();
	sipFactory = phApiFactory;
	imFactory = new MultiIMFactory(*phApiFactory, *gaimIMFactory);
#else
	sipFactory = new NullSipFactory();
	imFactory = new NullIMFactory();
#endif
	SipWrapperFactory::setFactory(sipFactory);
	IMWrapperFactory::setFactory(imFactory);

	//Starts the model component thread
	cWengoPhone.start();

	pFactory->exec();

	cWengoPhone.terminate();

	LOG_DEBUG("WengoPhone ended");

	return EXIT_SUCCESS;
}
