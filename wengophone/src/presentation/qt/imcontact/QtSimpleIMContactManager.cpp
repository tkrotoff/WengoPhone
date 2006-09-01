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

#include "QtSimpleIMContactManager.h"

#include "ui_SimpleIMContactManager.h"

#include "QtIMContactManager.h"

#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>

#include <model/contactlist/ContactProfile.h>

#include <util/Logger.h>

#include <QtGui/QtGui>

static const QString IMCONTACT_TEXT_SEPARATOR = ";";

QtSimpleIMContactManager::QtSimpleIMContactManager(ContactProfile & contactProfile,
	CUserProfile & cUserProfile, QWidget * parent)
	: QObject(parent),
	_contactProfile(contactProfile),
	_cUserProfile(cUserProfile) {

	_imContactManagerWidget = new QWidget(NULL);

	_ui = new Ui::SimpleIMContactManager();
	_ui->setupUi(_imContactManagerWidget);

	connect(_ui->advancedButton, SIGNAL(clicked()), SLOT(advancedClickedSlot()));

	connect(_ui->searchWengoContactButton, SIGNAL(clicked()), SLOT(searchWengoContactButtonClicked()));

	loadIMContacts();
}

QtSimpleIMContactManager::~QtSimpleIMContactManager() {
	delete _ui;
}

QWidget * QtSimpleIMContactManager::getWidget() const {
	return _imContactManagerWidget;
}

void QtSimpleIMContactManager::advancedClickedSlot() {
	advancedClicked();
}

std::set<IMContact *> QtSimpleIMContactManager::findIMContactsOfProtocol(EnumIMProtocol::IMProtocol imProtocol) const {
	std::set<IMContact *> result;

	const IMContactSet & imContactSet = _contactProfile.getIMContactSet();
	for (IMContactSet::const_iterator it = imContactSet.begin(); it != imContactSet.end(); it++) {
		if ((*it).getProtocol() == imProtocol) {
			result.insert((IMContact *)(&(*it)));
		}
	}

	return result;
}

QString QtSimpleIMContactManager::getIMContactsOfProtocol(EnumIMProtocol::IMProtocol imProtocol) const {
	QString imContacts;

	std::set<IMContact *> imContactSet = findIMContactsOfProtocol(imProtocol);
	for (std::set<IMContact *>::const_iterator it = imContactSet.begin(); it != imContactSet.end(); it++) {
		IMContact * imContact = *it;
		imContacts += QString::fromStdString(imContact->getContactId());
		imContacts += IMCONTACT_TEXT_SEPARATOR;
	}
	int index = imContacts.lastIndexOf(IMCONTACT_TEXT_SEPARATOR);
	imContacts.remove(index, 1);

	return imContacts;
}

void QtSimpleIMContactManager::loadIMContacts() {
	_ui->wengoLineEdit->setText(getIMContactsOfProtocol(EnumIMProtocol::IMProtocolWengo));
	_ui->msnLineEdit->setText(getIMContactsOfProtocol(EnumIMProtocol::IMProtocolMSN));
	_ui->aimLineEdit->setText(getIMContactsOfProtocol(EnumIMProtocol::IMProtocolAIMICQ));
	_ui->yahooLineEdit->setText(getIMContactsOfProtocol(EnumIMProtocol::IMProtocolYahoo));
	_ui->jabberLineEdit->setText(getIMContactsOfProtocol(EnumIMProtocol::IMProtocolJabber));
}

void QtSimpleIMContactManager::saveIMContacts() {
	const IMContactSet & imContactSet = _contactProfile.getIMContactSet();

	//Remove all IMContacts
	for (IMContactSet::const_iterator it = imContactSet.begin(); it != imContactSet.end(); it++) {
		IMContact * imContact = (IMContact *) &(*it);

		//Bypass SIP and Jabber protocols
		if (imContact->getProtocol() == EnumIMProtocol::IMProtocolSIPSIMPLE ||
			imContact->getProtocol() == EnumIMProtocol::IMProtocolJabber) {

			continue;
		}

		_contactProfile.removeIMContact(*imContact);
	}

	//Save all IMContacts
	addIMContact(EnumIMProtocol::IMProtocolWengo, _ui->wengoLineEdit->text());
	addIMContact(EnumIMProtocol::IMProtocolMSN, _ui->msnLineEdit->text());
	addIMContact(EnumIMProtocol::IMProtocolAIMICQ, _ui->aimLineEdit->text());
	addIMContact(EnumIMProtocol::IMProtocolYahoo, _ui->yahooLineEdit->text());
	addIMContact(EnumIMProtocol::IMProtocolJabber, _ui->jabberLineEdit->text());
}

void QtSimpleIMContactManager::addIMContact(EnumIMProtocol::IMProtocol imProtocol, const QString & text) {
	if (text.isEmpty()) {
		return;
	}

	QStringList contactIdList = text.split(IMCONTACT_TEXT_SEPARATOR);
	for (int i = 0; i < contactIdList.size(); i++) {
		IMContact imContact(imProtocol, contactIdList.at(i).toStdString());

		//IMAccount to associate with the IMContact
		std::set<IMAccount *> imAccounts = _cUserProfile.getIMAccountsOfProtocol(imProtocol);
		std::set<IMAccount *>::const_iterator it = imAccounts.begin();
		if (it != imAccounts.end()) {
			imContact.setIMAccount(*it);
		}

		_contactProfile.addIMContact(imContact);
	}
}

void QtSimpleIMContactManager::searchWengoContactButtonClicked() {
}
