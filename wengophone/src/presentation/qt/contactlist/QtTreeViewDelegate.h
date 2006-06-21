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

#ifndef OWQTTREEVIEWDELEGATE_H
#define OWQTTREEVIEWDELEGATE_H

#include <QItemDelegate>
#include <QSize>
#include <QPixmap>

class CWengoPhone;
class QModelIndex;
class QWidget;
class QStyleOptionViewItem;
class QAbstractItemModel;
class QRect;
class QPainter;
class QtUserWidget;

/**
 * Qt Presentation component for ContactList.
 *
 * @author Mr K
 */
class QtTreeViewDelegate : public QItemDelegate {
	Q_OBJECT
public:

	QtTreeViewDelegate(CWengoPhone & cWengoPhone, QObject * parent = 0);

	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;

	virtual void setEditorData(QWidget * editor, const QModelIndex & index) const;

	virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;

	virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

	void setParent(QWidget * parent);

	virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

protected:

	void drawGroup(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

	bool checkForUtf8(const std::string &text, int size) const;

	QPixmap getGroupBackGround(const QRect & rect) const;

	const QWidget * _parent;

	QPixmap _menuIcon;

	mutable QPixmap _groupBackground;

	CWengoPhone & _cWengoPhone;
};

#endif	//OWQTTREEVIEWDELEGATE_H
