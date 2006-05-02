/*
* WengoPhone, a voice over Internet phone
* Copyright (C) 2004-2006  Wengo
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

#include <control/contactlist/CContact.h>
#include <control/contactlist/CContactList.h>
#include <control/CWengoPhone.h>

#include <model/contactlist/Contact.h>
#include <model/profile/StreetAddress.h>
#include <model/profile/UserProfile.h>

#include <imwrapper/IMAccountHandler.h>

#include <util/Logger.h>
#include <util/Date.h>

#include <qtutil/StringListConvert.h>

#include <QtGui>

#include "QtEditContactProfile.h"
#include "QtIMContactDetails.h"
#include "QtAddIMContact.h"

#include "ui_ContactDetails.h"

using namespace std;

QtEditContactProfile::QtEditContactProfile(QtEditContactProfile::EditMode mode,Contact & contact, CWengoPhone & cWengoPhone,QWidget * parent)
	: QDialog(parent), _mode(mode), _cWengoPhone(cWengoPhone), _contact(contact) {

	_ui = new Ui::ContactDetails();
	_ui->setupUi(this);

	_addIMContactMenu = new QMenu(_ui->addIMContactButton);
	QAction * actionMSN = _addIMContactMenu->addAction(QIcon(":pics/protocol_msn.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolMSN)));
	QAction * actionAIMICQ = _addIMContactMenu->addAction(QIcon(":pics/protocol_aim.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolAIMICQ)));
	QAction * actionYahoo = _addIMContactMenu->addAction(QIcon(":pics/protocol_yahoo.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolYahoo)));
	QAction * actionJabber = _addIMContactMenu->addAction(QIcon(":pics/protocol_jabber.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolJabber)));

	connect(actionMSN, SIGNAL(triggered()), this, SLOT(actionClickedMSN()));
	connect(actionAIMICQ, SIGNAL(triggered()), this, SLOT(actionClickedAIMICQ()));
	connect(actionYahoo, SIGNAL(triggered()), this, SLOT(actionClickedYahoo()));
	connect(actionJabber, SIGNAL(triggered()), this, SLOT(actionClickedJabber()));

	connect(_ui->addIMContactButton, SIGNAL(clicked()), this, SLOT(addIMContactButtonClicked()));
	connect(_ui->saveButton, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));
	connect(_ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));

	QStringList tmp = StringListConvert::toQStringList(_cWengoPhone.getCContactList().getContactGroupStringList());
    _ui->groupComboBox->insertItems(0, tmp);
	readFromConfig();
}

void QtEditContactProfile::saveButtonClicked() {
	writeToConfig();
	accept();
}

void QtEditContactProfile::cancelButtonClicked() {
	reject();
}

void QtEditContactProfile::writeToConfig() {
	// Contact & c = _cContact.getContact();

	// Setting wengo id
	_contact.setWengoPhoneId(_ui->alias->text().toStdString());
	////

	// Setting name
	_contact.setFirstName(_ui->firstName->text().toStdString());
	_contact.setLastName(_ui->lastName->text().toStdString());
	////

	// Setting sex
	_contact.setSex((EnumSex::Sex) _ui->gender->currentIndex());
	////

	// Setting birthday
	QDate qdate = _ui->birthDate->date();
	Date date;
	date.setDay(qdate.day());
	date.setMonth(qdate.month());
	date.setYear(qdate.year());
	_contact.setBirthdate(date);
	////

	// Setting address
	StreetAddress address;
	//address.setStateProvince(_state->text().toStdString());
	//address.setCountry(_country->text().toStdString());
	address.setCity(_ui->city->text().toStdString());
	_contact.setStreetAddress(address);
	////

	// Setting IMAccounts
	////

	// Setting phone numbers
	_contact.setMobilePhone(_ui->cellPhone->text().toStdString());
	_contact.setWorkPhone(_ui->workPhone->text().toStdString());
	_contact.setHomePhone(_ui->homePhone->text().toStdString());
	_contact.setWengoPhoneNumber(_ui->wengoPhone->text().toStdString());
	////

	// Setting emails
	_contact.setPersonalEmail(_ui->email->text().toStdString());
	////

	// Settings websites
	_contact.setWebsite(_ui->web->text().toStdString());
	////

	if ( _mode == ModeCreate ){
		_contact.addToContactGroup( _ui->groupComboBox->currentText().toStdString());
	}
}

void QtEditContactProfile::readFromConfig() {
	// Contact & c = _cContact.getContact();

	// Setting wengo id
	_ui->alias->setText(QString::fromUtf8(_contact.getWengoPhoneId().c_str()));
	////

	// Setting name
	_ui->firstName->setText(QString::fromUtf8(_contact.getFirstName().c_str()));
	_ui->lastName->setText(QString::fromUtf8(_contact.getLastName().c_str()));
	////

	// Setting sex
	_ui->gender->setCurrentIndex((int) _contact.getSex());
	////

	// Setting birthday
	Date date = _contact.getBirthdate();
	_ui->birthDate->setDate(QDate(date.getYear(), date.getMonth(), date.getDay()));
	////

	// Setting address
	StreetAddress address = _contact.getStreetAddress();
	//_ui->state->setText(QString::fromStdString(address.getStateProvince()));
	//_ui->country->setText(QString::fromStdString(address.getCountry()));
	_ui->city->setText(QString::fromUtf8(address.getCity().c_str()));
	////

	// Setting IMAccounts
	unsigned j = 0;
	for (IMContactSet::const_iterator it = _contact.getIMContactSet().begin();
		it != _contact.getIMContactSet().end();
		++it, ++j) {

		QtIMContactDetails * qtIMContactDetails = new QtIMContactDetails((IMContact &)*it, _ui->IMAccountGroup);
		addIMContactDetails(qtIMContactDetails);
	}
	////

	// Setting phone numbers
	_ui->cellPhone->setText(QString::fromStdString(_contact.getMobilePhone()));
	_ui->workPhone->setText(QString::fromStdString(_contact.getWorkPhone()));
	_ui->homePhone->setText(QString::fromStdString(_contact.getHomePhone()));
	_ui->wengoPhone->setText(QString::fromStdString(_contact.getWengoPhoneNumber()));
	////

	// Setting emails
	_ui->email->setText(QString::fromStdString(_contact.getPersonalEmail()));
	////

	// Settings websites
	_ui->web->setText(QString::fromStdString(_contact.getWebsite()));
	////

	// Setting icon
	QPixmap pixmap;
	string myData = _contact.getIcon().getData();
	pixmap.loadFromData((uchar *)myData.c_str(), myData.size());
	_ui->avatarPixmap->setPixmap(pixmap.scaled(_ui->avatarPixmap->rect().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	////
}

void QtEditContactProfile::addIMContactDetails(QtIMContactDetails * qtIMContactDetails) {
	QLayout * layout =  _ui->IMAccountGroup->layout();

	if (layout) {
		layout->addWidget(qtIMContactDetails);

		connect(qtIMContactDetails, SIGNAL(removeButtonClicked(QtIMContactDetails *)),
			this, SLOT(removeButtonClicked(QtIMContactDetails *)));
	}
}

void QtEditContactProfile::removeIMContactDetails(QtIMContactDetails * qtIMContactDetails) {
	QLayout * layout =  _ui->IMAccountGroup->layout();

	if (layout) {
		_contact.removeIMContact(qtIMContactDetails->getIMContact());
		layout->removeWidget(qtIMContactDetails);
		delete qtIMContactDetails;
	}
}

void QtEditContactProfile::addIMContact(EnumIMProtocol::IMProtocol imProtocol) {
	/*
	QString protocolName = action->text();
	EnumIMProtocol::IMProtocol imProtocol = EnumIMProtocol::toIMProtocol(protocolName.toStdString());
	*/

	std::set<IMAccount *> imAccounts = _cWengoPhone.getCurrentUserProfile().getIMAccountHandler().getIMAccountsOfProtocol(imProtocol);

	QtAddIMContact qtAddIMContact(imProtocol, imAccounts, this);

	if (qtAddIMContact.exec()) {
		IMContact imContact(imProtocol, qtAddIMContact.contactId().toStdString());
		for (std::set<IMAccount *>::const_iterator it = imAccounts.begin();
			it != imAccounts.end();
			++it) {
			imContact.setIMAccount(*it);
			_contact.addIMContact(imContact);
		}
		readFromConfig();
	}
}

void QtEditContactProfile::removeButtonClicked(QtIMContactDetails * qtIMContactDetails) {
	removeIMContactDetails(qtIMContactDetails);
}

void QtEditContactProfile::addIMContactButtonClicked() {
	_addIMContactMenu->popup(_ui->addIMContactButton->mapToGlobal(_ui->addIMContactButton->pos()));
}

void QtEditContactProfile::createContact(){
    Contact & contact = _cWengoPhone.getWengoPhone().getCurrentUserProfile().getContactList().createContact();
}
