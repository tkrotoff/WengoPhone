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

#include <util/PostEvent.h>

#include <util/Logger.h>

#include <QtCore/QMetaObject>

#include <string>

void PostEvent::invokeMethod(QObject * object, const char * member,
	QGenericReturnArgument ret,
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

	bool result = QMetaObject::invokeMethod(object, member, ret,
		val0, val1, val2, val3, val4, val5, val6, val7, val8, val9);

	if (!result) {
		LOG_FATAL("cannot invoke method=" + object->objectName().toStdString() + "::" + std::string(member));
	}
}

void PostEvent::invokeMethod(QObject * object, const char * member,
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

	bool result = QMetaObject::invokeMethod(object, member,
		val0, val1, val2, val3, val4, val5, val6, val7, val8, val9);

	if (!result) {
		LOG_FATAL("cannot invoke method=" + object->objectName().toStdString() + "::" + std::string(member));
	}
}
