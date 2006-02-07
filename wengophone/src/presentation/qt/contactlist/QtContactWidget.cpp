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

#include "QtContactWidget.h"

#include <model/WengoPhone.h>
#include <model/presence/PresenceHandler.h>

#include <IMAccountHandler.h>
#include <IMContact.h>
#include <WidgetFactory.h>
#include <Object.h>

#include <QtGui>

QtContactWidget::QtContactWidget(Contact * contact, const QStringList & listContactGroup, QWidget * parent) : QObject(parent) {
	_contactWidget = WidgetFactory::create(":/forms/contactlist/ContactWidget.ui", parent);

	_contactGroupComboBox = Object::findChild<QComboBox *>(_contactWidget, "contactGroupComboBox");
	_firstNameLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "firstNameLineEdit");
	_lastNameLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "lastNameLineEdit");
	_sexComboBox = Object::findChild<QComboBox *>(_contactWidget, "sexComboBox");
	_birthdateEdit = Object::findChild<QDateTimeEdit *>(_contactWidget, "birthdateEdit");
	//picture
	_websiteLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "websiteLineEdit");
	_companyLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "companyLineEdit");
	_wengoPhoneLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "wengoPhoneLineEdit");
	_mobilePhoneLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "mobilePhoneLineEdit");
	_homePhoneLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "homePhoneLineEdit");
	_workPhoneLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "workPhoneLineEdit");
	_otherPhoneLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "otherPhoneLineEdit");
	_faxLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "faxLineEdit");
	_personalEmailLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "personalEmailLineEdit");
	_workEmailLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "workEmailLineEdit");
	_otherEmailLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "otherEmailLineEdit");

	//StreetAddress
	_streetLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "streetLineEdit");
	_stateProvinceLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "stateProvinceLineEdit");
	_cityLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "cityLineEdit");
	_zipCodeLineEdit = Object::findChild<QLineEdit *>(_contactWidget, "zipCodeLineEdit");
	_countryComboBox = Object::findChild<QComboBox *>(_contactWidget, "countryComboBox");

	_notesTextEdit = Object::findChild<QTextEdit *>(_contactWidget, "notesTextEdit");

	setContact(contact, listContactGroup);
}

void QtContactWidget::setContact(Contact * contact, const QStringList & listContactGroup) {
	setContactGroupStringList(listContactGroup);

	if (!contact) {
		return;
	}

	setFirstName(QString::fromStdString(contact->getFirstName()));
	setLastName(QString::fromStdString(contact->getLastName()));
	setSex(contact->getSex());

	Date date = contact->getBirthdate();
	setBirthdate(QDateTime(QDate(date.getYear(), date.getMonth(), date.getDay())));

	setPicture(QString::fromStdString(contact->getPicture()));
	setWebsite(QString::fromStdString(contact->getWebsite()));
	setCompany(QString::fromStdString(contact->getCompany()));
	setWengoPhone(QString::fromStdString(contact->getWengoPhoneId()));
	setMobilePhone(QString::fromStdString(contact->getMobilePhone()));
	setHomePhone(QString::fromStdString(contact->getHomePhone()));
	setWorkPhone(QString::fromStdString(contact->getWorkPhone()));
	setOtherPhone(QString::fromStdString(contact->getOtherPhone()));
	setFax(QString::fromStdString(contact->getFax()));
	setPersonalEmail(QString::fromStdString(contact->getPersonalEmail()));
	setWorkEmail(QString::fromStdString(contact->getWorkEmail()));
	setOtherEmail(QString::fromStdString(contact->getOtherEmail()));
	setStreetAddress(contact->getStreetAddress());
	setNotes(QString::fromStdString(contact->getNotes()));

	//TODO: set data from IMContact
}

void QtContactWidget::setContactGroupStringList(const QStringList & listContactGroup) {
	_contactGroupComboBox->insertItems(0, listContactGroup);
}

QString QtContactWidget::createContact(Contact * contact) {
	if (!contact) {
		return QString::null;
	}

	contact->setFirstName(getFirstName().toStdString());
	contact->setLastName(getLastName().toStdString());
	contact->setSex(getSex());
	
	QDateTime dateTime = getBirthdate();
	contact->setBirthdate(Date(dateTime.date().day(), dateTime.date().month(), dateTime.date().year()));
	
	contact->setPicture(getPicture().toStdString());
	contact->setWebsite(getWebsite().toStdString());
	contact->setCompany(getCompany().toStdString());
	contact->setWengoPhoneId(getWengoPhone().toStdString());
	contact->setMobilePhone(getMobilePhone().toStdString());
	contact->setHomePhone(getHomePhone().toStdString());
	contact->setWorkPhone(getWorkPhone().toStdString());
	contact->setOtherPhone(getOtherPhone().toStdString());
	contact->setFax(getFax().toStdString());
	contact->setPersonalEmail(getPersonalEmail().toStdString());
	contact->setWorkEmail(getWorkEmail().toStdString());
	contact->setOtherEmail(getOtherEmail().toStdString());
	contact->setStreetAddress(getStreetAddress());
	contact->setNotes(getNotes().toStdString());

	if (!getWengoPhone().isEmpty()) {
		List<IMAccount *> list = contact->getWengoPhone().getIMAccountHandler().getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolSIPSIMPLE);
		if (list.size() > 0) {
			contact->addIMContact(IMContact(*list[0],
				getWengoPhone().toStdString()));
		}
	}

	return _contactGroupComboBox->currentText();
}

void QtContactWidget::setFirstName(const QString & firstName) {
	_firstNameLineEdit->setText(firstName);
}

QString QtContactWidget::getFirstName() const {
	return _firstNameLineEdit->text();
}

void QtContactWidget::setLastName(const QString & lastName) {
	_lastNameLineEdit->setText(lastName);
}

QString QtContactWidget::getLastName() const {
	return _lastNameLineEdit->text();
}

void QtContactWidget::setSex(Contact::Sex sex) {
	_sexComboBox->setCurrentIndex(sex);
}

Contact::Sex QtContactWidget::getSex() const {
	return (Contact::Sex) _sexComboBox->currentIndex();
}

void QtContactWidget::setBirthdate(const QDateTime & birthdate) {
	_birthdateEdit->setDateTime(birthdate);
}

QDateTime QtContactWidget::getBirthdate() const {
	return _birthdateEdit->dateTime();
}

void QtContactWidget::setPicture(const QString & /*picture*/) {
}

QString QtContactWidget::getPicture() const {
	return QString();
}

void QtContactWidget::setWebsite(const QString & website) {
	_websiteLineEdit->setText(website);
}

QString QtContactWidget::getWebsite() const {
	return _websiteLineEdit->text();
}

void QtContactWidget::setCompany(const QString & company) {
	_companyLineEdit->setText(company);
}

QString QtContactWidget::getCompany() const {
	return _companyLineEdit->text();
}

void QtContactWidget::setWengoPhone(const QString & wengoPhone) {
	_wengoPhoneLineEdit->setText(wengoPhone);
}

QString QtContactWidget::getWengoPhone() const {
	return _wengoPhoneLineEdit->text();
}

void QtContactWidget::setMobilePhone(const QString & mobilePhone) {
	_mobilePhoneLineEdit->setText(mobilePhone);
}

QString QtContactWidget::getMobilePhone() const {
	return _mobilePhoneLineEdit->text();
}

void QtContactWidget::setHomePhone(const QString & homePhone) {
	_homePhoneLineEdit->setText(homePhone);
}

QString QtContactWidget::getHomePhone() const {
	return _homePhoneLineEdit->text();
}

void QtContactWidget::setWorkPhone(const QString & workPhone) {
	_workPhoneLineEdit->setText(workPhone);
}

QString QtContactWidget::getWorkPhone() const {
	return _workPhoneLineEdit->text();
}

void QtContactWidget::setOtherPhone(const QString & otherPhone) {
	_otherPhoneLineEdit->setText(otherPhone);
}

QString QtContactWidget::getOtherPhone() const {
	return _otherPhoneLineEdit->text();
}

void QtContactWidget::setFax(const QString & fax) {
	_faxLineEdit->setText(fax);
}

QString QtContactWidget::getFax() const {
	return _faxLineEdit->text();
}

void QtContactWidget::setPersonalEmail(const QString & personalEmail) {
	_personalEmailLineEdit->setText(personalEmail);
}

QString QtContactWidget::getPersonalEmail() const {
	return _personalEmailLineEdit->text();
}

void QtContactWidget::setWorkEmail(const QString & workEmail) {
	_workEmailLineEdit->setText(workEmail);
}

QString QtContactWidget::getWorkEmail() const {
	return _workEmailLineEdit->text();
}

void QtContactWidget::setOtherEmail(const QString & otherEmail) {
	_otherEmailLineEdit->setText(otherEmail);
}

QString QtContactWidget::getOtherEmail() const {
	return _otherEmailLineEdit->text();
}

void QtContactWidget::setStreetAddress(const StreetAddress & streetAddress) {
	_streetLineEdit->setText(QString::fromStdString(streetAddress.getStreet()));
	_stateProvinceLineEdit->setText(QString::fromStdString(streetAddress.getStateProvince()));
	_cityLineEdit->setText(QString::fromStdString(streetAddress.getCity()));
	_zipCodeLineEdit->setText(QString::fromStdString(streetAddress.getZipCode()));
	_countryComboBox->setCurrentIndex(
		_countryComboBox->findText(QString::fromStdString(streetAddress.getCountry())));
}

StreetAddress QtContactWidget::getStreetAddress() const {
	StreetAddress streetAddress;
	streetAddress.setStreet(_streetLineEdit->text().toStdString());
	streetAddress.setStateProvince(_stateProvinceLineEdit->text().toStdString());
	streetAddress.setCity(_cityLineEdit->text().toStdString());
	streetAddress.setZipCode(_zipCodeLineEdit->text().toStdString());
	streetAddress.setCountry(_countryComboBox->currentText().toStdString());
	return streetAddress;
}

void QtContactWidget::setNotes(const QString & notes) {
	_notesTextEdit->setPlainText(notes);
}

QString QtContactWidget::getNotes() const {
	return _notesTextEdit->toPlainText();
}
