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
		_qtContactList = NULL;
		_cWengoPhone = NULL;
	}

	~QtFactory() {
		delete _app;
		delete _qtContactList;
	}

	void processEvents() {
		_app->processEvents();
	}

	int exec() {
		return _app->exec();
	}

	PWengoPhone * createPresentationWengoPhone(CWengoPhone & cWengoPhone) {
		_cWengoPhone = &cWengoPhone;
		static QtWengoPhone qtWengoPhone(cWengoPhone);
		return &qtWengoPhone;
	}

	PPhoneLine * createPresentationPhoneLine(CPhoneLine & cPhoneLine) {
		QtPhoneLine * qtPhoneLine = new QtPhoneLine(cPhoneLine);
		return qtPhoneLine;
	}

	PPhoneCall * createPresentationPhoneCall(CPhoneCall & cPhoneCall) {
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
		QtWenboxPlugin * qtWenboxPlugin = new QtWenboxPlugin(cWenboxPlugin);
		return qtWenboxPlugin;
	}

	PChatHandler * createPresentationChatHandler(CChatHandler & cChatHandler) {
		static QtChatHandler * qtChatHandler = new QtChatHandler(cChatHandler);
		return qtChatHandler;
	}

	PSms * createPresentationSms(CSms & cSms) {
		static QtSms * qtSms = new QtSms(cSms);
		return qtSms;
	}

	PSubscribe * createPresentationSubscribe(CSubscribe & cSubscribe) {
		static QtSubscribe * qtSubscribe = new QtSubscribe(cSubscribe);
		return qtSubscribe;
	}

	PWsDirectory * createPresentationWsDirectory(CWsDirectory & cWsDirectory) {
		static QtWsDirectory * qtWsDirectory = new QtWsDirectory(cWsDirectory);
		return qtWsDirectory;
	}

	PSoftUpdate * createPresentationSoftUpdate(CSoftUpdate & cSoftUpdate) {
		static QtSoftUpdate * qtSoftUpdate = new QtSoftUpdate(cSoftUpdate);
		return qtSoftUpdate;
	}

	PHistory * createPresentationHistory(CHistory & cHistory) {
		static QtHistory * qtHistory = new QtHistory(cHistory);
		return qtHistory;
	}

private:

	QApplication * _app;

	QtContactList * _qtContactList;

	CWengoPhone * _cWengoPhone;

};

#endif	//QTFACTORY_H
