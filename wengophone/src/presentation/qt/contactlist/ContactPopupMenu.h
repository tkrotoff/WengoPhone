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

#ifndef CONTACTPOPUPMENU_H
#define CONTACTPOPUPMENU_H

#include <util/NonCopyable.h>

#include <QMenu>

class Contact;
class QTreeWidgetItem;
class QModelIndex;
class WengoPhone;

/**
 * Menu of a Contact, popped each time a user clicks on a Contact using
 * the right button of the mouse.
 *
 * @author Tanguy Krotoff
 */
class ContactPopupMenu : public QMenu, NonCopyable {
	Q_OBJECT
public:

	/**
	 * Constructs a ContactPopupMenu.
	 *
	 * @param parent parent widget
	 */
	ContactPopupMenu(QWidget * parent, WengoPhone & wengoPhone);

public Q_SLOTS:

	/**
	 * The popup menu is being showed.
	 *
	 * @param contact the menu is relative to a Contact
	 */
	void showMenu(const Contact * contact);

private Q_SLOTS:

	/**
	 * Shows the SessionWindow with the properties of this Contact.
	 *
	 * The user clicks on "view/modify profile".
	 */
	void showContact() const;

	/**
	 * Shows the SessionWindow ready to make a call using the
	 * Wengo phone number of the Contact.
	 *
	 * The user clicks on "call / wengo phone" in the menu.
	 */
	void showWengoPhone() const;

	/**
	 * @see showWengoPhone
	 */
	void showMobilePhone() const;

	/**
	 * @see showWengoPhone
	 */
	void showHomePhone() const;

	/**
	 * @see showWengoPhone
	 */
	void showWorkPhone() const;

	/**
	 * Removes a Contact from ContactList.
	 *
	 * The user clicks on "remove contact" in the menu.
	 */
	void removeContact() const;

	/**
	 * Blocks a Contact.
	 */
	void blockContact() const;

	/**
	 * The user wants to send a SMS.
	 */
	void sendSms() const;

	/**
	 * The user wants to start a chat conversation.
	 */
	void startChat() const;

private:

	const Contact * _contact;

	WengoPhone & _wengoPhone;

};

#endif	//CONTACTPOPUPMENU_H
