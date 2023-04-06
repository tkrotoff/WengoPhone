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

#include "ContactList.h"

#include "AddContactEvent.h"
#include "RemoveContactEvent.h"
#include "UpdateContactEvent.h"
#include "Softphone.h"

#include <qdir.h>

#include <cassert>
#include <iostream>
using namespace std;

ContactList::ContactList() {
}

ContactList::~ContactList() {
	eraseAll();
}

void ContactList::addContact(Contact & contact) {
	_contactList.push_back(&contact);
	AddContactEvent event(contact);
	notify(event);
}

bool ContactList::removeContact(Contact & contact) {
	//Deletes the VCard on the harddisk
	if (contact.hasFilename()) {
		QString filename = contact.getFilename();
		QFile file(filename);
		file.remove();
		filename.remove("." + Softphone::CONTACT_FILE_EXTENSION);
		filename.remove(".old");
		filename += ".xml";
		file.setName(filename);
		file.remove();
	}

	int count = _contactList.size();
	int i;
	for (i = 0; i < count; i++) {
		if (_contactList[i] == & contact) break;
	}
	if (i < count) {
		RemoveContactEvent event(contact);
		notify(event);
		_contactList.erase(_contactList.begin() + i);
		//delete contact;
		return true;
	}
	return false;
}

bool ContactList::contains(const Contact & contact) {
	for (unsigned int i = 0; i < _contactList.size(); i++) {
		if (_contactList[i]->getId() == contact.getId()) {
			return true;
		}
	}
	return false;
}

const Contact & ContactList::getContact(const ContactId & contactId) const {
	for (unsigned int i = 0; i < _contactList.size(); i++) {
		if (_contactList[i]->getId() == contactId) {
			return *_contactList[i];
		}
	}

	assert(NULL && "The contact cannot been found inside the list");
	return *new Contact();	//Removes the compiler warning
}

Contact & ContactList::getContact(const ContactId & contactId) {
	for (unsigned int i = 0; i < _contactList.size(); i++) {

		if (_contactList[i]->getId() == contactId) {
			return *_contactList[i];
		}
	}

	assert(NULL && "The contact cannot been found inside the list");
	return *new Contact();	//Removes the compiler warning
}

Contact * ContactList::getContact(const QString & phoneNumber) {
	for (unsigned int i = 0; i < _contactList.size(); i++) {
		if (*_contactList[i] == phoneNumber) {
			return _contactList[i];
		}
	}

	//If a Contact cannot be found given the phone number
	return NULL;
}

unsigned int ContactList::size() const {
	return _contactList.size();
}

const Contact & ContactList::operator[] (unsigned int i) const {
	return *_contactList[i];
}

Contact & ContactList::operator[] (unsigned int i) {
	return *_contactList[i];
}

void ContactList::reset() {
	//Deletes all the contacts from the ContactListView
	for (unsigned int i = 0; i < _contactList.size(); i++) {
		RemoveContactEvent event(operator[](i));
		notify(event);
	}

	eraseAll();
}

void ContactList::eraseAll() {
	//Deletes all the contacts
	for (unsigned int i = 0; i < _contactList.size(); i++) {
		delete _contactList[i];
	}

	_contactList.clear();
}
