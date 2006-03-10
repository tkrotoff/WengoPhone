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

#include "QtContactGroup.h"

#include "QtContact.h"
#include "QtContactList.h"
#include "QtUserList.h"
#include "QtUser.h"

#include <control/CWengoPhone.h>

#include <QTreeWidgetItem>
#include <QString>

#include <Logger.h>

using namespace std;

QtContactGroup::QtContactGroup(CContactGroup & cContactGroup, QtContactList * qtContactList)
	: QObjectThreadSafe(),
	_cContactGroup(cContactGroup) {

	_qtContactList = qtContactList;

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactGroup::initThreadSafe, this));
	postEvent(event);
}

QtContactGroup::~QtContactGroup() {
}

void QtContactGroup::initThreadSafe() {
}

string QtContactGroup::getDisplayName() const {
	return _cContactGroup.getContactGroupName();
}

void QtContactGroup::addContact(PContact * pContact) {
	typedef PostEvent1<void (PContact *), PContact *> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactGroup::addContactThreadSafe, this, _1), pContact);
	postEvent(event);
}

void QtContactGroup::addContactThreadSafe(PContact * pContact) {
	QList<QTreeWidgetItem *> list;

	list = _qtContactList->_treeWidget->findItems(QString::fromStdString(getDisplayName()), Qt::MatchExactly);
	// A goup name is unique inside the model so we are sure that the first 
	// QTreeWidgetItem is the right one.
	QtUserList * ul = QtUserList::getInstance();

	QTreeWidgetItem * newContact; 
	QString contactName;
	QString contactId;
	QtUser * user;

	LOG_DEBUG("display name: " + pContact->getDisplayName());
	contactName = QString::fromStdString(pContact->getDisplayName());

	newContact = new QTreeWidgetItem(list[0]);
	newContact->setText(0, QString::fromStdString(pContact->getId()));
	newContact->setFlags(newContact->flags() | Qt::ItemIsEditable);

	user = new QtUser(*pContact, _cContactGroup.getCWengoPhone().getWengoPhone());
	user->setId(QString::fromStdString(pContact->getId()));
	user->setUserName(contactName);

	ul->addUser(user);
}

void QtContactGroup::removeContact(PContact * pContact) {
	typedef PostEvent1<void (PContact *), PContact *> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactGroup::removeContactThreadSafe, this, _1), pContact);
	postEvent(event);
}

void QtContactGroup::removeContactThreadSafe(PContact * pContact) {
	//TODO: remove the contact from the GUI
	LOG_DEBUG("removing contact " + pContact->getDisplayName() + " from group " + getDisplayName());
}

void QtContactGroup::updatePresentation() {
	_qtContactList->updatePresentation();
}

void QtContactGroup::updatePresentationThreadSafe() {
}

