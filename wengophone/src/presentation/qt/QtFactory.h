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

#ifndef QTFACTORY_H
#define QTFACTORY_H

#include "presentation/PFactory.h"
#include "QtWengoPhone.h"
#include "QtWengoPhoneLogger.h"
#include "control/CWengoPhone.h"
#include "phoneline/QtPhoneLine.h"
#include "control/phoneline/CPhoneLine.h"
#include "phonecall/QtPhoneCall.h"
#include "control/phonecall/CPhoneCall.h"
#include "contactlist/QtContactList.h"
#include "control/contactlist/CContactList.h"
#include "contactlist/QtContact.h"
#include "control/contactlist/CContact.h"
#include "contactlist/QtContactGroup.h"
#include "control/contactlist/CContactGroup.h"
#include "wenbox/QtWenboxPlugin.h"
#include "control/wenbox/CWenboxPlugin.h"
#include "control/CIMHandler.h"
#include "control/chat/CChat.h"
#include "control/presence/CPresence.h"
#include "presence/QtPresenceHandler.h"

#include <QApplication>

PFactory * PFactory::_factory = 0;

class QtFactory : public PFactory {
public:

	QtFactory(int argc, char * argv[]) {
		_app = new QApplication(argc, argv);
		_qtContactList = NULL;
	}

	~QtFactory() {
		delete _app;
		delete _qtContactList;
	}

	int exec() {
		return _app->exec();
	}

	PWengoPhone * createPresentationWengoPhone(CWengoPhone & cWengoPhone) {
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

	PWengoPhoneLogger * createPresentationLogger(CWengoPhoneLogger & cWengoPhoneLogger) {
		static QtWengoPhoneLogger qtWengoPhoneLogger(cWengoPhoneLogger);
		return &qtWengoPhoneLogger;
	}

	PContactList * createPresentationContactList(CContactList & cContactList) {
		if (!_qtContactList) {
			_qtContactList = new QtContactList(cContactList);
		}
		return _qtContactList;
	}

	PContactGroup * createPresentationContactGroup(CContactGroup & cContactGroup) {
		QtContactGroup * qtContactGroup = new QtContactGroup(cContactGroup, _qtContactList);
		return qtContactGroup;
	}

	PContact * createPresentationContact(CContact & cContact) {
		QtContact * qtContact = new QtContact(cContact, _qtContactList);
		return qtContact;
	}

	PWenboxPlugin * createPresentationWenboxPlugin(CWenboxPlugin & cWenboxPlugin) {
		QtWenboxPlugin * qtWenboxPlugin = new QtWenboxPlugin(cWenboxPlugin);
		return qtWenboxPlugin;
	}

	PIMHandler * createPresentationIMHandler(CIMHandler & cIMHandler) {
		return NULL;
	}

	PChat * createPresentationChat(CChat & cChat) {
		return NULL;
	}

	PPresence * createPresentationPresence(CPresence & cPresence) {
		return NULL;
	}

	PConnectHandler * createPresentationConnectHandler(CConnectHandler & cConnectHandler) {
		return NULL;
	}

	PPresenceHandler * createPresentationPresenceHandler(CPresenceHandler & cPresenceHandler) {
		static QtPresenceHandler * qtPresenceHandler = new QtPresenceHandler(cPresenceHandler);
		return qtPresenceHandler;
	}

	PChatHandler * createPresentationChatHandler(CChatHandler & cChatHandler) {
		return NULL;
	}

private:

	QApplication * _app;

	QtContactList * _qtContactList;
};

#endif	//QTFACTORY_H
