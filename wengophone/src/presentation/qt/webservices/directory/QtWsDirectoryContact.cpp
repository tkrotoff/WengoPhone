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

#include "QtWsDirectoryContact.h"

#include "ui_WsDirectoryContact.h"

#include "QtWsDirectory.h"

#include <model/contactlist/ContactProfile.h>

#include <QtGui>

QtWsDirectoryContact::QtWsDirectoryContact(QtWsDirectory * qtWsDirectory, ContactProfile * contact, bool online, QWidget * parent)
	: QObject(parent),
	_qtWsDirectory(qtWsDirectory),
	_contact(contact) {

	_wsDirectoryWidget = new QWidget(parent);

	_ui = new Ui::WsDirectoryContact();
	_ui->setupUi(_wsDirectoryWidget);

	_ui->statusLabel->setPixmap(QPixmap(":/pics/status/offline.png"));
	_ui->statusLabel->setText(QString::null);

	connect(_ui->callButton, SIGNAL(clicked()), SLOT(call()));
	connect(_ui->addContactButton, SIGNAL(clicked()), SLOT(addContact()));

	setName(QString::fromUtf8(_contact->getFirstName().c_str()) 
		+ " " + QString::fromUtf8(_contact->getLastName().c_str()));
	setNickname(QString::fromUtf8(_contact->getFirstAvailableWengoId().c_str()));
	setWengoNumber(tr("Wengo number: ") 
		+ QString::fromStdString(_contact->getWengoPhoneNumber()));
	setSipAddress(tr("SIP address: ") 
		+ QString::fromStdString(_contact->getFirstAvailableWengoId() + "@213.91.9.210"));
	setCity(tr("City: ") 
		+ QString::fromUtf8(_contact->getStreetAddress().getCity().c_str()));
	setCountry(tr("Country: ") 
		+ QString::fromUtf8(_contact->getStreetAddress().getCountry().c_str()));
	setOnline(online);
}

QtWsDirectoryContact::~QtWsDirectoryContact() {
	delete _contact;
	delete _ui;
}

void QtWsDirectoryContact::setNickname(const QString & nickname) {
	_ui->nicknameLabel->setText(nickname);
}

void QtWsDirectoryContact::setWengoNumber(const QString & number) {
	_ui->wengoNumberLabel->setText(number);
}

void QtWsDirectoryContact::setSipAddress(const QString & sipAddress) {
	_ui->sipLabel->setText(sipAddress);
}

void QtWsDirectoryContact::setName(const QString & name) {
	_ui->nameLabel->setText(name);
}

void QtWsDirectoryContact::setCity(const QString & city) {
	_ui->cityLabel->setText(city);
}

void QtWsDirectoryContact::setCountry(const QString & country) {
	_ui->countryLabel->setText(country);
}

void QtWsDirectoryContact::setOnline(bool online) {
	if (online) {
		_ui->statusLabel->setPixmap(QPixmap(":/pics/status/online.png"));
	}
}

void QtWsDirectoryContact::call() {
	_qtWsDirectory->callContact(_ui->nicknameLabel->text());
}

void QtWsDirectoryContact::addContact() {
	_qtWsDirectory->addContact(_contact);
}
