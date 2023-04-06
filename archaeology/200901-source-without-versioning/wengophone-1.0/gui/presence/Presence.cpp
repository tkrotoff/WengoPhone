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

#include "Presence.h"

#include "sip/SipPrimitives.h"
#include "sip/SipAddress.h"
#include "contact/Contact.h"
#include "contact/ContactList.h"
#include "contact/AddContactEvent.h"
#include "contact/RemoveContactEvent.h"
#include "contact/UpdateContactEvent.h"
#include "contact/PresenceContactEvent.h"
#include "presence/PresenceStatus.h"
#include "connect/Connect.h"
#include "Softphone.h"
#include "WatcherList.h"
#include "XmlWatcherListReader.h"
#include "util/MyMessageBox.h"

#include <qstring.h>
#include <qobject.h>

#include <cassert>
#include <iostream>
#include <vector>
using namespace std;

Presence::Presence() {
	//For the design pattern observer
	ContactList::getInstance().addObserver(*this);
}

Presence::~Presence() {
	//For the design pattern observer
	ContactList::getInstance().removeObserver(*this);
}

void Presence::onNotify(const QString & event, const SipAddress & from, const QString & content) {
	static std::vector<std::string> alreadyNotified;

	Contact * contact = ContactList::getInstance().getContact(from.getValidPhoneNumber());

	cerr << "Presence::onNotify: " << event << " " << from.getValidPhoneNumber() << endl;

	if (event == "presence") {
		if (!contact) {
			//We don't care about this person because
			//this guy is not inside the ContactList
			return;
		}

		if (content.find("<basic>closed</basic>") != -1) {
			contact->setPresenceStatus(PresenceStatus(PresenceStatus::Offline));
			cerr << "Presence::onNotify: contact offline: " << contact->getWengoPhone() << endl;
		}

		else if (content.find("<basic>open</basic>") != -1) {
			QStringList afterNoteBegin = QStringList::split("<note>", content);
			QStringList beforeNoteEnd = QStringList::split("</note>", afterNoteBegin[1]);
			QString note = beforeNoteEnd[0];

			if (content != note) {
				contact->setPresenceStatus(PresenceStatus(note));
			} else {
				contact->setPresenceStatus(PresenceStatus(PresenceStatus::Online));
			}
			cerr << "Presence::onNotify: contact online: " << contact->getWengoPhone() << " " << note << endl;
		}
	}

	else if (event == "presence.winfo") {
		cerr << "Presence::onNotify: " << content << endl;
		//Parse the XML content and find the string:
		//if (content.find("<watcher status=\"pending\">") != -1) {
		XmlWatcherListReader parser(content);
		const WatcherList & watcherList = parser.getWatcherList();

		for (unsigned int i = 0; i < watcherList.size(); i++) {
			const Watcher & watcher = watcherList[i];

			//Retrieves the watcher inside the ContactList
			QString watcherUri = watcher.getSipAddress().getMinimalSipUri();
			QString watcherPhoneNumber = watcher.getSipAddress().getValidPhoneNumber();
			ContactList & contactList = ContactList::getInstance();
			Contact * contact = contactList.getContact(watcherPhoneNumber);

			if (watcher.isPending()) {
				//Watcher is pending
				if (contact) {
					if (!contact->isBlocked()) {
						allowWatcher(watcherUri);
					}
				} else {
					//Checks if the user has been already notified
					//so we don't show several similar popups.
					bool notified = false;
					for (unsigned int i = 0; i < alreadyNotified.size(); i++) {
						if (alreadyNotified[i] == watcherUri) {
							//Already notified
							notified = true;
							break;
						}
					}

					if (!notified) {
						alreadyNotified.push_back(watcherUri);
						bool allow = askAllowWatcher(watcherUri);
						contact = new Contact();
						contact->setWengoPhone(watcherPhoneNumber);
						if (allow) {
							contact->setBlocked(false);
							allowWatcher(watcherUri);
						} else {
							contact->setBlocked(true);
							forbidWatcher(watcherUri);
						}
						contact->save();
						contactList.addContact(*contact);
					}
				}
			} else {
				//Watcher is active
				if (contact) {
					if (contact->isBlocked()) {
						forbidWatcher(watcherUri);
					}
				}
			}
		}
	}

	else {
		assert(NULL && "Unknown message event");
	}
}

void Presence::subscribeToEverybodyPresence() {
	ContactList & contactList = ContactList::getInstance();
	for (unsigned int i = 0; i < contactList.size(); i++) {
		Contact & contact = contactList[i];
		if (!contact.getWengoPhone().isEmpty()) {
			subscribeToPresence(contact.getWengoPhone());
		}
	}
}

void Presence::subscriptionProgress(int /*subscriptionId*/, const SipAddress & to, bool success) {
	//Retrieves the watcher inside the ContactList
	QString watcherPhoneNumber = to.getValidPhoneNumber();
	ContactList & contactList = ContactList::getInstance();
	Contact * contact = contactList.getContact(watcherPhoneNumber);
	if (contact) {
		contact->setWengoPhoneValidity(success);
	}
}

void Presence::update(const Subject & /*subject*/, const Event & event) {
	QString typeEvent = event.getTypeEvent();

	if (typeEvent == "AddContactEvent") {
		const AddContactEvent & evt = (const AddContactEvent &)event;
		Contact & contact = evt.getContact();
		contact.addObserver(*this);

		if (!contact.getWengoPhone().isEmpty()) {
			if (!contact.isBlocked()) {
				subscribeToPresence(contact.getWengoPhone());
			}
		}
	}

	else if (typeEvent == "RemoveContactEvent") {
		const RemoveContactEvent & evt = (const RemoveContactEvent &)event;
		Contact & contact = evt.getContact();
		contact.removeObserver(*this);
	}

	else if (typeEvent == "UpdateContactEvent") {
		/*const UpdateContactEvent & evt = (const UpdateContactEvent &)event;
		const Contact & contact = evt.getContact();*/
	}

	else if (typeEvent == "PresenceContactEvent") {
		const PresenceContactEvent & evt = (const PresenceContactEvent &)event;
		const Contact & contact = evt.getContact();

		if (!contact.getWengoPhone().isEmpty()) {
			if (!contact.isBlocked()) {
				subscribeToPresence(contact.getWengoPhone());
			}
		}
	}

	else {
		assert(NULL && "Unknown message event");
	}
}

void Presence::publishOnlineStatus(const QString & note) {
	static QString previousStatusNote;

	QString tmp;
	if (note.isEmpty()) {
		tmp = previousStatusNote;
	} else {
		previousStatusNote = note;
		tmp = note;
	}

	Connect & connect = Connect::getInstance();
	if (!connect.isConnected()) {
		WaitingForConnectedSignal * waiting = new WaitingForConnectedSignal(tmp);
		QObject::connect(&connect, SIGNAL(connected()),
				waiting, SLOT(publishOnlineStatus()));
	} else {
		SipPrimitives::publishOnline(tmp.utf8());
	}
}

void Presence::publishOfflineStatus() {
	SipPrimitives::publishOffline();
}

void Presence::subscribeToPresence(const QString & sipAddress) {
	Connect & connect = Connect::getInstance();
	if (!connect.isConnected()) {
		WaitingForConnectedSignal * waiting = new WaitingForConnectedSignal(sipAddress);
		QObject::connect(&connect, SIGNAL(connected()),
				waiting, SLOT(subscribeToPresence()));
	} else {
		SipPrimitives::subscribeToPresence(SipAddress::fromPhoneNumber(sipAddress));
	}
}

void Presence::allowWatcher(const QString & sipAddress) {
	Connect & connect = Connect::getInstance();
	if (!connect.isConnected()) {
		WaitingForConnectedSignal * waiting = new WaitingForConnectedSignal(sipAddress);
		QObject::connect(&connect, SIGNAL(connected()),
				waiting, SLOT(allowWatcher()));
	} else {
		SipPrimitives::allowWatcher(SipAddress::fromPhoneNumber(sipAddress));
	}
}

void Presence::forbidWatcher(const QString & sipAddress) {
	Connect & connect = Connect::getInstance();
	if (!connect.isConnected()) {
		WaitingForConnectedSignal * waiting = new WaitingForConnectedSignal(sipAddress);
		QObject::connect(&connect, SIGNAL(connected()),
				waiting, SLOT(forbidWatcher()));
	} else {
		SipPrimitives::forbidWatcher(SipAddress::fromPhoneNumber(sipAddress));
	}
}

void Presence::askForWatcherList() {
	Connect & connect = Connect::getInstance();
	if (!connect.isConnected()) {
		WaitingForConnectedSignal * waiting = new WaitingForConnectedSignal();
		QObject::connect(&connect, SIGNAL(connected()),
				waiting, SLOT(askForWatcherList()));
	} else {
		SipPrimitives::askForWatcherList();
	}
}

bool Presence::askAllowWatcher(const QString & sipAddress) {
	int buttonClicked = MyMessageBox::question(Softphone::getInstance().getActiveWindow(),
				QObject::tr("WengoPhone"),
				sipAddress +
				QObject::tr(" wants to see your presence status.\n"
				"Do you want to allow him to see you and add him to your contact list?"),
				QObject::tr("Allow"),
				QObject::tr("Forbid"));

	switch(buttonClicked) {
	default:
	case QMessageBox::Yes:
		return true;
	case QMessageBox::No:
		return false;
	}
}


/* WaitingForConnectedSignal */

void WaitingForConnectedSignal::publishOnlineStatus() {
	disconnect(&Connect::getInstance(), 0,
		this, SLOT(publishOnlineStatus()));
	Presence::publishOnlineStatus(_param);
}

void WaitingForConnectedSignal::subscribeToPresence() {
	disconnect(&Connect::getInstance(), 0,
		this, SLOT(subscribeToPresence()));
	if(!_param.isEmpty()){
		Presence::subscribeToPresence(_param);
	}
}

void WaitingForConnectedSignal::allowWatcher() {
	disconnect(&Connect::getInstance(), 0,
		this, SLOT(allowWatcher()));
	if(!_param.isEmpty()){
		Presence::allowWatcher(_param);
	}
}

void WaitingForConnectedSignal::forbidWatcher() {
	disconnect(&Connect::getInstance(), 0,
		this, SLOT(forbidWatcher()));
	if(!_param.isEmpty()){
		Presence::forbidWatcher(_param);
	}
}

void WaitingForConnectedSignal::askForWatcherList() {
	disconnect(&Connect::getInstance(), 0,
		this, SLOT(askForWatcherList()));
	Presence::askForWatcherList();
}
