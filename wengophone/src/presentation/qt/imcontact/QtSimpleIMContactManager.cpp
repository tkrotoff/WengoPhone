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

#include <presentation/qt/QtWengoPhone.h>

#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>
#include <control/CWengoPhone.h>

#include <model/contactlist/ContactProfile.h>

#include <imwrapper/IMAccount.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <qtutil/SafeConnect.h>

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

	SAFE_CONNECT(_ui->searchWengoContactButton, SIGNAL(clicked()), SLOT(searchWengoContactButtonClicked()));

	loadIMContacts();
}

QtSimpleIMContactManager::~QtSimpleIMContactManager() {
	OWSAFE_DELETE(_ui);
}

QWidget * QtSimpleIMContactManager::getWidget() const {
	return _imContactManagerWidget;
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

	//Remove the last IMCONTACT_TEXT_SEPARATOR
	int index = imContacts.lastIndexOf(IMCONTACT_TEXT_SEPARATOR);
	imContacts.remove(index, 1);

	return imContacts;
}

void QtSimpleIMContactManager::changeIMProtocolPixmaps(EnumIMProtocol::IMProtocol imProtocol,
			QLabel * imProtocolLabel, const char * connectedPixmap, QLineEdit * imProtocolLineEdit) {

	std::set<IMAccount *> imAccounts = _cUserProfile.getIMAccountsOfProtocol(imProtocol);
	if (!imAccounts.empty()) {
		imProtocolLineEdit->setEnabled(true);
		imProtocolLabel->setPixmap(QPixmap(connectedPixmap));
	}
}

void QtSimpleIMContactManager::loadIMContacts() {
	//Update presentation depending if IM accounts exist
	changeIMProtocolPixmaps(EnumIMProtocol::IMProtocolWengo,
		_ui->wengoLabel, ":pics/protocols/wengo.png", _ui->wengoLineEdit);
	changeIMProtocolPixmaps(EnumIMProtocol::IMProtocolMSN,
		_ui->msnLabel, ":pics/protocols/msn.png", _ui->msnLineEdit);
	changeIMProtocolPixmaps(EnumIMProtocol::IMProtocolAIMICQ,
		_ui->aimLabel, ":pics/protocols/aim.png", _ui->aimLineEdit);
	changeIMProtocolPixmaps(EnumIMProtocol::IMProtocolYahoo,
		_ui->yahooLabel, ":pics/protocols/yahoo.png", _ui->yahooLineEdit);
	changeIMProtocolPixmaps(EnumIMProtocol::IMProtocolJabber,
		_ui->jabberLabel, ":pics/protocols/jabber.png", _ui->jabberLineEdit);

	//Loads all the IM contacts
	if (_ui->wengoLineEdit->isEnabled()) {
		_ui->wengoLineEdit->setText(getIMContactsOfProtocol(EnumIMProtocol::IMProtocolWengo));
	}
	if (_ui->msnLineEdit->isEnabled()) {
		_ui->msnLineEdit->setText(getIMContactsOfProtocol(EnumIMProtocol::IMProtocolMSN));
	}
	if (_ui->aimLineEdit->isEnabled()) {
		_ui->aimLineEdit->setText(getIMContactsOfProtocol(EnumIMProtocol::IMProtocolAIMICQ));
	}
	if (_ui->yahooLineEdit->isEnabled()) {
		_ui->yahooLineEdit->setText(getIMContactsOfProtocol(EnumIMProtocol::IMProtocolYahoo));
	}
	if (_ui->jabberLineEdit->isEnabled()) {
		_ui->jabberLineEdit->setText(getIMContactsOfProtocol(EnumIMProtocol::IMProtocolJabber));
	}
}

void QtSimpleIMContactManager::addIMContactsOfProtocol(EnumIMProtocol::IMProtocol imProtocol, const QString & text) {
	QString imContactsBefore = getIMContactsOfProtocol(imProtocol);
	QStringList contactIdListBefore = imContactsBefore.split(IMCONTACT_TEXT_SEPARATOR);
	QString imContactsAfter = text;
	QStringList contactIdListAfter = imContactsAfter.split(IMCONTACT_TEXT_SEPARATOR);

	//Remove IMContacts
	for (int i = 0; i < contactIdListBefore.size(); i++) {
		QString contactId = contactIdListBefore[i];

		if (imContactsAfter.contains(contactId)) {
			//Do nothing
		} else {
			//Remove the imContact from the contactProfile
			IMContact imContact(imProtocol, contactId.toStdString());
			_contactProfile.removeIMContact(imContact);
		}
	}

	//Add IMContacts
	for (int i = 0; i < contactIdListAfter.size(); i++) {
		QString contactId = contactIdListAfter[i];

		if (imContactsBefore.contains(contactId)) {
			//Do nothing
		} else {
			//Add the imContact to the contactProfile
			IMContact imContact(imProtocol, contactId.toStdString());

			//IMAccount to associate with the IMContact
			std::set<IMAccount *> imAccounts = _cUserProfile.getIMAccountsOfProtocol(imProtocol);
			std::set<IMAccount *>::const_iterator it = imAccounts.begin();
			if (it != imAccounts.end()) {
				imContact.setIMAccount(*it);
			}

			_contactProfile.addIMContact(imContact);
		}
	}
}

void QtSimpleIMContactManager::saveIMContacts() {
	if (_ui->wengoLineEdit->isEnabled()) {
		addIMContactsOfProtocol(EnumIMProtocol::IMProtocolWengo, _ui->wengoLineEdit->text());
	}
	if (_ui->msnLineEdit->isEnabled()) {
		addIMContactsOfProtocol(EnumIMProtocol::IMProtocolMSN, _ui->msnLineEdit->text());
	}
	if (_ui->aimLineEdit->isEnabled()) {
		addIMContactsOfProtocol(EnumIMProtocol::IMProtocolAIMICQ, _ui->aimLineEdit->text());
	}
	if (_ui->yahooLineEdit->isEnabled()) {
		addIMContactsOfProtocol(EnumIMProtocol::IMProtocolYahoo, _ui->yahooLineEdit->text());
	}
	if (_ui->jabberLineEdit->isEnabled()) {
		addIMContactsOfProtocol(EnumIMProtocol::IMProtocolJabber, _ui->jabberLineEdit->text());
	}
}

void QtSimpleIMContactManager::searchWengoContactButtonClicked() {
	//_imContactManagerWidget->parentWidget()->close();
	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cUserProfile.getCWengoPhone().getPresentation();
	//qtWengoPhone->searchWengoContact();
}
