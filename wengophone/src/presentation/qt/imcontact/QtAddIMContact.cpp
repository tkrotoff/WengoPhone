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

#include "QtAddIMContact.h"

#include "ui_AddIMContact.h"

#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>

#include <model/contactlist/ContactProfile.h>

#include <QtGui>

static const QString IM_PROTOCOL_WENGO = "Wengo";

QtAddIMContact::QtAddIMContact(ContactProfile & contactProfile,
	CUserProfile & cUserProfile, QWidget * parent)
	: QObject(parent),
	_contactProfile(contactProfile),
	_cUserProfile(cUserProfile) {

	_addIMContactWindow = new QDialog(parent);

	_ui = new Ui::AddIMContact();
	_ui->setupUi(_addIMContactWindow);

	_ui->protocolComboBox->addItem(QIcon(":pics/protocols/wengo.png"), IM_PROTOCOL_WENGO);

	_ui->protocolComboBox->addItem(QIcon(":pics/protocols/msn.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolMSN)));
	_ui->protocolComboBox->addItem(QIcon(":pics/protocols/aim.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolAIMICQ)));
	_ui->protocolComboBox->addItem(QIcon(":pics/protocols/yahoo.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolYahoo)));
	_ui->protocolComboBox->addItem(QIcon(":pics/protocols/jabber.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolJabber)));
	_ui->protocolComboBox->addItem(QIcon(":pics/protocols/sip.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolSIPSIMPLE)));

	connect(_ui->addIMContactButton, SIGNAL(clicked()), SLOT(addIMContact()));

	connect(_ui->protocolComboBox, SIGNAL(currentIndexChanged(const QString &)), SLOT(imProtocolChanged(const QString &)));

	show();
}

QtAddIMContact::~QtAddIMContact() {
	delete _ui;
}

int QtAddIMContact::show() {
	imProtocolChanged(_ui->protocolComboBox->currentText());
	return _addIMContactWindow->exec();
}

void QtAddIMContact::addIMContact() {
	std::string contactId = _ui->contactIdLineEdit->text().toStdString();

	QString protocolName = _ui->protocolComboBox->currentText();

	if (protocolName == IM_PROTOCOL_WENGO) {
		_contactProfile.setWengoPhoneId(contactId);
	} else {
		EnumIMProtocol::IMProtocol imProtocol = EnumIMProtocol::toIMProtocol(protocolName.toStdString());
		IMContact imContact(imProtocol, contactId);

		std::set<IMAccount *> imAccounts = getSelectedIMAccounts(imProtocol);

		if (imAccounts.empty()) {
			_contactProfile.addIMContact(imContact);
		}

		for (std::set<IMAccount *>::const_iterator it = imAccounts.begin();
			it != imAccounts.end(); ++it) {

			imContact.setIMAccount(*it);
			_contactProfile.addIMContact(imContact);
		}
	}
}

void QtAddIMContact::imProtocolChanged(const QString & protocolName) {
	EnumIMProtocol::IMProtocol imProtocol;

	if (protocolName == IM_PROTOCOL_WENGO) {
		imProtocol = EnumIMProtocol::IMProtocolSIPSIMPLE;
	} else {
		imProtocol = EnumIMProtocol::toIMProtocol(protocolName.toStdString());
	}

	loadIMAccounts(imProtocol);
}

void QtAddIMContact::loadIMAccounts(EnumIMProtocol::IMProtocol imProtocol) {
	_ui->treeWidget->clear();

	std::set<IMAccount *> imAccounts = _cUserProfile.getIMAccountsOfProtocol(imProtocol);

	for (std::set<IMAccount *>::const_iterator it = imAccounts.begin();
		it != imAccounts.end(); ++it) {

		QTreeWidgetItem * item = new QTreeWidgetItem(_ui->treeWidget);

		//By default, check the first element only
		if (it == imAccounts.begin()) {
			item->setCheckState(0, Qt::Checked);
		}

		else {
			item->setCheckState(0, Qt::Unchecked);
		}
		item->setText(1, QString::fromStdString((*it)->getLogin()));
	}
}

std::set<IMAccount *> QtAddIMContact::getSelectedIMAccounts(EnumIMProtocol::IMProtocol imProtocol) const {
	std::set<IMAccount *> result;

	std::set<IMAccount *> imAccounts = _cUserProfile.getIMAccountsOfProtocol(imProtocol);

	for (std::set<IMAccount *>::const_iterator it = imAccounts.begin();
		it != imAccounts.end(); ++it) {

		QList<QTreeWidgetItem *> list =
			_ui->treeWidget->findItems(QString::fromStdString((*it)->getLogin()), Qt::MatchExactly, 1);

		if (list.size() > 0) {
			//There should be only one item
			QTreeWidgetItem * item = list[0];
			if (item->checkState(0) == Qt::Checked) {
				result.insert(*it);
			}
		}
	}

	return result;
}
