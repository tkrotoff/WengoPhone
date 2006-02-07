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

#ifndef XPCOMFACTORY_H
#define XPCOMFACTORY_H

#include "XPCOMWengoPhone.h"
#include "XPCOMWengoPhoneLogger.h"
#include "XPCOMPhoneLine.h"
#include "XPCOMPhoneCall.h"
#include "XPCOMContact.h"
#include "XPCOMContactGroup.h"
#include "XPCOMContactList.h"

#include <presentation/PFactory.h>
#include <control/CWengoPhone.h>
#include <control/phoneline/CPhoneLine.h>
#include <control/phonecall/CPhoneCall.h>
#include <control/contactlist/CContactList.h>
#include <control/contactlist/CContact.h>
#include <control/contactlist/CContactGroup.h>

PFactory * PFactory::_factory = 0;

class XPCOMFactory : public PFactory {
public:

	XPCOMFactory() {
	}

	~XPCOMFactory() {
	}

	int exec() {
		return true;
	}

	PWengoPhone * createPresentationWengoPhone(CWengoPhone & cWengoPhone) {
		static XPCOMWengoPhone xpcomWengoPhone(&cWengoPhone);
		return &xpcomWengoPhone;
	}

	PPhoneLine * createPresentationPhoneLine(CPhoneLine & cPhoneLine) {
		XPCOMPhoneLine * xpcomPhoneLine = new XPCOMPhoneLine(cPhoneLine);
		return xpcomPhoneLine;
	}

	PPhoneCall * createPresentationPhoneCall(CPhoneCall & cPhoneCall) {
		XPCOMPhoneCall * xpcomPhoneCall = new XPCOMPhoneCall(cPhoneCall);
		return xpcomPhoneCall;
	}

	PWengoPhoneLogger * createPresentationLogger(CWengoPhoneLogger & cWengoPhoneLogger) {
		static XPCOMWengoPhoneLogger xpcomWengoPhoneLogger(cWengoPhoneLogger);
		return &xpcomWengoPhoneLogger;
	}

	PContactList * createPresentationContactList(CContactList & cContactList) {
		static XPCOMContactList xpcomContactList(cContactList);
		return &xpcomContactList;
	}

	PContactGroup * createPresentationContactGroup(CContactGroup & cContactGroup) {
		XPCOMContactGroup * xpcomContactGroup = new XPCOMContactGroup(cContactGroup);
		return xpcomContactGroup;
	}

	PContact * createPresentationContact(CContact & cContact) {
		XPCOMContact * xpcomContact = new XPCOMContact(cContact);
		return xpcomContact;
	}

	PWenboxPlugin * createPresentationWenboxPlugin(CWenboxPlugin & cWenboxPlugin) {
		return NULL;
	}

	PConnectHandler * createPresentationConnectHandler(CConnectHandler & cConnectHandler) {
		return NULL;
	}

	PPresenceHandler * createPresentationPresenceHandler(CPresenceHandler & cPresenceHandler) {
		return NULL;
	}

	PChatHandler * createPresentationChatHandler(CChatHandler & cChatHandler) {
		return NULL;
	}

private:
};

#endif	//XPCOMFACTORY_H
