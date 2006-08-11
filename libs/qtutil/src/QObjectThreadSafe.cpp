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

#include <qtutil/QObjectThreadSafe.h>

#include <QtCore/QCoreApplication>

QObjectThreadSafe::QObjectThreadSafe(QObject * parent)
	: QObject(parent) {

	_blockEvents = false;
}

QObjectThreadSafe::~QObjectThreadSafe() {
}

void QObjectThreadSafe::postEvent(PostEvent * event) {
	QCoreApplication::postEvent(this, event);
}

void QObjectThreadSafe::blockEvents(bool block) {
	_blockEvents = block;
}

bool QObjectThreadSafe::event(QEvent * event) {
	//If events are not blocked via blockEvents()
	if (!_blockEvents) {
		if (event->type() == PostEvent::PostEventValue) {
			PostEvent * e = (PostEvent *) event;
			e->callback();
			return true;
		}
	}
	return QObject::event(event);
}
