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

#ifndef QTFACTORY_H
#define QTFACTORY_H

#include <presentation/PFactory.h>

#include "QtWengoPhone.h"
#include <control/CWengoPhone.h>

#include "phoneline/QtPhoneLine.h"
#include <control/phoneline/CPhoneLine.h>

#include "phonecall/QtPhoneCall.h"
#include <control/phonecall/CPhoneCall.h>

#include "contactlist/QtContactList.h"
#include <control/contactlist/CContactList.h>

#include "wenbox/QtWenboxPlugin.h"
#include <control/wenbox/CWenboxPlugin.h>

#include "chat/QtChatHandler.h"

#include "webservices/sms/QtSms.h"
#include <control/webservices/sms/CSms.h>

#include "webservices/softupdate/QtSoftUpdate.h"
#include <control/webservices/softupdate/CSoftUpdate.h>

#include "history/QtHistory.h"
#include <control/history/CHistory.h>

#include "webservices/subscribe/QtSubscribe.h"
#include <control/webservices/subscribe/CSubscribe.h>

#include "webservices/directory/QtWsDirectory.h"
#include <control/webservices/directory/CWsDirectory.h>

#include <util/File.h>
#include <util/Logger.h>
#include <util/Path.h>

#include <cutil/global.h>

#include <QApplication>

#include <boost/program_options.hpp>
#include <iostream>
using namespace boost::program_options;
using namespace std;

#ifdef OS_MACOSX
	#include <CoreFoundation/CoreFoundation.h>
#endif

PFactory * PFactory::_factory = 0;

class QtFactory : public PFactory {
public:

	QtFactory(int argc, char * argv[]) {
		_app = new QApplication(argc, argv);
		QCoreApplication::addLibraryPath(".");

		_background = false;
		try {

			options_description desc("Allowed options");
			desc.add_options()
			// First parameter describes option name/short name
			// The second is parameter to option
			// The third is description
					("help,h", "print usage message")
					("background,b", "run in background mode")
					;

			variables_map vm;
			store(parse_command_line(argc, argv, desc), vm);

			if (vm.count("help")) {
				cout << desc << "\n";
				exit(0);
			}

			if (vm.count("background")) {
				LOG_DEBUG("run in background mode");
				_background = true;
			}
		}
		catch(exception& e) {
			cerr << e.what() << "\n";
		}

#ifdef OS_MACOSX
		std::string qtPlugins = Path::getApplicationPrivateFrameworksDirPath() +
			File::convertPathSeparators("qt-plugins/");

		QCoreApplication::addLibraryPath(QString::fromStdString(qtPlugins));
#endif

		reset();
	}

	void processEvents() {
		_app->processEvents();
	}

	int exec() {
		return _app->exec();
	}

	void reset() {
		// Objects are deleted by the class who constructs them.
		// So we can set the pointer to NULL safely.
		_qtContactList = NULL;
		_qtChatHandler = NULL;
		_qtWenboxPlugin = NULL;
		_qtSms = NULL;
		_qtSubscribe = NULL;
		_qtWsDirectory = NULL;
		_qtSoftUpdate = NULL;
		_qtHistory = NULL;
	}

	PWengoPhone * createPresentationWengoPhone(CWengoPhone & cWengoPhone) {
		_cWengoPhone = &cWengoPhone;
		static QtWengoPhone qtWengoPhone(cWengoPhone, _background);
		return &qtWengoPhone;
	}

	PPhoneLine * createPresentationPhoneLine(CPhoneLine & cPhoneLine) {
		//FIXME: memory leak?
		QtPhoneLine * qtPhoneLine = new QtPhoneLine(cPhoneLine);
		return qtPhoneLine;
	}

	PPhoneCall * createPresentationPhoneCall(CPhoneCall & cPhoneCall) {
		//FIXME: memory leak?
		QtPhoneCall * qtPhoneCall = new QtPhoneCall(cPhoneCall);
		return qtPhoneCall;
	}

	PContactList * createPresentationContactList(CContactList & cContactList) {
		if (!_qtContactList) {
			if (!_cWengoPhone) {
				LOG_FATAL("PWengoPhone must be created before PContactList");
			}
			_qtContactList = new QtContactList(cContactList, *_cWengoPhone);
		}
		return _qtContactList;
	}

	PWenboxPlugin * createPresentationWenboxPlugin(CWenboxPlugin & cWenboxPlugin) {
		if (!_qtWenboxPlugin) {
			_qtWenboxPlugin = new QtWenboxPlugin(cWenboxPlugin);
		}
		return _qtWenboxPlugin;
	}

	PChatHandler * createPresentationChatHandler(CChatHandler & cChatHandler) {
		if (!_qtChatHandler) {
			_qtChatHandler = new QtChatHandler(cChatHandler);
		}	
		return _qtChatHandler;
	}

	PSms * createPresentationSms(CSms & cSms) {
		if (!_qtSms) {
			_qtSms = new QtSms(cSms);
		}	
		return _qtSms;
	}

	PSubscribe * createPresentationSubscribe(CSubscribe & cSubscribe) {
		if (!_qtSubscribe) {
			_qtSubscribe = new QtSubscribe(cSubscribe);
		}
		return _qtSubscribe;
	}

	PWsDirectory * createPresentationWsDirectory(CWsDirectory & cWsDirectory) {
		if (!_qtWsDirectory) {
			_qtWsDirectory = new QtWsDirectory(cWsDirectory);
		}
		return _qtWsDirectory;
	}

	PSoftUpdate * createPresentationSoftUpdate(CSoftUpdate & cSoftUpdate) {
		if (!_qtSoftUpdate) {
			_qtSoftUpdate = new QtSoftUpdate(cSoftUpdate);
		}
		return _qtSoftUpdate;
	}

	PHistory * createPresentationHistory(CHistory & cHistory) {
		if (!_qtHistory) {
			_qtHistory = new QtHistory(cHistory);
		}
		return _qtHistory;
	}

private:

	QApplication * _app;

	QtContactList * _qtContactList;

	QtChatHandler * _qtChatHandler;

	QtWenboxPlugin * _qtWenboxPlugin;

	QtSms * _qtSms;

	QtSubscribe * _qtSubscribe;

	QtWsDirectory * _qtWsDirectory;

	QtSoftUpdate * _qtSoftUpdate;

	QtHistory * _qtHistory;

	CWengoPhone * _cWengoPhone;

	bool _background;
};

#endif	//QTFACTORY_H
