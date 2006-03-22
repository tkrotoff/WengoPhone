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

#ifndef QTCONTACTWIDGET_H
#define QTCONTACTWIDGET_H

#include <model/contactlist/Contact.h>

#include <QObject>

class QWidget;
class QLineEdit;
class QComboBox;
class QTextEdit;
class QDateTimeEdit;
class QDateTime;
class QStringList;

/**
 * Gui widget for a Contact.
 *
 * Graphical representation of a Contact: permits to the user
 * to enter the name, phone numbers, address... of a Contact.
 *
 * @see ContactWidget.ui
 * @author Tanguy Krotoff
 */
class QtContactWidget : public QObject {
public:

	/**
	 * Constructs a ContactWidget using a corresponding Contact.
	 *
	 * @param contact Contact that will be represented graphically by the ContactWidget
	 */
	QtContactWidget(Contact * contact, const QStringList & listContactGroup, QWidget * parent);

	QWidget * getWidget() const {
		return _contactWidget;
	}

	/**
	 * Creates a new Contact by setting its first name, last name ect...
	 *
	 * @param contact Contact to be completed with first name, last name...
	 */
	void createContact(Contact & contact);

private:

	void setContact(Contact * contact, const QStringList & listContactGroup);

	void setContactGroupStringList(const QStringList & listContactGroup);

	void setFirstName(const QString & firstName);
	QString getFirstName() const;

	void setLastName(const QString & lastName);
	QString getLastName() const;

	void setSex(EnumSex::Sex sex);
	EnumSex::Sex getSex() const;

	void setBirthdate(const QDateTime & birthdate);
	QDateTime getBirthdate() const;

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
	 * Low-level widget of this class.
	 */
	QWidget * _contactWidget;

	QComboBox * _contactGroupComboBox;
	QLineEdit * _firstNameLineEdit;
	QLineEdit * _lastNameLineEdit;
	QComboBox * _sexComboBox;
	QDateTimeEdit * _birthdateEdit;
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
};

#endif	//QTCONTACTWIDGET_H
