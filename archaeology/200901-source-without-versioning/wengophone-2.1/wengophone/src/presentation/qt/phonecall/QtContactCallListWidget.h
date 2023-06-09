/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include <QtCore/QMutex>
#include <QtGui/QWidget>

class PhoneCall;
class ConferenceCall;
class CWengoPhone;
class QtPhoneCall;

namespace Ui { class ContactCallListWidget; }


class QtContactCallListWidget : public QWidget {
	Q_OBJECT
public:

	QtContactCallListWidget(CWengoPhone & cWengoPhone, QWidget * parent);

	~QtContactCallListWidget();

	void addPhoneCall(QtPhoneCall * qtPhoneCall);

	bool hasPhoneCall(PhoneCall * phoneCall);

	bool hasActivePhoneCall();

	QtPhoneCall * takeQtPhoneCall(PhoneCall * phoneCall);

	QtPhoneCall * getFirstQtPhoneCall();

public Q_SLOTS:

	void hangup();

private Q_SLOTS:

	void deleteQtPhoneCall(QObject * qtPhoneCall);

	void slotStartedTalking(QtPhoneCall* phoneCall);

private:

	QWidget * _content;

	CWengoPhone & _cWengoPhone;

	QMutex _mutex;

	enum ConferenceState { CS_CallingFirst, CS_FirstOnHold, CS_Started };
	ConferenceState _conferenceState;

	typedef QList < QtPhoneCall * > QtPhoneCallList;
	QtPhoneCallList _phoneCallList;

	QtPhoneCall * _qtPhoneCall;
};

#endif	//QTCONTACTCALLLISTWIDGET_H
