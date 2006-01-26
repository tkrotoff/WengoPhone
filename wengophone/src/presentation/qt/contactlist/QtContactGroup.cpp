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
#include "TreeItem.h"
#include "QtContact.h"
#include "QtContactList.h"

QtContactGroup::QtContactGroup(CContactGroup & cContactGroup, QtContactList * qtContactList)
	: QObjectThreadSafe(),
	_cContactGroup(cContactGroup) {

	_qtContactList = qtContactList;

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactGroup::initThreadSafe, this));
	postEvent(event);
}

QtContactGroup::~QtContactGroup() {
	delete _metaContactGroup;
	delete _item;
}

void QtContactGroup::initThreadSafe() {
	qRegisterMetaType<MetaContactGroup>("MetaContactGroup");

	_metaContactGroup = new MetaContactGroup(_cContactGroup.getContactGroup());

	_item = new TreeItem(QVariant(QMetaType::type("MetaContactGroup"), _metaContactGroup));
}

void QtContactGroup::addContact(PContact * pContact) {
	typedef PostEvent1<void (PContact *), PContact *> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactGroup::addContactThreadSafe, this, _1), pContact);
	postEvent(event);
}

void QtContactGroup::addContactThreadSafe(PContact * pContact) {
	QtContact * qtContact = (QtContact *) pContact;

	_item->appendChild(qtContact->getTreeItem());
	updatePresentation();
}

void QtContactGroup::updatePresentation() {
	_qtContactList->updatePresentation();
}

void QtContactGroup::updatePresentationThreadSafe() {
	//_item->setData(_metaContactGroup);
}
