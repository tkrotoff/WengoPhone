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

#ifndef OWQTPOSTEVENT_H
#define OWQTPOSTEVENT_H

#include <qtevent/qteventdll.h>

#include <util/NonCopyable.h>

class QtThreadEvent;
class QObject;

/**
 * Qt implementation for IPostEvent.
 *
 * Class Event backend for Qt
 *
 * @see event/Event
 * @see event/IPostEvent
 * @author Tanguy Krotoff
 */
class QtPostEvent : NonCopyable {
public:

	QTEVENT_API QtPostEvent(QObject * receiver);

	QTEVENT_API void postEvent(QtThreadEvent * threadEvent);

private:

	QObject * _receiver;
};

/**
 * Installs filter for Qt post event (=thread event)
 *
 * @param receiver QObject where to install the thread event filter
 */
QTEVENT_API QtPostEvent * installQtPostEventFilter(QObject * receiver);

#endif	//OWQTPOSTEVENT_H
