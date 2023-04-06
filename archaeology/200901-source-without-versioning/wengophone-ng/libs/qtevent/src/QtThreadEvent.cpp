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

#include <qtevent/QtThreadEvent.h>

#include <util/SafeDelete.h>
#include <util/Logger.h>

const int QtThreadEvent::EventValue = QEvent::User + 4;

QtThreadEvent::QtThreadEvent(QObject * object, const char * member,
	QGenericArgument val0,
	QGenericArgument val1,
	QGenericArgument val2,
	QGenericArgument val3,
	QGenericArgument val4,
	QGenericArgument val5,
	QGenericArgument val6,
	QGenericArgument val7,
	QGenericArgument val8,
	QGenericArgument val9)
	: QEvent((QEvent::Type) EventValue) {

	saveMethod(object, member, val0, val1, val2, val3, val4, val5, val6, val7, val8, val9);
}

void QtThreadEvent::saveMethod(QObject * object, const char * member,
	QGenericArgument val0,
	QGenericArgument val1,
	QGenericArgument val2,
	QGenericArgument val3,
	QGenericArgument val4,
	QGenericArgument val5,
	QGenericArgument val6,
	QGenericArgument val7,
	QGenericArgument val8,
	QGenericArgument val9) {

	_object = object;
	_member = member;
	_val0 = val0;
	_val1 = val1;
	_val2 = val2;
	_val3 = val3;
	_val4 = val4;
	_val5 = val5;
	_val6 = val6;
	_val7 = val7;
	_val8 = val8;
	_val9 = val9;
}

QtThreadEvent::~QtThreadEvent() {
}

void QtThreadEvent::callback() {
	bool ret = QMetaObject::invokeMethod(_object, _member.c_str(), Qt::QueuedConnection,
		_val0, _val1, _val2, _val3,
		_val4, _val5, _val6, _val7, _val8, _val9);

	if (!ret) {
		LOG_FATAL("cannot invoke method=" + _object->objectName().toStdString() + "::" + _member.c_str());
	}
}
