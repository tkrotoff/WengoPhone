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

#include "QtAddContact.h"

#include "QtContactWidget.h"

#include <model/WengoPhoneLogger.h>
#include <model/contactlist/Contact.h>
#include <model/presence/PresenceHandler.h>
#include <control/CWengoPhone.h>

#include <WidgetFactory.h>
#include <Object.h>
#include <StringListConvert.h>

#include <QtGui>

QtAddContact::QtAddContact(CWengoPhone & cWengoPhone, QWidget * parent)
	: QObject(parent),
	_cWengoPhone(cWengoPhone) {

	_addContactWindow = WidgetFactory::create(":/forms/contactlist/AddContactWindow.ui", parent);

	QStringList tmp = StringListConvert::toQStringList(_cWengoPhone.getContactGroupStringList());
	_qtContactWidget = new QtContactWidget(NULL, tmp, NULL);
	QWidget * contactWidget = _qtContactWidget->getWidget();

	//stackedWidget
	QStackedWidget * stackedWidget = Object::findChild<QStackedWidget *>(_addContactWindow, "stackedWidget");
	stackedWidget->addWidget(contactWidget);
	stackedWidget->setCurrentWidget(contactWidget);

	//addButton
	QPushButton * addButton = Object::findChild<QPushButton *>(_addContactWindow, "addButton");
	connect(addButton, SIGNAL(clicked()), SLOT(addContact()));

	_addContactWindow->show();
}

void QtAddContact::showContactGroups() {
}

void QtAddContact::addContact() {
	Contact * contact = new Contact(PresenceHandler::getInstance());

	QString contactGroupName = _qtContactWidget->createContact(contact);

	((QDialog *) _addContactWindow)->accept();

	LOG_DEBUG("contact: " + contactGroupName.toStdString() + contact->toString());

	_cWengoPhone.addContact(contact, contactGroupName.toStdString());
}
