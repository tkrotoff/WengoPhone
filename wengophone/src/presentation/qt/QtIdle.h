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

#ifndef QTIDLE_H
#define QTIDLE_H

#include <idle/Idle.h>

#include <qtutil/QObjectThreadSafe.h>

class Settings;
class UserProfile;

/**
 * Detects idle status.
 *
 * @see Idle
 * @author Tanguy Krotoff
 */
class QtIdle : public QObjectThreadSafe {
	Q_OBJECT
public:

	QtIdle(UserProfile & userProfile, QObject * parent);

private:

	void initThreadSafe() { }

	void configChangedEventHandler(Settings & sender, const std::string & key);

	void configChangedEventHandlerThreadSafe(Settings & sender, const std::string & key);

	/**
	 * Idle status has changed, IMAccount presence state should be changed.
	 */
	void idleStatusChangedEventHandler(Idle & sender, Idle::Status status);

	UserProfile & _userProfile;

	Idle * _idle;
};

#endif	//QTIDLE_H
