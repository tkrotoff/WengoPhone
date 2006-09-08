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

#include "QtPhoneLine.h"

#include <presentation/qt/phonecall/QtPhoneCall.h>
#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/statusbar/QtStatusBar.h>
#include <presentation/qt/QtSystray.h>
#include <presentation/qt/callbar/QtCallBar.h>

#include <control/phoneline/CPhoneLine.h>
#include <control/phonecall/CPhoneCall.h>
#include <control/CWengoPhone.h>

#include <util/Logger.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

QtPhoneLine::QtPhoneLine(CPhoneLine & cPhoneLine)
	: QObjectThreadSafe(NULL),
	_cPhoneLine(cPhoneLine) {

	_activeCPhoneCall = NULL;
	_qtWengoPhone = (QtWengoPhone *) _cPhoneLine.getCWengoPhone().getPresentation();

	stateChangedEvent += boost::bind(&QtPhoneLine::stateChangedEventHandler, this, _1);
	phoneCallCreatedEvent += boost::bind(&QtPhoneLine::phoneCallCreatedEventHandler, this, _1);
	phoneCallClosedEvent += boost::bind(&QtPhoneLine::phoneCallClosedEventHandler, this, _1);

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneLine::initThreadSafe, this));
	postEvent(event);
}

QtPhoneLine::~QtPhoneLine() {
}

void QtPhoneLine::initThreadSafe() {
	//callButton
	SAFE_CONNECT(&_qtWengoPhone->getQtCallBar(), SIGNAL(callButtonClicked()), SLOT(callButtonClicked()));
}

void QtPhoneLine::updatePresentation() {
}

void QtPhoneLine::updatePresentationThreadSafe() {
}

void QtPhoneLine::addPhoneCall(QtPhoneCall * qtPhoneCall) {
}

void QtPhoneLine::stateChangedEventHandler(EnumPhoneLineState::PhoneLineState state) {
	typedef PostEvent1<void (EnumPhoneLineState::PhoneLineState), EnumPhoneLineState::PhoneLineState> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneLine::stateChangedEventHandlerThreadSafe, this, _1), state);
	postEvent(event);
}

void QtPhoneLine::stateChangedEventHandlerThreadSafe(EnumPhoneLineState::PhoneLineState state) {
	_qtWengoPhone->getQtStatusBar().phoneLineStateChanged(state);
	_qtWengoPhone->getQtSystray().phoneLineStateChanged(state);
}

void QtPhoneLine::phoneCallCreatedEventHandler(CPhoneCall & cPhoneCall) {
	typedef PostEvent1<void (CPhoneCall &), CPhoneCall &> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneLine::phoneCallCreatedEventHandlerThreadSafe, this, _1), cPhoneCall);
	postEvent(event);
}

void QtPhoneLine::phoneCallCreatedEventHandlerThreadSafe(CPhoneCall & cPhoneCall) {
	_activeCPhoneCall = &cPhoneCall;
}

void QtPhoneLine::phoneCallClosedEventHandler(CPhoneCall & cPhoneCall) {
	typedef PostEvent1<void (CPhoneCall &), CPhoneCall &> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneLine::phoneCallClosedEventHandlerThreadSafe, this, _1), cPhoneCall);
	postEvent(event);
}

void QtPhoneLine::phoneCallClosedEventHandlerThreadSafe(CPhoneCall & cPhoneCall) {
	if (_activeCPhoneCall == &cPhoneCall) {
		_activeCPhoneCall = NULL;
	}
}

void QtPhoneLine::callButtonClicked() {
	if (_activeCPhoneCall) {
		_activeCPhoneCall->accept();
		_qtWengoPhone->getQtCallBar().setEnabledCallButton(false);
	}
}
