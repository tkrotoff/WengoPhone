/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef PRESENCEPOPUPMENU_H
#define PRESENCEPOPUPMENU_H

#include "PresenceStatus.h"

#include <qpopupmenu.h>
#include <qstring.h>
#include <qpixmap.h>

#include <vector>

class UserDefinePresenceStatusWidget;
class PhoneLineEdit;

/**
 * Popup menu for the presence of a Contact.
 *
 * Presence of the Contact: online, offline, away...
 *
 * @author Tanguy Krotoff
 */
class PresencePopupMenu : public QPopupMenu {
	Q_OBJECT
public:

	PresencePopupMenu(QWidget * parent);

	~PresencePopupMenu();

	/**
	 * Emits the signal such that the default status is selected.
	 */
	void defaultStatus();

	/**
	 * Publish user presence to offline.
	 *
	 * Used when the user quits WengoPhone.
	 */
	void publishOfflineStatus();

	const PresenceStatus & getCurrentPresenceStatus() const {
		return _myPresenceStatus;
	}

signals:

	/**
	 * Presence of the user has been changed.
	 *
	 * @param presenceStatus new status of the user
	 */
	void presenceChanged(const PresenceStatus & presenceStatus);

private slots:

	/**
	 * The user clicked on an item from the menu.
	 *
	 * @param index index inside the PresenceStatusList
	 */
	void presenceStatusSelected(int index);

	void userDefineTextReturnPressed();

private:

	PresencePopupMenu(const PresencePopupMenu &);
	PresencePopupMenu & operator=(const PresencePopupMenu &);

	class PublishPresenceTask;
	friend class PublishPresenceTask;

	void publishMyPresenceStatus();

	/**
	 * Adds a menu item to the menu
	 *
	 * @param index index inside the PresenceStatusList
	 */
	void addPresenceStatus(int index);

	/**
	 * Defines a list of PresenceStatus.
	 */
	typedef std::vector<PresenceStatus> PresenceStatusList;

	/**
	 * List of different status: online, offline, away...
	 */
	PresenceStatusList _presenceStatusList;

	PhoneLineEdit * _userDefineText;

	/**
	 * PresenceStatus of the WengoPhone user.
	 */
	PresenceStatus _myPresenceStatus;
};

#endif	//PRESENCEPOPUPMENU_H
