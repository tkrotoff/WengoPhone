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

#include "ContactListDelegate.h"
#include <model/contactlist/Contact.h>
#include <model/contactlist/ContactGroup.h>
#include "MetaContact.h"
#include "MetaContactGroup.h"
#include "ContactListUtil.h"
#include "QtContact.h"

#include <Logger.h>
#include <QPainter>
#include <QTreeView>

#include <string>
using namespace std;

ContactListDelegate::ContactListDelegate(QTreeView * view, QObject *parent)
	: QItemDelegate(parent) {
	
	_view = view;
}

void ContactListDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
	IQtContact *iQtContact;
	ContactGroup * contactGroup;

	if (index.parent().isValid()) {
		
		iQtContact = ContactListUtil::getIQtContact(index.data(Qt::UserRole));
		contactGroup = ContactListUtil::getContactGroup(index.parent().data(Qt::UserRole));
		
		assert((iQtContact != NULL) && "iQtContact must be a valid pointer");
		assert((contactGroup != NULL) && "contactGroup must be a valid pointer");
		
		drawContact(painter, option, index, iQtContact, contactGroup);

	} else {
		contactGroup = ContactListUtil::getContactGroup(index.data(Qt::UserRole));

		assert((contactGroup != NULL) && "contactGroup must be a valid pointer");
		
		drawContactGroup(painter, option, index, contactGroup);

	}
}

QSize ContactListDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const {
	QSize sz;
	
	if (index.parent().isValid()) {
		IQtContact *iQtContact = ContactListUtil::getIQtContact(index.data(Qt::UserRole));
		if (iQtContact->isDisplayDetailsEnabled()) {
			sz = QItemDelegate::sizeHint(option, index) + QSize(2, 2 + 32);
		} else {
			sz = QItemDelegate::sizeHint(option, index) + QSize(2, 2);
		}
	} else {
		sz = QItemDelegate::sizeHint(option, index) + QSize(2, 2);
	}
	return sz;
}

void ContactListDelegate::drawContact(QPainter *painter, const QStyleOptionViewItem & option,
	const QModelIndex &index, IQtContact *iQtContact, ContactGroup *contactGroup) const {

	QVariant v = index.data(Qt::UserRole);
	QIcon icon(":/pics/contact_phone_wengo.png");
	
	if (iQtContact->isDisplayDetailsEnabled()) {
		QRect rect = option.rect;
		
		// Contact Icon
		rect.setHeight(16);
		_view->style()->drawItemPixmap(painter, rect, Qt::AlignLeft,
			icon.pixmap(16, 16));
			
		// Contact Name
		rect.setX(rect.x() + 18);
		_view->style()->drawItemText(painter, rect, 
			Qt::AlignLeft, option.palette, true, QString::fromStdString(iQtContact->getContact()->getFirstName()));
		
		// Display communication possibilities
		rect.setY(rect.y() + 16);
		/*
		QStyleOptionButton buttonOption;
		//buttonOption.state = QStyle::State_Off;
		buttonOption.rect = rect;
		buttonOption.palette = option.palette;

		_view->style()->drawControl(QStyle::CE_RadioButton, &buttonOption, painter);
		*/
		
	} else {
		QItemDelegate::paint(painter, option, index);
	}
}

void ContactListDelegate::drawContactGroup(QPainter *painter, const QStyleOptionViewItem & option,
	const QModelIndex &index, ContactGroup *contactGroup) const {

	const QAbstractItemModel *model = index.model();
	QStyleOptionButton buttonOption;

	buttonOption.state = option.state;
#ifdef Q_WS_MAC
	buttonOption.state |= QStyle::State_Raised;
#endif
	buttonOption.state &= ~QStyle::State_HasFocus;

	buttonOption.rect = option.rect;
	buttonOption.palette = option.palette;
	buttonOption.features = QStyleOptionButton::None;
	buttonOption.text = QString::fromStdString(contactGroup->toString());

	_view->style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter, _view);
}
