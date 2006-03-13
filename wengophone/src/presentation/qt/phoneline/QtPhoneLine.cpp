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

#include "QtPhoneLine.h"

#include "presentation/qt/phonecall/QtPhoneCall.h"
#include "control/phoneline/CPhoneLine.h"

#include <WidgetFactory.h>
#include <Object.h>

#include <QtGui>

QtPhoneLine::QtPhoneLine(CPhoneLine & cPhoneLine)
	: QObjectThreadSafe(),
	_cPhoneLine(cPhoneLine) {

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneLine::initThreadSafe, this));
	postEvent(event);
}

void QtPhoneLine::initThreadSafe() {
	_phoneLineWidget = WidgetFactory::create(":/forms/phoneline/PhoneLineWidget.ui", NULL);

	_stateLabel = Object::findChild<QLabel *>(_phoneLineWidget, "stateLabel");
}

void QtPhoneLine::updatePresentation() {
}

void QtPhoneLine::updatePresentationThreadSafe() {
}

void QtPhoneLine::addPhoneCall(QtPhoneCall * qtPhoneCall) {
	//_stateLabel->setText(qtPhoneCall->);
}

void QtPhoneLine::phoneLineStateChangedEvent(PhoneLineState state, int lineId) {
	typedef PostEvent2<void (PhoneLineState, int), PhoneLineState, int> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneLine::phoneLineStateChangedEventThreadSafe, this, _1, _2), state, lineId);
	postEvent(event);
}

void QtPhoneLine::phoneLineStateChangedEventThreadSafe(PhoneLineState state, int lineId) {
	_stateLabel->setText(QString::number(state));
}
