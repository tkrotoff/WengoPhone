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
#include "QtWsDirectory.h"

#include <model/profile/Profile.h>

#include <qtutil/MouseEventFilter.h>

#include <QtGui>

QtWsDirectoryContact::QtWsDirectoryContact(QtWsDirectory * qtWsDirectory, Profile * profile, bool online, QWidget * parent) 
	: QWidget(parent), _qtWsDirectory(qtWsDirectory), _profile(profile) {

	ui.setupUi(this);
	ui.statusLabel->setPixmap(QPixmap(":/pics/status/offline.png"));
	ui.statusLabel->setText("");
	
	MousePressEventFilter * mouseFilter1 = new MousePressEventFilter(
		this, SLOT(callLabelClicked()), Qt::LeftButton);
	ui.callLabel->installEventFilter(mouseFilter1);
	
	MousePressEventFilter * mouseFilter2 = new MousePressEventFilter(
			this, SLOT(addLabelClicked()), Qt::LeftButton);
	ui.addLabel->installEventFilter(mouseFilter2);

	setName(QString::fromUtf8(profile->getFirstName().c_str()) + " " + QString::fromUtf8(profile->getLastName().c_str()));
	setNickname(tr("Nickname: ") + QString::fromUtf8(profile->getWengoPhoneId().c_str()));
	setWengoNumber(tr("Wengo number: ") + QString::fromStdString(profile->getWengoPhoneNumber()));
	setSipAddress(tr("Sip address: ") + QString::fromStdString(profile->getWengoPhoneId() + "@213.91.9.210"));
	setLocality(tr("City: ") + QString::fromUtf8(profile->getStreetAddress().getCity().c_str()));
	setCountry(tr("Country: ") + QString::fromUtf8(profile->getStreetAddress().getCountry().c_str()));
	setOnline(online);
}

QtWsDirectoryContact::~QtWsDirectoryContact() {
	delete _profile;
}

void QtWsDirectoryContact::setNickname(const QString & nickname) {
	ui.nicknameLabel->setText(nickname);
}

void QtWsDirectoryContact::setWengoNumber(const QString & number) {
	ui.wengoNumberLabel->setText(number);
}

void QtWsDirectoryContact::setSipAddress(const QString & sipAddress) {
	ui.sipLabel->setText(sipAddress);
}

void QtWsDirectoryContact::setName(const QString & name) {
	ui.nameLabel->setText(name);
}

void QtWsDirectoryContact::setLocality(const QString & locality) {
	ui.localityLabel->setText(locality);
}

void QtWsDirectoryContact::setCountry(const QString & country) {
	ui.countryLabel->setText(country);
}

void QtWsDirectoryContact::setOnline(bool online) {
	if( online ) {
		ui.statusLabel->setPixmap(QPixmap(":/pics/status/online.png"));
	}
}

void QtWsDirectoryContact::callLabelClicked() {
	_qtWsDirectory->callContact(ui.nicknameLabel->text());
}

void QtWsDirectoryContact::addLabelClicked() {
	_qtWsDirectory->addContact(_profile);
}
