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

#ifndef CALLHISTORYWIDGET_H
#define CALLHISTORYWIDGET_H

#include "callhistory/CallHistory.h"

#include <observer/Observer.h>

#include <qobject.h>

class CallLog;
class Subject;
class QWidget;
class QListView;
class QListViewItem;
class QPoint;

/**
 * Gui widget that shows the history of calls.
 *
 * Design Pattern Observer.
 *
 * @see CallLog
 * @author Tanguy Krotoff
 */
class CallHistoryWidget : public QObject, public Observer {
	Q_OBJECT
public:

	CallHistoryWidget(CallHistory & callHistory, QWidget * parent);

	~CallHistoryWidget();

	/**
	 * Gets the low-level gui widget.
	 *
	 * @return the low-level gui widget
	 */
	QWidget * getWidget() const {
		return _callHistoryWidget;
	}

	/**
	 * Callback, called each time a Subject has changed.
	 *
	 * @param subject Subject that has been changed
	 * @param event Event
	 * @see Observer::update();
	 */
	virtual void update(const Subject & subject, const Event & event);

private slots:

	void callContact(QListViewItem * item, const QPoint & pos, int column);

	void deletePressed();

private:

	CallHistoryWidget(const CallHistoryWidget &);
	CallHistoryWidget & operator=(const CallHistoryWidget &);

	void addCallLog(const CallLog & callLog);

	void removeCallLog(const QString & typeStringTranslated, const QString & dateTime,
			const QString & contactName, const QString & duration, const QString & phoneNumber);

	/**
	 * Low-level gui widget of this class.
	 */
	QWidget * _callHistoryWidget;

	/**
	 * Graphical list of CallLog.
	 */
	QListView * _callHistoryListView;

	static const int LISTVIEW_COLUMN_ICON;

	static const int LISTVIEW_COLUMN_DATETIME;

	static const int LISTVIEW_COLUMN_FULL_NAME;

	static const int LISTVIEW_COLUMN_DURATION;

	static const int LISTVIEW_COLUMN_PHONE_NUMBER;

	static const int LISTVIEW_COLUMN_SMS_MESSAGE;
};

#endif	//CALLHISTORYWIDGET_H
