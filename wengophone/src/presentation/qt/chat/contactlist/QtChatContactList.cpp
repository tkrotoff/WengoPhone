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

#include "QtChatContactList.h"
#include "QtChatContactListItem.h"

#include <QtGui/QtGui>

#include <util/Logger.h>

QtChatContactList::QtChatContactList(QWidget * parent)
	: QWidget(parent) {

	QGridLayout * glayout = new QGridLayout();
	glayout->setMargin(0);
	glayout->setSpacing(0);
	setLayout(glayout);
}

void QtChatContactList::addContact(const QString & contactId, QPixmap picture,
 const QString & nickname) {

	 QtChatContactListItem * item = new QtChatContactListItem(this, contactId, picture, nickname);
	_widgetMap[contactId] = item;

	QGridLayout * glayout = dynamic_cast<QGridLayout *>(layout());
	glayout->addWidget(item, _widgetMap.count(), 0);
}

void QtChatContactList::removeContact(const QString & contactId) {

	QtChatContactListItem * item = _widgetMap[contactId];
	item->hide();
	_widgetMap.remove(contactId);
	delete item;
}

