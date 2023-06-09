/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2006-2007  Wengo
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

#include <qtevent/QtPostEvent.h>

#include <qtevent/QtThreadEvent.h>
#include <qtevent/ThreadEventFilter.h>

#include <QtCore/QtCore>

QtPostEvent::QtPostEvent(QObject * receiver) {
	_receiver = receiver;
}

void QtPostEvent::postEvent(QtThreadEvent * threadEvent) {
	QCoreApplication::postEvent(_receiver, threadEvent);
}

QtPostEvent * installQtPostEventFilter(QObject * receiver) {
	ThreadEventFilter * threadEventFilter = new ThreadEventFilter();
	QCoreApplication::instance()->installEventFilter(threadEventFilter);
	return new QtPostEvent(receiver);
}
