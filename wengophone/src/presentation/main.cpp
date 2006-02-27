/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include <model/WengoPhone.h>
#include <control/CWengoPhone.h>

#ifdef GTKINTERFACE
	#include <presentation/gtk/GtkFactory.h>
#else
	#include <presentation/qt/QtFactory.h>
#endif

#include <imwrapper/IMWrapperFactory.h>
#include <sipwrapper/SipWrapperFactory.h>

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

#include <global.h>

#if defined(CC_MSVC)
	#include <win32/MemoryDump.h>
#endif

#include <string>
#include <iostream>
using namespace std;

/**
 * Stub function to make GCC silent.
 *
 * @see http://www-eleves-isia.cma.fr/documentation/BoostDoc/boost_1_29_0/libs/test/doc/minimal.htm
 */
int test_main(int argc, char *argv[]) {
	return 1;
}

int main(int argc, char * argv[]) {

#if defined(CC_MSVC)
	new MemoryDump("WengoPhoneNG");
#endif

	//Graphical interface implementation
	PFactory * pFactory = NULL;
#ifdef GTKINTERFACE
	pFactory = new GtkFactory(argc, argv);
#else
	pFactory = new QtFactory(argc, argv);
#endif
	PFactory::setFactory(pFactory);

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

	WengoPhone wengoPhone;
	CWengoPhone cWengoPhone(wengoPhone);

	//Starts the model component thread
	cWengoPhone.start();

	return pFactory->exec();
}
