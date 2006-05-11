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

#include "QtPhoneCall.h"
#include "QtContactCallList.h"
#include <model/phonecall/PhoneCall.h>
#include <control/phonecall/CPhoneCall.h>

QtContactCallList::QtContactCallList(QWidget * parent)
	: QScrollArea(parent) {

	_widget = new QWidget(this);

	_widgetLayout = new QVBoxLayout(_widget);

	setWidget(_widget);

	setWidgetResizable(true);
}

void QtContactCallList::addPhoneCall(QtPhoneCall * qtPhoneCall) {
	QMutexLocker locker(& _mutex);
	qtPhoneCall->getWidget()->setParent(_widget);
	_widgetLayout->addWidget(qtPhoneCall->getWidget());
	connect(qtPhoneCall, SIGNAL(deleteMe(QtPhoneCall *)), SLOT(deleteQtPhoneCall(QtPhoneCall *)));
	_phoneCallList.append(qtPhoneCall);
}

QtContactCallList::QtPhoneCallList & QtContactCallList::getPhoneCallList() {
	return _phoneCallList;
}

void QtContactCallList::deleteQtPhoneCall(QtPhoneCall * qtPhoneCall) {
	QMutexLocker locker(& _mutex);
	QtPhoneCallList::iterator iter;
	QtPhoneCall * tmp = NULL;
	for (iter = _phoneCallList.begin(); iter != _phoneCallList.end(); iter++) {
		tmp = (* iter);
		if (tmp == qtPhoneCall) {
			_phoneCallList.erase(iter);
			break;
		}
	}
	qtPhoneCall->deleteLater();
}

bool QtContactCallList::hasPhoneCall(PhoneCall * phoneCall) {
	QMutexLocker locker(& _mutex);
	QtPhoneCallList::iterator iter;
	QtPhoneCall * tmp = NULL;
	for (iter = _phoneCallList.begin(); iter != _phoneCallList.end(); iter++) {
		tmp = (* iter);
		if (tmp->getCPhoneCall().getPhoneCall().getCallId() == phoneCall->getCallId()) {
			return true;
		}
	}
	return false;
}

void QtContactCallList::clearCalls() {
	QMutexLocker locker(& _mutex);
	_phoneCallList.clear();
}

int QtContactCallList::count() {
	QMutexLocker locker(& _mutex);
	return _phoneCallList.size();
}

QtPhoneCall * QtContactCallList::takeQtPhoneCall(PhoneCall * phoneCall) {
	QMutexLocker locker(& _mutex);

	QtPhoneCallList::iterator iter;
	for (iter = _phoneCallList.begin(); iter != _phoneCallList.end(); iter++) {

		if ((* iter)->getCPhoneCall().getPhoneCall().getPeerSipAddress().getUserName() ==
				phoneCall->getPeerSipAddress().getUserName()) {
			_widgetLayout->removeWidget((* iter)->getWidget());
			QtPhoneCall * qtphoneCall = (* iter);
			_phoneCallList.erase(iter);
			return qtphoneCall;
		}
	}
	return NULL;
}
