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

#ifndef OWQTHISTORYWIDGET_H
#define OWQTHISTORYWIDGET_H

#include <QtCore/QObject>

#include "QtHistoryItem.h"

class HistoryTreeEventManager;

namespace Ui { class HistoryWidget; }

/**
 * History Widget.
 *
 * Shows call/sms history
 *
 * @author Mathieu Stute
 * @author Tanguy Krotoff
 */
class QtHistoryWidget : public QObject {
	Q_OBJECT
public:

	QtHistoryWidget(QWidget * parent);

	~QtHistoryWidget();

	QWidget * getWidget() const;

	void clearHistory();

	void sortHistory();

	void addSMSItem(const QString & text,const QDate & date,
		const QTime & time, const QTime & duration, const QString & name, unsigned id);

	void addOutGoingCallItem(const QString & text,const QDate & date,
		const QTime & time, const QTime & duration, const QString & name, unsigned id);

	void addIncomingCallItem(const QString & text,const QDate & date,
		const QTime & time, const QTime & duration, const QString & name, unsigned id);

	void addChatItem(const QString & text,const QDate & date,
		const QTime & time, const QTime & duration, const QString & name, unsigned id);

	void addMissedCallItem(const QString & text,const QDate & date,
		const QTime & time, const QTime & duration, const QString & name, unsigned id);

	void addRejectedCallItem(const QString & text,const QDate & date,
		const QTime & time, const QTime & duration, const QString & name, unsigned id);

	void editItem(const QString & text,const QDate & date,
		const QTime & time, const QTime & duration, const QString & name, unsigned id);

	void showPopupMenu(const QPoint & point);

	void resizeColumns();

public Q_SLOTS:

	void showSMSCall(bool checked);

	void showOutGoingCall(bool checked);

	void showIncoming(bool checked);

	void showChat(bool checked);

	void showMissedCall(bool checked);

	void showAll(bool checked);

	void itemDoubleClicked(QTreeWidgetItem * item, int column);

	void headerClicked(int logicalIndex);

	void eraseEntry();

	void replayEntry();

Q_SIGNALS:

	void replayItem(QtHistoryItem * item);

	void removeItem(unsigned id);

	void missedCallsSeen();

private Q_SLOTS:

	void itemClicked();

private:

	/**
	 * Code factorization.
	 */
	void addItem(const QString & text, const QIcon & icon, const QDate & date, const QTime & time,
		const QTime & duration, const QString & name, unsigned id, QtHistoryItem::HistoryType type);

	/**
	 * Code factorization.
	 */
	void showItem(const QString & text);

	Ui::HistoryWidget * _ui;

	QWidget * _historyWidget;

	QMenu * _menu;

	QMenu * _popupMenu;

	QtHistoryItem * _currentItem;

	HistoryTreeEventManager * _historyTreeEventManager;
};


class HistoryTreeEventManager : public QObject {
	Q_OBJECT
public:

	HistoryTreeEventManager(QTreeWidget * target, QtHistoryWidget * historyWidget);

private:

	bool eventFilter(QObject * object, QEvent * event);

	QtHistoryWidget * _historyWidget;
};

#endif	//OWQTHISTORYWIDGET_H
