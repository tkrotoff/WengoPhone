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

#include <model/contactlist/Contact.h>

#include <QtGui>

QtAddIMContact::QtAddIMContact(Contact & contact, QWidget * parent)
	: QObject(parent),
	_contact(contact) {

	_addIMContactWindow = new QDialog(parent);

	_ui = new Ui::AddIMContact();
	_ui->setupUi(_addIMContactWindow);

	_ui->protocolComboBox->addItem(QIcon(":pics/protocols/msn.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolMSN)));
	_ui->protocolComboBox->addItem(QIcon(":pics/protocols/aim.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolAIMICQ)));
	_ui->protocolComboBox->addItem(QIcon(":pics/protocols/yahoo.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolYahoo)));
	_ui->protocolComboBox->addItem(QIcon(":pics/protocols/jabber.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolJabber)));
	_ui->protocolComboBox->addItem(QIcon(":pics/protocols/jabber.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolGoogleTalk)));

	connect(_ui->addIMContactButton, SIGNAL(clicked()), SLOT(addIMContact()));

	show();
}

QtAddIMContact::~QtAddIMContact() {
	delete _ui;
}

int QtAddIMContact::show() {
	return _addIMContactWindow->exec();
}

void QtAddIMContact::addIMContact() {
	QString protocolName = _ui->protocolComboBox->currentText();
	EnumIMProtocol::IMProtocol imProtocol = EnumIMProtocol::toIMProtocol(protocolName.toStdString());
	QString contactId = _ui->contactIdLineEdit->text();
	IMContact imContact(imProtocol, contactId.toStdString());
	//imContact.setIMAccount(imAccount);
	_contact.addIMContact(imContact);
}
