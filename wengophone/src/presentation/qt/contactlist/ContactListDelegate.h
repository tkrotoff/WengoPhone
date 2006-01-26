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

#ifndef CONTACTLISTDELEGATE_H
#define CONTACTLISTDELEGATE_H

#include <QItemDelegate>
#include <QModelIndex>

class QPainter;
class QTreeView;
class QObject;
class Contact;
class ContactGroup;
class IQtContact;

class ContactListDelegate : public QItemDelegate {
	Q_OBJECT
public:

	ContactListDelegate(QTreeView *view, QObject *parent);

	void paint(QPainter * painter, const QStyleOptionViewItem & option,
		const QModelIndex & index) const;
	QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
	
private:
	/**
	 * Draw a contact.
	 *
	 * @param painter painter used to draw
	 * @param option style option
	 * @param index index of the contact to draw
	 * @param iQtContact QtContact to draw
	 * @param contactGroup parent contact group of the contact
	 */
	void drawContact(QPainter *painter, const QStyleOptionViewItem & option, 
		const QModelIndex &index, IQtContact *iQtContact, ContactGroup *contactGroup) const;
	
	/**
	 * Draw a contact group.
	 *
	 * @param painter painter used to draw
	 * @param option style option
	 * @param index index of the contact group to draw
	 * @param contactGroup the contact group to draw
	 */
	void drawContactGroup(QPainter *painter, const QStyleOptionViewItem & option, 
		const QModelIndex &index, ContactGroup *contactGroup) const;
	
	/**
	 * Return the height in pixel taken by a contact group.
	 *
	 * @param contactGroup the contact group to get the size from
	 * @return the height taken by the contact group in pixel
	 */
	unsigned getContactGroupHeight(ContactGroup *contactGroup);

	/**
	 * Return the height in pixel taken by a contact.
	 *
	 * @return the height taken by a contact in pixel
	 */
	unsigned getContactHeight(Contact *contact);
	
	/** Vertical position of last element drawn */
	unsigned _y;
	
	/** Pointer to view */
	QTreeView * _view;
};

#endif	//CONTACTLISTDELEGATE_H
