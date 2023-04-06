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

#include "ContactWindow.h"

#include "Contact.h"
#include "ContactList.h"
#include "MyWidgetFactory.h"
#include "Softphone.h"

#include <qdialog.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>

#include <iostream>
using namespace std;

ContactWindow::ContactWindow(QWidget * parent, Contact * contact)
: QObject(parent) {
	_contact = contact;

	_contactWindow = (QDialog *) MyWidgetFactory::create("ContactWindowForm.ui", this, parent);

	_stack = (QWidgetStack *) _contactWindow->child("contactWidgetStack", "QWidgetStack");

	_contactWidget = new ContactWidget(_contactWindow);

	//Add/update contact button
	QPushButton * addButton = (QPushButton *) _contactWindow->child("addButton",
									"QPushButton");
	connect(addButton, SIGNAL(clicked()),
			_contactWidget, SLOT(updateContact()));

	if (_contact != NULL) {
		_contactWidget->setContact(*_contact);
		addButton->setText(tr("&Update Contact"));
	}

	_stack->addWidget(_contactWidget->getWidget());
	_stack->raiseWidget(_contactWidget->getWidget());
}

ContactWindow::~ContactWindow() {
	delete _contactWidget;
	delete _stack;
	delete _contactWindow;
}

void ContactWindow::resetWindow() {
	/*_stack->removeWidget(_contactWidget->getWidget());
	//FIXME delete _contactWidget;
	//removeWidget() calls the destructor? probably not

	_contactWidget = new ContactWidget(_contactWindow);

	_stack->addWidget(_contactWidget->getWidget());
	_stack->raiseWidget(_contactWidget->getWidget());*/
	_contactWidget->reset();
}

int ContactWindow::execAddContact() {
	if (!Softphone::getInstance().isUserLogged()) {
		return QDialog::Rejected;
	}

	resetWindow();
	_contactWidget->setContact(*new Contact());

	return _contactWindow->exec();
}

int ContactWindow::execShowContact() {
	return _contactWindow->exec();
}
