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

#ifndef OWPOSTEVENT_H
#define OWPOSTEVENT_H

#include <util/owutildll.h>

#include <util/NonCopyable.h>

#include <QtCore/QGenericArgument>

class QGenericReturnArgument;
class QObject;

/**
 * Replaces QMetaObject::invokeMethod().
 *
 * Invoke a method from one thread to another.
 *
 * A QObject belongs to a QThread if this QObject has been created inside
 * QThread::run() method. Beware of this otherwise you will make mistakes.
 *
 * Checks if QMetaObject::invokeMethod() succeed or not; if not then
 * an assertion will occur.
 * PostEvent::invokeMethod() always queue the method using Qt::QueuedConnection
 *
 * Example:
 * <pre>
 * PostEvent::invokeMethod(_connectManager, "removeContact",
 * 	Q_ARG(std::string, accountId));
 * //This will call in a 'thread event loop way':
 * //ConnectManager::removeContact(const std::string accountId)
 * </pre>
 *
 * @see QMetaObject::invokeMethod()
 * @author Tanguy Krotoff
 */
class PostEvent : NonCopyable {
public:

	/**
	 * @see QMetaObject::invokeMethod()
	 */
	OWUTIL_API static void invokeMethod(QObject * object, const char * member,
		QGenericReturnArgument ret,
		QGenericArgument val0 = QGenericArgument(0),
		QGenericArgument val1 = QGenericArgument(),
		QGenericArgument val2 = QGenericArgument(),
		QGenericArgument val3 = QGenericArgument(),
		QGenericArgument val4 = QGenericArgument(),
		QGenericArgument val5 = QGenericArgument(),
		QGenericArgument val6 = QGenericArgument(),
		QGenericArgument val7 = QGenericArgument(),
		QGenericArgument val8 = QGenericArgument(),
		QGenericArgument val9 = QGenericArgument());

	/**
	 * @see QMetaObject::invokeMethod()
	 */
	OWUTIL_API static void invokeMethod(QObject * object, const char * member,
		QGenericArgument val0 = QGenericArgument(0),
		QGenericArgument val1 = QGenericArgument(),
		QGenericArgument val2 = QGenericArgument(),
		QGenericArgument val3 = QGenericArgument(),
		QGenericArgument val4 = QGenericArgument(),
		QGenericArgument val5 = QGenericArgument(),
		QGenericArgument val6 = QGenericArgument(),
		QGenericArgument val7 = QGenericArgument(),
		QGenericArgument val8 = QGenericArgument(),
		QGenericArgument val9 = QGenericArgument());
};

#endif	//OWPOSTEVENT_H
