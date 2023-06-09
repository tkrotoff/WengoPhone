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

#include "QtContactCallListWidget.h"
#include "QtContactCallList.h"
#include "QtPhoneCall.h"

#include <control/phonecall/CPhoneCall.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <model/phonecall/ConferenceCall.h>
#include <model/phonecall/PhoneCall.h>
#include <model/profile/UserProfile.h>

QtContactCallListWidget::QtContactCallListWidget(CWengoPhone & cWengoPhone, QWidget * parent, Qt::WFlags f)
	: QWidget(parent, f), _cWengoPhone(cWengoPhone) {

	_layout = new QGridLayout(this);
	_conferenceCall = NULL;
	_listWidget = new QtContactCallList(this);

	// Setup the list widget
	_layout->addWidget(_listWidget, 0, 0);
	setAttribute(Qt::WA_DeleteOnClose, true);
}

void QtContactCallListWidget::addPhoneCall(QtPhoneCall * qtPhoneCall) {
	connect(qtPhoneCall, SIGNAL(startConference(PhoneCall *, PhoneCall *)),
		SLOT(startConference(PhoneCall *, PhoneCall *)));
	connect(qtPhoneCall, SIGNAL(stopConference()), SLOT(stopConference()));
	//connect(qtPhoneCall, SIGNAL(callRejected()), SLOT(callRejected()));
	connect(_listWidget, SIGNAL(lastQtPhoneCallClosed()),SLOT(callRejected()));
	_listWidget->addPhoneCall(qtPhoneCall);
}

void QtContactCallListWidget::startConference(PhoneCall * sourceCall, PhoneCall * targetCall) {
	startConferenceSignal(sourceCall, targetCall);
}

void QtContactCallListWidget::stopConference() {

	if (!_conferenceCall)
		return;

	QtPhoneCall * phone;

	phone = _listWidget->getPhoneCallList() [0];
	_conferenceCall->removePhoneCall(phone->getCPhoneCall().getPhoneCall());

	phone = _listWidget->getPhoneCallList() [1];
	_conferenceCall->removePhoneCall(phone->getCPhoneCall().getPhoneCall());

	delete _conferenceCall;
	_conferenceCall = NULL;
}

void QtContactCallListWidget::hangup() {
	QMutexLocker locker(& _mutex);
	QtPhoneCall * phone;

	QtContactCallList::QtPhoneCallList phoneCallList;
	QtContactCallList::QtPhoneCallList::iterator iter;

	phoneCallList = _listWidget->getPhoneCallList();

	for (iter = phoneCallList.begin(); iter != phoneCallList.end(); iter++) {
		phone = (* iter);

		phone->rejectActionTriggered(true);
	}
	_listWidget->clearCalls();
	// phone = _listWidget->getPhoneCallList()[0];
	//phone->rejectActionTriggered(true);
}

void QtContactCallListWidget::callRejected() {

//	if (_listWidget->count() == 0)
		close();
}

bool QtContactCallListWidget::hasPhoneCall(PhoneCall * phoneCall) {
	return _listWidget->hasPhoneCall(phoneCall);
}

QtPhoneCall * QtContactCallListWidget::takeQtPhoneCall(PhoneCall * phoneCall) {
	return _listWidget->takeQtPhoneCall(phoneCall);
}

QtPhoneCall * QtContactCallListWidget::getFirstQtPhoneCall(){
    if (_listWidget->count() == 0)
        return NULL;
    return _listWidget->getPhoneCallList()[0];
}

bool QtContactCallListWidget::hasActivePhoneCall(){
    return _listWidget->hasActivePhoneCall();
}
