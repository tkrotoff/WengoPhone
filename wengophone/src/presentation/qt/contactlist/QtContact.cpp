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

#include "QtContact.h"

#include "QtContactList.h"

#include <control/contactlist/CContact.h>

#include <Logger.h>

using namespace std;

QtContact::QtContact(CContact & cContact, QtContactList * qtContactList) 
	: QObjectThreadSafe(), PContact(cContact.getContact()), _cContact(cContact) {

	_qtContactList = qtContactList;
	
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContact::initThreadSafe, this));
	postEvent(event);
}

QtContact::~QtContact() {
}

void QtContact::initThreadSafe() {
}

void QtContact::updatePresentation() {
	_qtContactList->updatePresentation();
}

void QtContact::updatePresentationThreadSafe() {
}

string QtContact::getDisplayName() const {
	return _cContact.getDisplayName();
}

string QtContact::getId() const {
	return _cContact.getId();
}

bool QtContact::hasIM() const {
	return _cContact.hasIM();
}

bool QtContact::hasCall() const {
	return _cContact.hasCall();
}

bool QtContact::hasVideo() const {
	return _cContact.hasVideo();
}

EnumPresenceState::PresenceState QtContact::getPresenceState() const {
	return _cContact.getPresenceState();
}

void QtContact::contactModifiedEventHandler(Contact & sender) {
		updatePresentation();
}
