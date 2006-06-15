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

#ifndef OWQTUSERTREEEVENTMANAGER_H
#define OWQTUSERTREEEVENTMANAGER_H

#include <QtGui>

class CContactList;

/**
 * Events manager for QTreeWidget
 *
 * @author Mr K
 */
class QtUserTreeEventManager : public QObject {
	Q_OBJECT
public :

	QtUserTreeEventManager(CContactList & cContactList, QObject * parent = 0, QTreeWidget * target = 0);

public Q_SLOTS :

	void timerTimeout();

Q_SIGNALS:

	void itemEntered(QTreeWidgetItem * item);

	void itemTimeout(QTreeWidgetItem * item);

	void mouseClicked(Qt::MouseButton button);

protected:

	bool eventFilter(QObject * obj, QEvent * event);

	virtual void mousePressEvent(QMouseEvent * event);

	virtual void mouseMoveEvent(QMouseEvent * event);

	virtual void dragEnterEvent(QDragEnterEvent * event);

	virtual void dropEvent(QDropEvent * event);

	virtual void dragMoveEvent(QDragMoveEvent * event);

	virtual void mouseReleaseEvent(QMouseEvent * event);

	QTreeWidget * _tree;

	QPoint _dstart;

	QTreeWidgetItem * _selectedItem;

	QTreeWidgetItem * _entered;

	QTimer _timer;

	bool _inDrag;

	CContactList & _cContactList;
};

#endif // OWQTUSERTREEEVENTMANAGER_H