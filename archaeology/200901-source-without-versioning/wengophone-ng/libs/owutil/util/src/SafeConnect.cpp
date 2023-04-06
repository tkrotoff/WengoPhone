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

#include <util/SafeConnect.h>

#include <util/Logger.h>

#include <QtCore/QObject>
#include <QtCore/QMetaType>
#include <QtCore/QMetaMethod>

/* Algorithm taken from qobject.cpp */
void checkQueuedConnectionTypes(const QList<QByteArray> & typeNames) {
	for (int i = 0; i < typeNames.count(); ++i) {
		const QByteArray typeName = typeNames.at(i);
		int type = -1;
		if (typeName.endsWith('*')) {
			type = QMetaType::VoidStar;
		} else {
			type = QMetaType::type(typeName);
		}

		if (!type) {
			LOG_FATAL("cannot queue arguments of type=" + std::string(typeName.constData()));
		}
	}
}

void SafeConnect::connect(const QObject * sender, const char * signal, const QObject * receiver, const char * method, Qt::ConnectionType type) {
	//Check that each parameters from the signal are queueable
	//if not, then throw an assert
	//This has been tested using Qt 4.1.5, this version of QObject::connect()
	//does not return false if Qt::ConnectionType != Qt::QueuedConnection
	//check line 2211 inside QObject::connect() from qobject.cpp
	const QMetaObject * smeta = sender->metaObject();
	QByteArray tmpSignalName = QMetaObject::normalizedSignature(signal);
	const char * signalName = tmpSignalName.constData() + 1;
	int signalIndex = smeta->indexOfSignal(signalName);
	checkQueuedConnectionTypes(smeta->method(signalIndex).parameterTypes());
	////

	if (!QObject::connect(sender, signal, receiver, method, type)) {
		LOG_FATAL("cannot connect sender=" + sender->objectName().toStdString() +
				" signal=" + std::string(signal) +
				" receiver=" + receiver->objectName().toStdString() +
				" method=" + std::string(method));
	}
}

void SafeConnect::disconnect(const QObject * sender, const char * signal, const QObject * receiver, const char * method) {
	if (!QObject::disconnect(sender, signal, receiver, method)) {
		LOG_FATAL("cannot disconnect sender=" + sender->objectName().toStdString() +
				" signal=" + std::string(signal) +
				" receiver=" + receiver->objectName().toStdString() +
				" method=" + std::string(method));
	}
}
