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

#include <coipmanager_threaded/session/TSession.h>

#include <coipmanager_threaded/TCoIpManager.h>

#include <coipmanager/session/Session.h>

#include <util/PostEvent.h>
#include <util/SafeConnect.h>

TSession::TSession(TCoIpManager & tCoIpManager, Session * session)
	: TCoIpModule(tCoIpManager, session) {

	SAFE_CONNECT(session, SIGNAL(contactAddedSignal(Contact)),
		SIGNAL(contactAddedSignal(Contact)));

	SAFE_CONNECT(session, SIGNAL(contactRemovedSignal(Contact)),
		SIGNAL(contactRemovedSignal(Contact)));
}

TSession::~TSession() {
}

Session * TSession::getSession() const {
	return static_cast<Session *>(_module);
}

EnumSessionError::SessionError TSession::addContact(const Contact & contact) {
	return getSession()->addContact(contact);
}

void TSession::removeContact(const std::string & contactId) {
	PostEvent::invokeMethod(getSession(), "removeContact",
			Q_ARG(std::string, contactId));
}

EnumSessionError::SessionError TSession::setContactList(const ContactList & contactList) {
	return getSession()->setContactList(contactList);
}

ContactList TSession::getContactList() const {
	return getSession()->getContactList();
}

Account * TSession::getAccount() const {
	return getSession()->getAccount();
}
