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

#include "ContactWidget.h"

#include "ContactList.h"
#include "MyWidgetFactory.h"

#include <qlineedit.h>
#include <qwidgetstack.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qtextedit.h>
#include <qregexp.h>
#include <qvalidator.h>

#include <cassert>
#include <iostream>
using namespace std;

const char * ContactWidget::REG_EXP_ALPHANUM = "([0-9]|[a-z]|[A-Z]|[@]|[.]|[-]|[_])*";

ContactWidget::ContactWidget(QWidget * parent) : QObject(parent) {
	_contact = NULL;
	_contactWidget = MyWidgetFactory::create("ContactWidgetForm.ui", this, parent);
	
	_firstNameLineEdit = (QLineEdit *) _contactWidget->child("firstNameLineEdit", "QLineEdit");
	_lastNameLineEdit = (QLineEdit *) _contactWidget->child("lastNameLineEdit", "QLineEdit");
	_sexComboBox = (QComboBox *) _contactWidget->child("sexComboBox", "QComboBox");
	_birthdateEdit = (QDateEdit *) _contactWidget->child("birthdateEdit", "QDateEdit");
	//picture
	_websiteLineEdit = (QLineEdit *) _contactWidget->child("websiteLineEdit", "QLineEdit");
	_companyLineEdit = (QLineEdit *) _contactWidget->child("companyLineEdit", "QLineEdit");
	_wengoPhoneLineEdit = (QLineEdit *) _contactWidget->child("wengoPhoneLineEdit", "QLineEdit");
	_mobilePhoneLineEdit = (QLineEdit *) _contactWidget->child("mobilePhoneLineEdit", "QLineEdit");
	_homePhoneLineEdit = (QLineEdit *) _contactWidget->child("homePhoneLineEdit", "QLineEdit");
	_workPhoneLineEdit = (QLineEdit *) _contactWidget->child("workPhoneLineEdit", "QLineEdit");
	_otherPhoneLineEdit = (QLineEdit *) _contactWidget->child("otherPhoneLineEdit", "QLineEdit");
	_faxLineEdit = (QLineEdit *) _contactWidget->child("faxLineEdit", "QLineEdit");
	_personalEmailLineEdit = (QLineEdit *) _contactWidget->child("personalEmailLineEdit", "QLineEdit");
	_workEmailLineEdit = (QLineEdit *) _contactWidget->child("workEmailLineEdit", "QLineEdit");
	_otherEmailLineEdit = (QLineEdit *) _contactWidget->child("otherEmailLineEdit", "QLineEdit");

	//StreetAddress
	_streetLineEdit = (QLineEdit *) _contactWidget->child("streetLineEdit", "QLineEdit");
	_stateProvinceLineEdit = (QLineEdit *) _contactWidget->child("stateProvinceLineEdit", "QLineEdit");
	_cityLineEdit = (QLineEdit *) _contactWidget->child("cityLineEdit", "QLineEdit");
	_zipCodeLineEdit = (QLineEdit *) _contactWidget->child("zipCodeLineEdit", "QLineEdit");
	_countryComboBox = (QComboBox *) _contactWidget->child("countryComboBox", "QComboBox");

	_notesTextEdit = (QTextEdit *) _contactWidget->child("notesTextEdit", "QTextEdit");

	QRegExp regexp(REG_EXP_ALPHANUM);
	QRegExpValidator * validator = new QRegExpValidator(regexp, _contactWidget);
	_wengoPhoneLineEdit->setValidator(validator);
	_wengoPhoneLineEdit->setFocus();
}

void ContactWidget::setContact(Contact & contact) {
	_contact = &contact;
	setId(contact.getId());
	setFirstName(contact.getFirstName());
	setLastName(contact.getLastName());
	setSex(contact.getSex());
	setBirthdate(contact.getBirthdate());
	setPicture(contact.getPicture());
	setWebsite(contact.getWebsite());
	setCompany(contact.getCompany());
	setWengoPhone(contact.getWengoPhone());
	setMobilePhone(contact.getMobilePhone());
	setHomePhone(contact.getHomePhone());
	setWorkPhone(contact.getWorkPhone());
	setOtherPhone(contact.getOtherPhone());
	setFax(contact.getFax());
	setPersonalEmail(contact.getPersonalEmail());
	setWorkEmail(contact.getWorkEmail());
	setOtherEmail(contact.getOtherEmail());
	setStreetAddress(contact.getStreetAddress());
	setNotes(contact.getNotes());
}

ContactWidget::~ContactWidget() {
}

void ContactWidget::reset() {
	static const Contact empty;

	setFirstName(empty.getFirstName());
	setLastName(empty.getLastName());
	setSex(empty.getSex());
	setBirthdate(empty.getBirthdate());
	setPicture(empty.getPicture());
	setWebsite(empty.getWebsite());
	setCompany(empty.getCompany());
	setWengoPhone(empty.getWengoPhone());
	setMobilePhone(empty.getMobilePhone());
	setHomePhone(empty.getHomePhone());
	setWorkPhone(empty.getWorkPhone());
	setOtherPhone(empty.getOtherPhone());
	setFax(empty.getFax());
	setPersonalEmail(empty.getPersonalEmail());
	setWorkEmail(empty.getWorkEmail());
	setOtherEmail(empty.getOtherEmail());
	setStreetAddress(empty.getStreetAddress());
	setNotes(empty.getNotes());
}

void ContactWidget::updateContact() {
	if (_contact == NULL) {
		_contact = new Contact();
	}

	_contact->setFirstName(getFirstName());
	_contact->setLastName(getLastName());
	_contact->setSex(getSex());
	_contact->setBirthdate(getBirthdate());
	_contact->setPicture(getPicture());
	_contact->setWebsite(getWebsite());
	_contact->setCompany(getCompany());
	_contact->setWengoPhone(getWengoPhone());
	_contact->setMobilePhone(getMobilePhone());
	_contact->setHomePhone(getHomePhone());
	_contact->setWorkPhone(getWorkPhone());
	_contact->setOtherPhone(getOtherPhone());
	_contact->setFax(getFax());
	_contact->setPersonalEmail(getPersonalEmail());
	_contact->setWorkEmail(getWorkEmail());
	_contact->setOtherEmail(getOtherEmail());
	_contact->setStreetAddress(getStreetAddress());
	_contact->setNotes(getNotes());

	ContactList & contactList = ContactList::getInstance();
	if (!contactList.contains(*_contact)) {
		contactList.addContact(*_contact);
	}

	_contact->save();
}

void ContactWidget::setFirstName(const QString & firstName) {
	_firstNameLineEdit->setText(firstName);
}

QString ContactWidget::getFirstName() const {
	return _firstNameLineEdit->text();
}

void ContactWidget::setLastName(const QString & lastName) {
	_lastNameLineEdit->setText(lastName);
}

QString ContactWidget::getLastName() const {
	return _lastNameLineEdit->text();
}

void ContactWidget::setSex(Contact::Sex sex) {
	_sexComboBox->setCurrentItem(sex);
}

Contact::Sex ContactWidget::getSex() const {
	return (Contact::Sex) _sexComboBox->currentItem();
}

void ContactWidget::setBirthdate(const QDate & birthdate) {
	_birthdateEdit->setDate(birthdate);
}

QDate ContactWidget::getBirthdate() const {
	return _birthdateEdit->date();
}

void ContactWidget::setPicture(const QString & /*picture*/) {
}

QString ContactWidget::getPicture() const {
	return QString();
}

void ContactWidget::setWebsite(const QString & website) {
	_websiteLineEdit->setText(website);
}

QString ContactWidget::getWebsite() const {
	return _websiteLineEdit->text();
}

void ContactWidget::setCompany(const QString & company) {
	_companyLineEdit->setText(company);
}

QString ContactWidget::getCompany() const {
	return _companyLineEdit->text();
}

void ContactWidget::setWengoPhone(const QString & wengoPhone) {
	_wengoPhoneLineEdit->setText(wengoPhone);
}

QString ContactWidget::getWengoPhone() const {
	return _wengoPhoneLineEdit->text();
}

void ContactWidget::setMobilePhone(const QString & mobilePhone) {
	_mobilePhoneLineEdit->setText(mobilePhone);
}

QString ContactWidget::getMobilePhone() const {
	return _mobilePhoneLineEdit->text();
}

void ContactWidget::setHomePhone(const QString & homePhone) {
	_homePhoneLineEdit->setText(homePhone);
}

QString ContactWidget::getHomePhone() const {
	return _homePhoneLineEdit->text();
}

void ContactWidget::setWorkPhone(const QString & workPhone) {
	_workPhoneLineEdit->setText(workPhone);
}

QString ContactWidget::getWorkPhone() const {
	return _workPhoneLineEdit->text();
}

void ContactWidget::setOtherPhone(const QString & otherPhone) {
	_otherPhoneLineEdit->setText(otherPhone);
}

QString ContactWidget::getOtherPhone() const {
	return _otherPhoneLineEdit->text();
}

void ContactWidget::setFax(const QString & fax) {
	_faxLineEdit->setText(fax);
}

QString ContactWidget::getFax() const {
	return _faxLineEdit->text();
}

void ContactWidget::setPersonalEmail(const QString & personalEmail) {
	_personalEmailLineEdit->setText(personalEmail);
}

QString ContactWidget::getPersonalEmail() const {
	return _personalEmailLineEdit->text();
}

void ContactWidget::setWorkEmail(const QString & workEmail) {
	_workEmailLineEdit->setText(workEmail);
}

QString ContactWidget::getWorkEmail() const {
	return _workEmailLineEdit->text();
}

void ContactWidget::setOtherEmail(const QString & otherEmail) {
	_otherEmailLineEdit->setText(otherEmail);
}

QString ContactWidget::getOtherEmail() const {
	return _otherEmailLineEdit->text();
}

void ContactWidget::setStreetAddress(const StreetAddress & streetAddress) {
	_streetLineEdit->setText(streetAddress.getStreet());
	_stateProvinceLineEdit->setText(streetAddress.getStateProvince());
	_cityLineEdit->setText(streetAddress.getCity());
	_zipCodeLineEdit->setText(streetAddress.getZipCode());
	_countryComboBox->setCurrentText(streetAddress.getCountry());
}

StreetAddress ContactWidget::getStreetAddress() const {
	StreetAddress streetAddress;
	streetAddress.setStreet(_streetLineEdit->text());
	streetAddress.setStateProvince(_stateProvinceLineEdit->text());
	streetAddress.setCity(_cityLineEdit->text());
	streetAddress.setZipCode(_zipCodeLineEdit->text());
	streetAddress.setCountry(_countryComboBox->currentText());
	return streetAddress;
}

void ContactWidget::setNotes(const QString & notes) {
	_notesTextEdit->setText(notes);
}

QString ContactWidget::getNotes() const {
	return _notesTextEdit->text();
}
