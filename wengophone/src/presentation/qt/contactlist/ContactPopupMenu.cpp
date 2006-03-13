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

#include "ContactPopupMenu.h"

#include <model/WengoPhone.h>
#include <model/contactlist/Contact.h>
#include <model/chat/ChatHandler.h>

#include <QtGui>

#include <Logger.h>

#include <iostream>
using namespace std;

ContactPopupMenu::ContactPopupMenu(QWidget * parent, WengoPhone & wengoPhone)
	: QMenu(parent), _wengoPhone(wengoPhone) {
	_contact = NULL;

	QMenu * callMenu = new QMenu(parent);
	callMenu->setIcon(QIcon(":/pics/call_outgoing.png"));
	callMenu->setTitle(tr("Call"));
	callMenu->addAction(QIcon(":/pics/contact_phone_wengo.png"), tr("Wengo Phone"),
				this, SLOT(showWengoPhone()));
	callMenu->addAction(QIcon(":/pics/contact_phone_mobile.png"), tr("Mobile Phone"),
				this, SLOT(showMobilePhone()));
	callMenu->addAction(QIcon(":/pics/contact_phone_home.png"), tr("Home Phone"),
				this, SLOT(showHomePhone()));
	callMenu->addAction(QIcon(":/pics/contact_phone_work.png"), tr("Work Phone"),
				this, SLOT(showWorkPhone()));

	addMenu(callMenu);
	addAction(QIcon(":/pics/sms_send.png"), tr("Send SMS"), this, SLOT(sendSms()));
	addAction(QIcon(":/pics/start_chat.png"), tr("Start Chat..."),
			this, SLOT(startChat()));
	addSeparator();
	/*insertItem(tr("Send File"));
	insertItem(tr("Send Another Contact"));
	insertSeparator();*/
	addAction(QIcon(":/pics/contact_view.png"), tr("View/Modify Profile"), this, SLOT(showContact()));
	//insertItem(tr("View History"));
	//insertSeparator();
	addAction(QIcon(":/pics/contact_remove.png"), tr("Remove Contact"), this, SLOT(removeContact()));
	addAction(QIcon(":/pics/contact_block.png"), tr("Block Contact"), this, SLOT(blockContact()));
}

void ContactPopupMenu::showMenu(const Contact * contact) {
	if (!contact) {
		return;
	}

	_contact = contact;
	exec(QCursor::pos());
}

void ContactPopupMenu::showContact() const {
}

void ContactPopupMenu::removeContact() const {
}

void ContactPopupMenu::showWengoPhone() const {
}

void ContactPopupMenu::showMobilePhone() const {
}

void ContactPopupMenu::showHomePhone() const {
}

void ContactPopupMenu::showWorkPhone() const {
}

void ContactPopupMenu::sendSms() const {
}

void ContactPopupMenu::startChat() const {
	LOG_DEBUG("starting chat with " + _contact->getWengoPhoneId());
	//_wengoPhone.getChatHandler().createSession();
}

void ContactPopupMenu::blockContact() const {
}
