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

#ifndef OWNOTIFICATION_H
#define OWNOTIFICATION_H

#include <cutil/global.h>

#ifdef OS_WINDOWS
// Prevent a compilation error on Windows with MSVC 8
#include <windows.h>
#endif

#include <coipmanager/coipmanagerdll.h>
#include <coipmanager/notification/EnumNotificationType.h>

#include <util/Singleton.h>

#include <QtCore/QObject>

/**
 * Notification center for CoIpManager.
 *
 * This class emits events to tell when session are created, destroyed,
 * changed. This class is used to launch action against some events (play
 * a sound when a contact is connected, launch a call when connected, ...).
 * It is used only for notification about Session for now.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API Notification : public QObject, public Singleton<Notification> {
	Q_OBJECT
	friend class Singleton<Notification>;
public:

	/**
	 * Post an event.
	 */
	void postNotification(EnumNotificationType::NotificationType type, const std::string & objectId);
	
Q_SIGNALS:

	/**
	 * Emitted when a new notification is posted.
	 *
	 * @param sender this class
	 * @param type type of notification
	 * @param objectId id of the notification subject
	 */
	void notificationSignal(EnumNotificationType::NotificationType type, std::string objectId);

private:

	Notification();

};

#endif //OWNOTIFICATION_H
