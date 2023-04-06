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

#include "AudioCallManager.h"
#include "ContactWindow.h"
#include "ContactList.h"
#include "ContactListWidget.h"
#include "AudioCall.h"
#include "sip/SipAddress.h"
#include "sip/SipPrimitives.h"
#ifdef ENABLE_VIDEO
#include "config/Video.h"
#endif

#include <qlistview.h>
#include <qpoint.h>
#include <qwidgetfactory.h>
#include <qdialog.h>
#include <qstring.h>

#include <iostream>
using namespace std;

static const int ID_BLOCK_MENUITEM = 1;

ContactPopupMenu::ContactPopupMenu(QWidget * parent)
: QPopupMenu(parent) {
	_parent = parent;
	QPopupMenu * callPopupMenu = new QPopupMenu(_parent);
	callPopupMenu->insertItem(QPixmap::fromMimeSource("contact_phone_wengo.png"), tr("Wengo Phone"),
				this, SLOT(callWengoPhone()));
	callPopupMenu->insertItem(QPixmap::fromMimeSource("contact_phone_mobile.png"), tr("Mobile Phone"),
				this, SLOT(callMobilePhone()));
	callPopupMenu->insertItem(QPixmap::fromMimeSource("contact_phone_home.png"), tr("Home Phone"),
				this, SLOT(callHomePhone()));
	callPopupMenu->insertItem(QPixmap::fromMimeSource("contact_phone_work.png"), tr("Work Phone"),
				this, SLOT(callWorkPhone()));

	insertItem(QPixmap::fromMimeSource("call_outgoing.png"), tr("Call"), callPopupMenu);
	insertItem(QPixmap::fromMimeSource("sms_send.png"), tr("Send SMS"), this, SLOT(sendSms()));
	insertItem(QPixmap::fromMimeSource("user.png"), tr("Start Chat..."),
			this, SLOT(startChat()));
	insertSeparator();
	/*insertItem(tr("Send File"));
	insertItem(tr("Send Another Contact"));
	insertSeparator();*/
	insertItem(QPixmap::fromMimeSource("contact_view.png"), tr("View/Modify Profile"), this, SLOT(showContact()));
	//insertItem(tr("View History"));
	//insertSeparator();
	insertItem(QPixmap::fromMimeSource("contact_remove.png"), tr("Remove Contact"), this, SLOT(removeContact()));
	insertItem(QPixmap::fromMimeSource("contact_block.png"), tr("Block Contact"), this, SLOT(blockContact()), 0, ID_BLOCK_MENUITEM);
}

void ContactPopupMenu::showMenu(QListViewItem * item, const QPoint & pos, int) {
	//NULL if the user didn't click on an item
	if (item == NULL) {
		return;
	}

	//No popup menu for the "No contact" entry
	if( item->text(ContactListWidget::LISTVIEW_COLUMN_CONTACT_FULLNAME) == tr("No contact") ) {
		return;
	}
	
	_contactId = ContactId(item->text(ContactListWidget::LISTVIEW_COLUMN_CONTACT_ID));
	ContactList & contactList = ContactList::getInstance();
	Contact & contact = contactList.getContact(_contactId);

	if (contact.isBlocked()) {
		changeItem(ID_BLOCK_MENUITEM, QPixmap::fromMimeSource("contact_unblock.png"), tr("Unblock Contact"));
	} else {
		changeItem(ID_BLOCK_MENUITEM, QPixmap::fromMimeSource("contact_block.png"), tr("Block Contact"));
	}

	exec(pos);
}

void ContactPopupMenu::showContact() const {
	ContactList & contactList = ContactList::getInstance();
	Contact & contact = contactList.getContact(_contactId);

	ContactWindow * contactWindow = new ContactWindow(_parent, &contact);
	contactWindow->execShowContact();
}

void ContactPopupMenu::removeContact() const {
	ContactListWidget::removeContact(_contactId, _parent);
}

void ContactPopupMenu::callWengoPhone() const {
	ContactList & contactList = ContactList::getInstance();
	Contact & contact = contactList.getContact(_contactId);
	QString number = contact.getWengoPhone();

	if (!number.isEmpty()) {
#ifdef ENABLE_VIDEO
		if (Video::getInstance().getEnable()) {
			AudioCallManager::getInstance().createAudioCall(
					SipAddress::fromPhoneNumber(number), &contact, true);
		} else
#endif
		{
			AudioCallManager::getInstance().createAudioCall(
					SipAddress::fromPhoneNumber(number), &contact, false);
		}
	}
}

void ContactPopupMenu::callMobilePhone() const {
	ContactList & contactList = ContactList::getInstance();
	Contact & contact = contactList.getContact(_contactId);
	QString number = contact.getMobilePhone();

	if (!number.isEmpty()) {
		AudioCallManager::getInstance().createAudioCall(
					SipAddress::fromPhoneNumber(number), &contact);
	}
}

void ContactPopupMenu::callHomePhone() const {
	ContactList & contactList = ContactList::getInstance();
	Contact & contact = contactList.getContact(_contactId);
	QString number = contact.getHomePhone();

	if (!number.isEmpty()) {
	AudioCallManager::getInstance().createAudioCall(
					SipAddress::fromPhoneNumber(number), &contact);
	}
}

void ContactPopupMenu::callWorkPhone() const {
	ContactList & contactList = ContactList::getInstance();
	Contact & contact = contactList.getContact(_contactId);
	QString number = contact.getWorkPhone();

	if (!number.isEmpty()) {
		AudioCallManager::getInstance().createAudioCall(
					SipAddress::fromPhoneNumber(number), &contact);
	}
}

void ContactPopupMenu::sendSms() const {
	ContactList & contactList = ContactList::getInstance();
	Contact & contact = contactList.getContact(_contactId);

	AudioCallManager::getInstance().sendSms(contact.getMobilePhone(), "", &contact);
}

void ContactPopupMenu::startChat() const {
	ContactList & contactList = ContactList::getInstance();
	Contact & contact = contactList.getContact(_contactId);

	AudioCallManager::getInstance().startChat(contact);
}

void ContactPopupMenu::blockContact() const {
	ContactList & contactList = ContactList::getInstance();
	Contact & contact = contactList.getContact(_contactId);
	contact.setBlocked(!contact.isBlocked());

	if (contact.isBlocked()) {
		//Block contact
		SipPrimitives::forbidWatcher(SipAddress::fromPhoneNumber(contact.getWengoPhone()));
	} else {
		//Unblock contact
		SipPrimitives::allowWatcher(SipAddress::fromPhoneNumber(contact.getWengoPhone()));
		SipPrimitives::subscribeToPresence(SipAddress::fromPhoneNumber(contact.getWengoPhone()));
	}

	contact.save();
}
