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
#include "chat/QtChatHandler.h"
#include "contactlist/QtContactList.h"
#include "history/QtHistory.h"
#include "phoneline/QtPhoneLine.h"
#include "phonecall/QtPhoneCall.h"
#include "profile/QtUserProfile.h"
#include "profile/QtUserProfileHandler.h"
#include "webservices/sms/QtSms.h"
#include "webservices/softupdate/QtSoftUpdate.h"
#include "webservices/subscribe/QtSubscribe.h"
#include "webservices/directory/QtWsDirectory.h"
#include "wenbox/QtWenboxPlugin.h"

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

#ifdef OS_MACOSX
		std::string qtPlugins = Path::getApplicationPrivateFrameworksDirPath() +
			File::convertPathSeparators("qt-plugins/");

		QCoreApplication::addLibraryPath(QString::fromStdString(qtPlugins));
#endif

		reset();
		_qtWengoPhone = NULL;
		_qtUserProfileHandler = NULL;
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
		if (!_qtWengoPhone) {
			_qtWengoPhone = new QtWengoPhone(cWengoPhone);
		}

		return _qtWengoPhone;
	}

	PUserProfile * createPresentationUserProfile(CUserProfile & cUserProfile) {
		//FIXME: QtWengoPhone must be instanciated before any QtUserProfile
		return new QtUserProfile(cUserProfile, *_qtWengoPhone);
	}

	PUserProfileHandler * createPresentationUserProfileHandler(CUserProfileHandler & cUserProfileHandler) {
		if (!_qtUserProfileHandler) {
			//FIXME: QtWengoPhone must be instanciated before _qtUserProfileHandler
			_qtUserProfileHandler = new QtUserProfileHandler(cUserProfileHandler, *_qtWengoPhone);
		}
		
		return _qtUserProfileHandler;
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

	QtWengoPhone * _qtWengoPhone;

	QtUserProfileHandler * _qtUserProfileHandler;
};

#endif	//QTFACTORY_H
