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
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <control/CWengoPhone.h>

#ifdef GTKINTERFACE
	#include <presentation/gtk/GtkFactory.h>
#else
	#include <presentation/qt/QtFactory.h>
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

#include <cutil/global.h>

#if defined(CC_MSVC)
	#include <memorydump/MemoryDump.h>
#endif

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

/**
 * Stub function to make GCC silent.
 *
 * @see http://www-eleves-isia.cma.fr/documentation/BoostDoc/boost_1_29_0/libs/test/doc/minimal.htm
 */
int test_main(int argc, char *argv[]) {
	return 1;
}

std::string setAddionnalInfo() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string info = "User: " + config.getProfileLastUsedName() + "\n";
	info += "buildid: " + String::fromUnsignedLongLong(WengoPhoneBuildId::BUILDID) + "\n";
	info += "revision: " + String::fromUnsignedLongLong(WengoPhoneBuildId::REVISION) + "\n";
	return info;
}

int main(int argc, char * argv[]) {

#if defined(CC_MSVC)
	MemoryDump * memoryDump = new MemoryDump("WengoPhoneNG");
	
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	memoryDump->setLanguage(config.getLanguage().c_str());
	memoryDump->SetGetAdditionnalInfo(setAddionnalInfo);
#endif
	
	//Graphical interface implementation
	PFactory * pFactory = NULL;
#ifdef GTKINTERFACE
	pFactory = new GtkFactory(argc, argv);
#else
	pFactory = new QtFactory(argc, argv);
#endif
	PFactory::setFactory(pFactory);

    if (Processes::isRunning("qtwengophone.exe")) {
        QMessageBox::warning(0, QObject::tr("WengoPhone"), QObject::tr("WengoPhone is already running."), QMessageBox::Ok, 0);
        exit(0);
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

	return pFactory->exec();
}
