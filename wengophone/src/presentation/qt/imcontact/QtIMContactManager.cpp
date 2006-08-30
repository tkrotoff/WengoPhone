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

#include "QtIMContactManager.h"

#include "ui_IMContactManager.h"

#include "QtIMContactItem.h"
#include "QtAddIMContact.h"

#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>

#include <model/contactlist/ContactProfile.h>

#include <QtGui/QtGui>

QtIMContactManager::QtIMContactManager(ContactProfile & contactProfile,
	CUserProfile & cUserProfile, QWidget * parent)
	: QObject(parent),
	_contactProfile(contactProfile),
	_cUserProfile(cUserProfile),
	_parent(parent) {

	_imContactManagerWidget = new QWidget(NULL);

	_ui = new Ui::IMContactManager();
	_ui->setupUi(_imContactManagerWidget);

	connect(_ui->addIMContactButton, SIGNAL(clicked()), SLOT(addIMContact()));

	connect(_ui->deleteIMContactButton, SIGNAL(clicked()), SLOT(deleteIMContact()));

	loadIMContacts();
}

QtIMContactManager::~QtIMContactManager() {
	delete _ui;
}

void QtIMContactManager::loadIMContacts() {
	_ui->treeWidget->clear();

	const IMContactSet & imContactSet = _contactProfile.getIMContactSet();

	for (IMContactSet::const_iterator it = imContactSet.begin(); it != imContactSet.end(); it++) {

		IMContact * imContact = (IMContact *) &(*it);
		QStringList imContactStrList;
		imContactStrList << QString::fromStdString(imContact->getContactId());
		EnumIMProtocol::IMProtocol imProtocol = imContact->getProtocol();

		if (imProtocol == EnumIMProtocol::IMProtocolSIPSIMPLE) {
			//This protocol is internal to WengoPhone, should not be shown to the user
			//continue;
		}

		imContactStrList << QString::fromStdString(EnumIMProtocol::toString(imProtocol));
		imContactStrList << QString::null;

		QtIMContactItem * item = new QtIMContactItem(_ui->treeWidget, imContactStrList);
		item->setIMContact(imContact);
	}
}

void QtIMContactManager::addIMContact() {
	QtAddIMContact * qtAddIMContact = new QtAddIMContact(_contactProfile, _cUserProfile, _imContactManagerWidget);
	loadIMContacts();
}

void QtIMContactManager::deleteIMContact() {
	QtIMContactItem * imContactItem = (QtIMContactItem *) _ui->treeWidget->currentItem();
	if (imContactItem) {
		IMContact * imContact = imContactItem->getIMContact();

		int buttonClicked = QMessageBox::question(_imContactManagerWidget,
					"WengoPhone",
					tr("Are sure you want to delete this IM contact?\n") +
					QString::fromStdString(imContact->getContactId()),
					tr("&Delete"), tr("Cancel"));

		//Button delete clicked
		if (buttonClicked == 0) {
			_contactProfile.removeIMContact(*imContact);
		}
	}

	loadIMContacts();
}
