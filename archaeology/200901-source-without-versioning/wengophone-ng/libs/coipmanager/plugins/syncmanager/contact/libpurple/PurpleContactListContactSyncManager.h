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

#ifndef OWPURPLECONTACTLISTCONTACTSYNC_H
#define OWPURPLECONTACTLISTCONTACTSYNC_H

#include <coipmanager/syncmanager/contact/IIMContactSyncManagerPlugin.h>

extern "C" {
#include <libpurple/connection.h>
#include <libpurple/blist.h>
}

/**
 * LibPurple Contact data synchronization implementations.
 * This module also retrieve presence state of IMContacts.
 *
 * @author Philippe Bernery
 * @author Julien Bossart
 */
class PurpleContactListContactSyncManager : public IIMContactSyncManagerPlugin {
public:

	PurpleContactListContactSyncManager(CoIpManager & coIpManager);

	virtual ~PurpleContactListContactSyncManager();

	virtual void initialize();

	/**
	 * @name LibPurple callbacks.
	 * @{
	 */
	static void newListCbk(PurpleBuddyList * blist);
	static void newNodeCbk(PurpleBlistNode * node);
	static void showCbk(PurpleBuddyList * list);
	static void updateCbk(PurpleBuddyList * list, PurpleBlistNode * node);
	static void removeCbk(PurpleBuddyList * list, PurpleBlistNode * node);
	static void destroyCbk(PurpleBuddyList * list);
	static void setVisibleCbk(PurpleBuddyList * list, gboolean show);
	static void requestAddBuddyCbk(PurpleAccount * account, const char * username,
		const char * group, const char * alias);
	static void requestAddChatCbk(PurpleAccount * account, PurpleGroup * group,
		const char * alias, const char * name);
	static void requestAddGroupCbk();
	/**
	 * @}
	 */

	virtual void add(const IMContact & imContact);

	virtual void remove(const IMContact & imContact);

	virtual void update(const IMContact & imContact);

private:

	static void newBuddyAdded(PurpleBuddy * pBuddy);
	static void newGroupAdded(PurpleGroup * pGroup);
	static void updateBuddy(PurpleBuddyList * list, PurpleBuddy * pBuddy);
	static void updateBuddyIcon(PurpleBuddy * pbuddy);

	void initContactEvent();

	static PurpleContactListContactSyncManager * _instance;
};

#endif	//OWPURPLECONTACTLISTCONTACTSYNC_H
