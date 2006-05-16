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

#ifndef QTCONTACTCALLLIST_H
#define QTCONTACTCALLLIST_H

#include <QtGui>

class QtPhoneCall;
class PhoneCall;

class QtContactCallList : public QScrollArea {
	Q_OBJECT
public:

	typedef QList < QtPhoneCall * > QtPhoneCallList;

	QtContactCallList(QWidget * parent = 0);

	void addPhoneCall(QtPhoneCall * qtPhoneCall);

	bool hasPhoneCall(PhoneCall * phoneCall);

	bool hasActivePhoneCall();

	QtPhoneCallList & getPhoneCallList();

	QtPhoneCall * takeQtPhoneCall(PhoneCall * phoneCall);

	void clearCalls();

	int count();

public Q_SLOTS:

	void deleteQtPhoneCall(QtPhoneCall * qtPhoneCall);

protected:

	QWidget * _widget;

	QVBoxLayout * _widgetLayout;

	QtPhoneCallList _phoneCallList;

	QMutex _mutex;
};

#endif // QTCONTACTCALLLIST_H
