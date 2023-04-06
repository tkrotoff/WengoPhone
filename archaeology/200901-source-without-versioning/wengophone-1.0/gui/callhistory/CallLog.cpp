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

#include "CallLog.h"

#include <qpixmap.h>
#include <qobject.h>

#include <iostream>
#include <cassert>
using namespace std;

CallLog::CallLog() {
	_duration = 0;
	_callDateTime = QDateTime::currentDateTime();
	_timer = 0;
	_contactName = "";
	_phoneNumber = "";
}

CallLog::CallLog(const CallLog & callLog) {
	_duration = callLog._duration;
	_callDateTime = callLog._callDateTime;
	_timer = callLog._timer;
}

CallLog::~CallLog() {
}

void CallLog::setType(const QString & type) {
	if (type == "CallIncoming") {
		setType(CallIncoming);
	} else if (type == "CallOutgoing") {
		setType(CallOutgoing);
	} else if (type == "CallMissed") {
		setType(CallMissed);
	} else if (type == "SmsIncoming") {
		setType(SmsIncoming);
	} else if (type == "SmsOutgoing") {
		setType(SmsOutgoing);
	} else {
		assert(NULL && "Type unknown");
	}
}

void CallLog::startTimer() {
	_timer = QDateTime::currentDateTime(Qt::UTC).toTime_t();
}

void CallLog::stopTimer() {
	_duration = QDateTime::currentDateTime(Qt::UTC).toTime_t() - _timer;
}

QTime CallLog::getDuration() const {
	QTime time;
	return time.addSecs(_duration);
}

void CallLog::setDuration(unsigned long duration) {
	_duration = duration;
}

QString CallLog::getTypeStringTranslated() const {
	switch(_type) {
	case CallIncoming:
		return QObject::tr("CallIncoming");

	case CallOutgoing:
		return QObject::tr("CallOutgoing");

	case CallMissed:
		return QObject::tr("CallMissed");

	case SmsIncoming:
		return QObject::tr("SmsIncoming");

	case SmsOutgoing:
		return QObject::tr("SmsOutgoing");

	default:
		assert(NULL && "Type unknown");
		//No warning from the compiler
		return "";
	}
}

QString CallLog::getTypeString() const {
	switch(_type) {
	case CallIncoming:
		return "CallIncoming";

	case CallOutgoing:
		return "CallOutgoing";

	case CallMissed:
		return "CallMissed";

	case SmsIncoming:
		return "SmsIncoming";

	case SmsOutgoing:
		return "SmsOutgoing";

	default:
		assert(NULL && "Type unknown");
		//No warning from the compiler
		return "";
	}
}

QPixmap CallLog::getTypeIcon() const {
	if (_type == CallIncoming || _type == SmsIncoming) {
		return QPixmap::fromMimeSource("call_incoming.png");
	} else if (_type == CallOutgoing || _type == SmsOutgoing) {
		return QPixmap::fromMimeSource("call_outgoing.png");
	} else if (_type == CallMissed) {
		return QPixmap::fromMimeSource("call_missed.png");
	} else {
		assert(NULL && "Type unknown");
		//No warning from the compiler
		return QPixmap();
	}
}
