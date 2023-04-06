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

#ifndef OWQTTHREADEVENT_H
#define OWQTTHREADEVENT_H

#include <qtevent/qteventdll.h>

#include <QtCore/QEvent>
#include <QtCore/QGenericArgument>

#include <string>

/**
 * ThreadEvent for Qt.
 *
 * Used when sending a Qt event from another thread than the Qt thread.
 * This event permits thread safety.
 *
 * @see QCoreApplication::postEvent()
 * @author Tanguy Krotoff
 */
class QtThreadEvent : public QEvent {
public:

	/** QtThreadEvent is a QEvent with a specific value. */
	QTEVENT_API static const int EventValue;

	QTEVENT_API QtThreadEvent(QObject * object, const char * member,
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

	QTEVENT_API virtual ~QtThreadEvent();

	QTEVENT_API void callback();

private:

	void saveMethod(QObject * object, const char * member,
		QGenericArgument val0,
		QGenericArgument val1,
		QGenericArgument val2,
		QGenericArgument val3,
		QGenericArgument val4,
		QGenericArgument val5,
		QGenericArgument val6,
		QGenericArgument val7,
		QGenericArgument val8,
		QGenericArgument val9);

	QObject * _object;
	//std::string since we want a copy of const char * member
	//and not a pointer on another (case const char * _member)
	std::string _member;
	QGenericArgument _val0;
	QGenericArgument _val1;
	QGenericArgument _val2;
	QGenericArgument _val3;
	QGenericArgument _val4;
	QGenericArgument _val5;
	QGenericArgument _val6;
	QGenericArgument _val7;
	QGenericArgument _val8;
	QGenericArgument _val9;
};

#endif	//OWQTTHREADEVENT_H
