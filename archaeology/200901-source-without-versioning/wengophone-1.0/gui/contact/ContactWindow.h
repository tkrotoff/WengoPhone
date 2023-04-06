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

#ifndef CONTACTWINDOW_H
#define CONTACTWINDOW_H

#include "ContactWidget.h"

#include <qobject.h>

#include <iostream>
using namespace std;

class Contact;
class QWidget;
class QWidgetStack;

/**
 * Wrapper of the gui window to add or update a Contact.
 *
 * The window is composed by a ContactWidget that represents a Contact graphically.
 *
 * @author Tanguy Krotoff
 */
class ContactWindow : public QObject {
	Q_OBJECT
public:

	/**
	 * Constructs a ContactWindow given a Contact.
	 *
	 * @param parent pointer to the QWidget parent
	 * @param contact Contact that will be showed inside the window
	 */
	ContactWindow(QWidget * parent, Contact * contact = NULL);

	~ContactWindow();

	/**
	 * Gets the low-level widget of this class.
	 *
	 * @return low-level widget
	 */
	QDialog * getWidget() const {
		return _contactWindow;
	}
	
	ContactWidget * getContactWidget() {
		return _contactWidget;
	}


public slots:

	/**
	 * Shows the dialog as a modal dialog, blocking until the user closes it.
	 *
	 * @return DialogCode result: QDialog::Accepted or QDialog::Rejected
	 * @see QDialog
	 */
	int execAddContact();

	/**
	 * Shows the dialog as a modal dialog, blocking until the user closes it.
	 *
	 * @return DialogCode result: QDialog::Accepted or QDialog::Rejected
	 * @see QDialog
	 */
	int execShowContact();

private:

	ContactWindow(const ContactWindow &);
	ContactWindow & operator=(const ContactWindow &);

	/**
	 * Resets the window: replaces the ContactWidget component of the window
	 * by another one that is empty.
	 */
	void resetWindow();

	/**
	 * Low-level widget component of this class.
	 */
	QDialog * _contactWindow;

	/**
	 * ContactWidget component.
	 */
	ContactWidget * _contactWidget;

	Contact * _contact;

	/**
	 * QWidgetStack where the ContactWidget takes place.
	 */
	QWidgetStack * _stack;
};

#endif	//CONTACTWINDOW_H
