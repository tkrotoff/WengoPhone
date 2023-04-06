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

#ifndef OWCONTACTGROUPMANAGER_H
#define OWCONTACTGROUPMANAGER_H

#include <coipmanager_base/contact/ContactGroupList.h>

#include <coipmanager/coipmanagerdll.h>

#include <QtCore/QMutex>
#include <QtCore/QObject>

class UserProfileManager;

/**
 * Contact group list manager.
 *
 * It accesses ContactGroupList in a thread-safe manner.
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 */
class COIPMANAGER_API ContactGroupManager : public QObject {
	Q_OBJECT
public:

	ContactGroupManager(UserProfileManager & userProfileManager, ContactGroupList & contactGroupList);

	~ContactGroupManager();

	/**
	 * Adds a group.
	 *
	 * Checks if the group is already in the list.
	 *
	 * @return uuid of the added group, uuid of the group if already exists
	 */
	std::string add(const std::string & groupName);

	/**
	 * Removes a group.
	 *
	 * Does not delete contact associated with this group. They are only
	 * removed from the group.
	 *
	 * @return true if actually removed (group is in the group list).
	 */
	bool remove(const std::string & groupId);

	/**
	 * Renames a group if it exists.
	 *
	 * @param groupId uuid of the group to rename
	 * @param name new name
	 * @return true if actually renamed (group is in the group list)
	 */
	bool rename(const std::string & groupId, const std::string & name);

	/**
	 * Adds a Contact to a group.
	 *
	 * @param groupId uuid of the group to add to
	 * @param contactId uuid of the Contact to add
	 * @result true if actually added (contact was not present in group)
	 */
	bool addToGroup(const std::string & groupId, const std::string & contactId);

	/**
	 * Removes a Contact from a group.
	 *
	 * @param groupId uuid of the group to remove from
	 * @param contactId uuid of the Contact to remove
	 * @result true if actually removed (contact was present in group)
	 */
	bool removeFromGroup(const std::string & groupId, const std::string & contactId);

Q_SIGNALS:

	/**
	 * Emitted when a group has been added.
	 */
	void groupAddedSignal(std::string groupId);

	/**
	 * Emitted when a group has been removed.
	 */
	void groupRemovedSignal(std::string groupId);

private:

	/**
	 * @return true if the group with uuid 'groupId' is in ContactGroupList.
	 */
	bool exists(const std::string & groupId) const;

	UserProfileManager & _userProfileManager;

	ContactGroupList & _contactGroupList;

	QMutex * _mutex;
};

#endif	//OWCONTACTGROUPMANAGER_H
