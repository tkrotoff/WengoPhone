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

#ifndef CONTACTWIDGET_H
#define CONTACTWIDGET_H

#include "Contact.h"
#include "StreetAddress.h"

#include <qobject.h>
#include <qstring.h>

class QWidget;
class QLineEdit;
class QComboBox;
class QDateEdit;
class QTextEdit;

/**
 * Gui widget for a Contact.
 *
 * Graphical representation of a Contact: permits to the user
 * to enter the name, phone numbers, address... of a Contact.
 *
 * @see ContactWidgetForm.ui
 * @author Tanguy Krotoff
 */
class ContactWidget : public QObject {
	Q_OBJECT
public:

	/**
	 * Constructs an empty ContactWidget.
	 */
	ContactWidget(QWidget * parent);

	~ContactWidget();

	/**
	 * Constructs a ContactWidget using a corresponding Contact.
	 *
	 * @param contact Contact that will be represented graphically by the ContactWidget
	 */
	void setContact(Contact & contact);

	/**
	 * Gets the low-level widget that represents the gui window.
	 */
	QWidget * getWidget() const {
		return _contactWidget;
	}

	/**
	 * Resets to default the widget.
	 */
	void reset();

public slots:

	/**
	 * Updates or creates a Contact using the informations contained in the ContactWidget.
	 *
	 * If the Contact is not present inside the ContactList, the Contact is added to it.
	 * No Contact object is passed in parameter because Qt slots doesn't
	 * accept default values. Thus setContact() must be used before to use this slot.
	 * @see setContact()
	 */
	void updateContact();

private:

	ContactWidget(const ContactWidget &);
	ContactWidget & operator=(const ContactWidget &);

	void setId(const ContactId & contactId) { _contactId = contactId; }
	const ContactId & getId() const { return _contactId; }

	void setFirstName(const QString & firstName);
	QString getFirstName() const;

	void setLastName(const QString & lastName);
	QString getLastName() const;

	void setSex(Contact::Sex sex);
	Contact::Sex getSex() const;

	void setBirthdate(const QDate & birthdate);
	QDate getBirthdate() const;

	void setPicture(const QString & picture);
	QString getPicture() const;

	void setWebsite(const QString & website);
	QString getWebsite() const;

	void setCompany(const QString & company);
	QString getCompany() const;

	void setWengoPhone(const QString & wengoPhone);
	QString getWengoPhone() const;

	void setMobilePhone(const QString & mobilePhone);
	QString getMobilePhone() const;

	void setHomePhone(const QString & homePhone);
	QString getHomePhone() const;

	void setWorkPhone(const QString & workPhone);
	QString getWorkPhone() const;

	void setOtherPhone(const QString & otherPhone);
	QString getOtherPhone() const;

	void setFax(const QString & fax);
	QString getFax() const;

	void setPersonalEmail(const QString & personalEmail);
	QString getPersonalEmail() const;

	void setWorkEmail(const QString & workEmail);
	QString getWorkEmail() const;

	void setOtherEmail(const QString & otherEmail);
	QString getOtherEmail() const;

	void setStreetAddress(const StreetAddress & streetAddress);
	StreetAddress getStreetAddress() const;

	void setNotes(const QString & notes);
	QString getNotes() const;

	/**
	 * ID of the Contact that is represented by this class.
	 * Initialized to unknow contact id.
	 *
	 * @see Contact
	 */
	ContactId _contactId;

	/**
	 * Contact associated with this class.
	 */
	Contact * _contact;

	/**
	 * Low-level widget of this class.
	 */
	QWidget * _contactWidget;

	QLineEdit * _firstNameLineEdit;
	QLineEdit * _lastNameLineEdit;
	QComboBox * _sexComboBox;
	QDateEdit * _birthdateEdit;
	//picture
	QLineEdit * _websiteLineEdit;
	QLineEdit * _companyLineEdit;
	QLineEdit * _wengoPhoneLineEdit;
	QLineEdit * _mobilePhoneLineEdit;
	QLineEdit * _homePhoneLineEdit;
	QLineEdit * _workPhoneLineEdit;
	QLineEdit * _otherPhoneLineEdit;
	QLineEdit * _faxLineEdit;
	QLineEdit * _personalEmailLineEdit;
	QLineEdit * _workEmailLineEdit;
	QLineEdit * _otherEmailLineEdit;

	//StreetAddress
	QLineEdit * _streetLineEdit;
	QLineEdit * _stateProvinceLineEdit;
	QLineEdit * _cityLineEdit;
	QLineEdit * _zipCodeLineEdit;
	QComboBox * _countryComboBox;

	QTextEdit * _notesTextEdit;
	static const char * REG_EXP_ALPHANUM;
};

#endif	//CONTACTWIDGET_H
