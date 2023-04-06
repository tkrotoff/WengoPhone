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

#include "Contact.h"

#include "UpdateContactEvent.h"
#include "PresenceContactEvent.h"
#include "database/Database.h"
#include "database/FileTarget.h"
#include "database/Target.h"
#include "Softphone.h"
#include "sip/SipPrimitives.h"
#include "XmlContactWriter.h"

#include <exception/FileNotFoundException.h>

#include <qdir.h>

#include <iostream>
using namespace std;

/*Contact::Contact(const QString & filename) {
	_id = generateId();
	_filename = filename;
}*/

Contact::Contact() {
	_id = generateId();

	_firstName = QString::null;
	_lastName = QString::null;
	_sex = Unknow;
	_birthdate = QDate();
	_picture = QString::null;
	_website = QString::null;
	_company = QString::null;
	_wengoPhone = QString::null;
	_mobilePhone = QString::null;
	_homePhone = QString::null;
	_workPhone = QString::null;
	_otherPhone = QString::null;
	_fax = QString::null;
	_personalEmail = QString::null;
	_workEmail = QString::null;
	_otherEmail = QString::null;
	_notes = QString::null;
	_blocked = false;
	_wengoPhoneValidity = true;
}

void Contact::saveAsVCard() {
	using namespace database;

	Database & database = Database::getInstance();
	QString vcard = database.transformToVCard(*this);

	FileTarget * target = NULL;

	if (!hasFilename()) {
		QString filePath = Softphone::getInstance().getContactListDir() + toString() + "_" +
					getId().toString() + "." + Softphone::CONTACT_FILE_EXTENSION;
		setFilename(filePath);
	}

	try {
		target = new FileTarget(getFilename());
	} catch (const FileNotFoundException & e) {
		cerr << e.what() << endl;
		delete target;
		return;
	}

	database.load(vcard, *target);
	delete target;
}

void Contact::save() {
	using namespace database;

	saveAsVCard();

	FileTarget * target = NULL;
	try {
		QString filePath = getFilename();
		filePath.remove("." + Softphone::CONTACT_FILE_EXTENSION);
		filePath += ".xml";
		target = new FileTarget(filePath);
	} catch (const FileNotFoundException & e) {
		cerr << e.what() << endl;
		delete target;
		return;
	}

	XmlContactWriter writer(*this);
	target->write(writer.toString());
	delete target;
}

/*Contact::Contact(const QString & firstName, const QString & lastName) {
	_id = generateId();
	_firstName = firstName;
	_lastName = lastName;
}*/

QString Contact::getDefaultPhone() const {
	if (getPhoneList().isEmpty()) {
		return QString::null;
	} else {
		return *getPhoneList().begin();
	}
}

QStringList Contact::getPhoneList() const {
	QStringList phoneList;
	if (!getWengoPhone().isEmpty()) phoneList += getWengoPhone();
	if (!getMobilePhone().isEmpty()) phoneList += getMobilePhone();
	if (!getHomePhone().isEmpty()) phoneList += getHomePhone();
	if (!getWorkPhone().isEmpty()) phoneList += getWorkPhone();
	if (!getOtherPhone().isEmpty()) phoneList += getOtherPhone();
	return phoneList;
}

void Contact::setFirstName(const QString & firstName) {
	if (_firstName != firstName) {
		_firstName = firstName;
		UpdateContactEvent event(*this);
		notify(event);
	}
}

void Contact::setLastName(const QString & lastName) {
	if (_lastName != lastName) {
		_lastName = lastName;
		UpdateContactEvent event(*this);
		notify(event);
	}
}

void Contact::setCompany(const QString & company) {
	if (_company != company) {
		_company = company;
		UpdateContactEvent event(*this);
		notify(event);
	}
}

void Contact::setWengoPhone(const QString & wengoPhone) {
	if (_wengoPhone != wengoPhone) {
		_wengoPhone = wengoPhone;
		if (!wengoPhone.isEmpty()) {
			PresenceContactEvent event(*this);
			notify(event);
		}
		UpdateContactEvent event(*this);
		notify(event);
	}
}

void Contact::setWengoPhoneWithoutSubscribe(const QString & wengoPhone) {
	if (_wengoPhone != wengoPhone) {
		_wengoPhone = wengoPhone;
		UpdateContactEvent event(*this);
		notify(event);
	}
}

void Contact::setMobilePhone(const QString & mobilePhone) {
	if (_mobilePhone != mobilePhone) {
		_mobilePhone = mobilePhone;
		UpdateContactEvent event(*this);
		notify(event);
	}
}

void Contact::setHomePhone(const QString & homePhone) {
	if (_homePhone != homePhone) {
		_homePhone = homePhone;
		UpdateContactEvent event(*this);
		notify(event);
	}
}

void Contact::setWorkPhone(const QString & workPhone) {
	if (_workPhone != workPhone) {
		_workPhone = workPhone;
		UpdateContactEvent event(*this);
		notify(event);
	}
}

void Contact::setPresenceStatus(const PresenceStatus & presenceStatus) {
	//if (_presenceStatus.getStatusText() != _presenceStatus.getStatusText()) {
		_presenceStatus = presenceStatus;
		UpdateContactEvent event(*this);
		notify(event);
	//}
}

void Contact::setBlocked(bool blocked) {
	if (_blocked != blocked) {
		_blocked = blocked;
		UpdateContactEvent event(*this);
		notify(event);
	}
}

void Contact::setWengoPhoneValidity(bool wengoPhoneValidity) {
	if (_wengoPhoneValidity != wengoPhoneValidity) {
		_wengoPhoneValidity = wengoPhoneValidity;
		UpdateContactEvent event(*this);
		notify(event);
	}
}

QString Contact::toString() const {
	QString display;

	if (!getLastName().isEmpty()) {
		display += getLastName();
	}
	if (!getFirstName().isEmpty()) {
		if (!getLastName().isEmpty()) {
			display += ", " + getFirstName();
		} else {
			display += getFirstName();
		}
	}
	if (!getCompany().isEmpty()) {
		display += " - " + getCompany();
	}

	if (display.isEmpty()) {
		display += getDefaultPhone();
	}

	return display;
}

QString Contact::generateId() {
	static unsigned int count = ContactId::UNKNOW + 1;
	QString date = "";//QDateTime::currentDateTime().toString("mmss");
	date += QString::number(count++);
	return date;
}
