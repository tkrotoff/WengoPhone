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

#ifndef QTCONTACTCALLLISTWIDGET_H
#define QTCONTACTCALLLISTWIDGET_H

#include <control/CWengoPhone.h>


#include <QtGui>

class QtContactCallList;
class PhoneCall;
class QtPhoneCall;
class ConferenceCall;

class QtContactCallListWidget : public QWidget
{
	Q_OBJECT

public:

	QtContactCallListWidget (CWengoPhone & cWengoPhone,QWidget * parent = 0, Qt::WFlags f = 0);

	void addPhoneCall(QtPhoneCall * qtPhoneCall);

	bool hasPhoneCall(PhoneCall * phoneCall);

public Q_SLOTS:

	void startConference(QtPhoneCall * qtPhoneCall);

	void stopConference();

	void hangup();

    void callRejected();



protected:

	QGridLayout * _layout;

	QtContactCallList * _listWidget;

	ConferenceCall * _conferenceCall;

	CWengoPhone & _cWengoPhone;

    QMutex _mutex;
};

#endif // QTCONTACTCALLLISTWIDGET_H
