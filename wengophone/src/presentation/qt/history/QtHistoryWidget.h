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

#ifndef QTHISTORYWIDGET_H
#define QTHISTORYWIDGET_H

#include <QtGui>
#include <QObject>

#include "QtHistoryItem.h"

#include <control/history/CHistory.h>

/**
 * History Widget.
 *
 * @author Mr K
 * @author Mathieu Stute
 */
class QtHistoryWidget : public QWidget {

	Q_OBJECT

public:

	QtHistoryWidget ( QWidget * parent = 0, Qt::WFlags f = 0);

	void clearHistory();

	void addSMSItem(const QString & text,const QDate & date, 
		const QTime & time, const QTime & duration, const QString & name, unsigned int id);

	void addOutGoingCallItem(const QString & text,const QDate & date,
		const QTime & time, const QTime & duration, const QString & name, unsigned int id);

	void addIncomingCallItem(const QString & text,const QDate & date, 
		const QTime & time, const QTime & duration, const QString & name, unsigned int id);

	void addChatItem(const QString & text,const QDate & date, 
		const QTime & time, const QTime & duration, const QString & name, unsigned int id);

	void addMissedCallItem(const QString & text,const QDate & date, 
		const QTime & time, const QTime & duration, const QString & name, unsigned int id);

	void addRejectedCallItem(const QString & text,const QDate & date, 
		const QTime & time, const QTime & duration, const QString & name, unsigned int id);

	void editItem(const QString & text,const QDate & date, 
		const QTime & time, const QTime & duration, const QString & name, unsigned int id);
	
	void showPopupMenu(const QPoint & point);
	
public Q_SLOTS:

	void showSMSCall(bool checked);

	void showOutGoingCall(bool checked);

	void showIncoming(bool checked);

	void showChat(bool checked);

	void showMissedCall(bool checked);

	void showAll(bool checked);

	void itemDoubleClicked ( QTreeWidgetItem * item, int column );

	void headerClicked(int logicalIndex);
	
	void eraseEntry();
	
	void replayEntry();

Q_SIGNALS:
	
	void replayItem( QtHistoryItem * item );
	
	void removeItem( unsigned int id );

protected:

	QTreeWidget * _treeWidget;

	QHeaderView * _header;

	QMenu * _menu;

	QMenu * _popupMenu;
	
	QtHistoryItem * _currentItem;
};


class HistoryTreeEventManager : public QObject {
	Q_OBJECT
public:
	HistoryTreeEventManager(QTreeWidget * target, QtHistoryWidget * historyWidget);
	
protected:
	bool eventFilter(QObject *obj, QEvent *event);
	
private:
	QtHistoryWidget * _historyWidget;
};

#endif
