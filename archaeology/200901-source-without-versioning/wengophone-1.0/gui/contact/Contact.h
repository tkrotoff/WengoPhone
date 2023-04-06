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

#ifndef CONTACT_H
#define CONTACT_H

#include "StreetAddress.h"
#include "ContactId.h"
#include "presence/PresenceStatus.h"
#include "sip/SipAddress.h"

#include <observer/Subject.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qdatetime.h>

class UpdateContactEvent;

/**
 * Represents a Contact with name, address, phone numbers, email...
 *
 * Design Pattern Observer.
 * When a Contact is updated, the Contact will directy talk with ContactListWidget via
 * the design pattern observer (MVC)
 *
 * @see Observer
 * @see Subject
 * @author Tanguy Krotoff
 */
class Contact : public Subject {
public:

	/**
	 * Gender of the Contact.
	 */
	enum Sex {
		Unknow,
		Male,
		Female
	};

	/**
	 * Constructs a Contact given a file.
	 *
	 * @param filename file that represents a Contact
	 */
	//Contact(const QString & filename);
	Contact();

	/**
	 * Constructs a Contact given a first name and a last name.
	 *
	 * @param firstName first name of the Contact
	 * @param lastName last name of the Contact
	 */
	//Contact(const QString & firstName, const QString & lastName);

	~Contact() {
	}

	/**
	 * Compares two Contact together.
	 *
	 * <code>
	 * Contact toto = ...
	 * Contact titi = ...
	 * if (toto == titi) {
	 * }
	 * </code>
	 *
	 * @param contact1 left part of the operator
	 * @param contact2 right part of the operator
	 * @return true if both Contact are equals, false otherwise
	 */
	friend bool operator==(const Contact & contact1, const Contact & contact2) {
		return contact1._id == contact2._id;
	}

	friend bool operator==(const Contact & contact, const QString & phoneNumber) {
		QStringList phoneList = contact.getPhoneList();
		for (QStringList::Iterator it = phoneList.begin(); it != phoneList.end(); ++it) {
			if (*it == phoneNumber ||
				SipAddress::fromPhoneNumber(*it) ==
				SipAddress::fromPhoneNumber(phoneNumber)) {

				return true;
			}
		}
		return false;
	}

	/**
	 * Saves the Contact to a file.
	 */
	void save();

	void setFilename(const QString & filename) {
		_filename = filename;
	}

	/**
	 * Gets the filename associates with this Contact.
	 *
	 * @return filename
	 */
	QString getFilename() const {
		return _filename;
	}

	bool hasFilename() const {
		return !_filename.isNull();
	}

	/**
	 * Generates a string representing the Contact.
	 * Used by ContactListWidget in order to show the contacts.
	 *
	 * @return string that represents the Contact
	 */
	virtual QString toString() const;

	const ContactId & getId() const { return _id; }

	QString getFullName() const { return getFirstName() + " " + getLastName(); }

	void setFirstName(const QString & firstName);
	QString getFirstName() const { return _firstName; }

	void setLastName(const QString & lastName);
	QString getLastName() const { return _lastName; }

	void setSex(Sex sex) { _sex = sex; }
	Sex getSex() const { return _sex; }

	void setBirthdate(const QDate & birthdate) { _birthdate = birthdate; }
	QDate getBirthdate() const { return _birthdate; }

	void setPicture(const QString & picture) { _picture = picture; }
	QString getPicture() const { return _picture; }

	void setWebsite(const QString & website) { _website = website; }
	QString getWebsite() const { return _website; }

	void setCompany(const QString & company);
	QString getCompany() const { return _company; }

	QStringList getPhoneList() const;

	QString getDefaultPhone() const;

	void setWengoPhone(const QString & wengoPhone);
	void setWengoPhoneWithoutSubscribe(const QString & wengoPhone);
	QString getWengoPhone() const { return _wengoPhone; }

	void setMobilePhone(const QString & mobilePhone);
	QString getMobilePhone() const { return _mobilePhone; }

	void setHomePhone(const QString & homePhone);
	QString getHomePhone() const { return _homePhone; }

	void setWorkPhone(const QString & workPhone);
	QString getWorkPhone() const { return _workPhone; }

	void setOtherPhone(const QString & otherPhone) { _otherPhone = otherPhone; }
	QString getOtherPhone() const { return _otherPhone; }

	void setFax(const QString & fax) { _fax = fax; }
	QString getFax() const { return _fax; }

	void setPersonalEmail(const QString & personalEmail) { _personalEmail = personalEmail; }
	QString getPersonalEmail() const { return _personalEmail; }

	void setWorkEmail(const QString & workEmail) { _workEmail = workEmail; }
	QString getWorkEmail() const { return _workEmail; }

	void setOtherEmail(const QString & otherEmail) { _otherEmail = otherEmail; }
	QString getOtherEmail() const { return _otherEmail; }

	void setStreetAddress(const StreetAddress & streetAddress) { _streetAddress = streetAddress; }
	StreetAddress getStreetAddress() const { return _streetAddress; }

	void setNotes(const QString & notes) { _notes = notes; }
	QString getNotes() const { return _notes; }

	void setPresenceStatus(const PresenceStatus & presenceStatus);
	const PresenceStatus & getPresenceStatus() const { return _presenceStatus; }

	void setBlocked(bool blocked);
	bool isBlocked() const { return _blocked; }

	void setWengoPhoneValidity(bool wengoPhoneValidity);
	bool isWengoPhoneValid() const { return _wengoPhoneValidity; }

private:

	Contact(const Contact &);
	Contact & operator=(const Contact &);

	/**
	 * Adds the Contact to a VCard file.
	 */
	void saveAsVCard();

	/**
	 * Generates the unique ID of the contact.
	 *
	 * The unique ID is a simple counter.
	 *
	 * @return the generated unique ID
	 */
	static QString generateId();

	/**
	 * File corresponding to the Contact.
	 */
	QString _filename;

	/**
	 * Unique ID of the Contact.
	 */
	ContactId _id;

	/**
	 * A Contact can be part of a group: friends, job...
	 */
	QString _group;

	/** Personal Details */
	QString _firstName;
	QString _lastName;
	Sex _sex;
	QDate _birthdate;
	QString _picture;
	QString _website;
	QString _company;

	/** Phone Numbers */
	QString _wengoPhone;
	QString _mobilePhone;
	QString _homePhone;
	QString _workPhone;
	QString _otherPhone;
	QString _fax;

	/** Online Addresses */
	QString _personalEmail;
	QString _workEmail;
	QString _otherEmail;

	/**
	 * Street Address.
	 */
	StreetAddress _streetAddress;

	/**
	 * Contact presence status.
	 */
	PresenceStatus _presenceStatus;

	/**
	 * Notes: a brief text attached to a Contact.
	 */
	QString _notes;

	/**
	 * If the Contact is blocked: he cannot see my presence status.
	 *
	 * Default is false.
	 */
	bool _blocked;

	/**
	 * If the Contact has a valid Wengo phone number (SipAddress).
	 *
	 * Default is true.
	 */
	bool _wengoPhoneValidity;
};

#endif	//CONTACT_H
