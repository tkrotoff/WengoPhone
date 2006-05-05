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
#include "QtUserManager.h"

#include <control/CWengoPhone.h>

#include <QTreeWidgetItem>
#include <QString>

#include <util/Logger.h>

using namespace std;

QtContactGroup::QtContactGroup(CContactGroup & cContactGroup, QtContactList * qtContactList)
	: QObjectThreadSafe(NULL),
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
	if (list.size() > 0) {
		// A goup name is unique inside the model so we are sure that the first
		// QTreeWidgetItem is the right one.
		QtUserList * ul = QtUserList::getInstance();

		QTreeWidgetItem * newContact;
		QString contactName;
		QString contactId;
		QtUser * user;

		LOG_DEBUG("display name: " + pContact->getCContact().getDisplayName());

		QUuid uid = QUuid::createUuid (); // Unique user identifier

		newContact = new QTreeWidgetItem(list[0]);
		newContact->setText(0, uid.toString());
		newContact->setFlags(newContact->flags() | Qt::ItemIsEditable);

		user = new QtUser(pContact->getCContact(), _cContactGroup.getCWengoPhone().getWengoPhone());
		user->setId(uid.toString());

		ul->addUser(user);
	}
}

void QtContactGroup::removeContact(PContact * pContact) {
	typedef PostEvent1<void (PContact *), PContact *> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactGroup::removeContactThreadSafe, this, _1), pContact);
	postEvent(event);
}

void QtContactGroup::removeContactThreadSafe(PContact * pContact) {
	_qtContactList->getUserManager()->removeContact(pContact->getCContact());
	_cContactGroup.contactReleased(pContact);
	updatePresentation();
}

void QtContactGroup::updatePresentation() {
	_qtContactList->updatePresentation();
}

void QtContactGroup::updatePresentationThreadSafe() {
}
