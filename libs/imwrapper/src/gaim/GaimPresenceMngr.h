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

#ifndef GAIMPRESENCEMNGR_H
#define GAIMPRESENCEMNGR_H

#include <list>

#include "GaimIMPresence.h"
extern "C" {
#include "gaim/blist.h"
}

/**
 *
 * @ingroup model
 * @author Julien Bossart
 */
class GaimPresenceMngr
{
public:

	static GaimPresenceMngr *getInstance();

	/* ********** GaimBuddyListCallback *********** */
	static void NewListCbk(GaimBuddyList *blist);
	static void NewNodeCbk(GaimBlistNode *node);
	static void ShowCbk(GaimBuddyList *list);
	static void UpdateCbk(GaimBuddyList *list, GaimBlistNode *node);
	static void RemoveCbk(GaimBuddyList *list, GaimBlistNode *node);
	static void DestroyCbk(GaimBuddyList *list);
	static void SetVisibleCbk(GaimBuddyList *list, gboolean show);
	static void RequestAddBuddyCbk(GaimAccount *account, const char *username,
									const char *group, const char *alias);
	static void RequestAddChatCbk(GaimAccount *account, GaimGroup *group,
									const char *alias, const char *name);
	static void RequestAddGroupCbk(void);
	/* ******************************************** */


	static GaimIMPresence *AddIMPresence(IMAccount &account);
	void RemoveIMPresence(IMAccount &account);

private:

	GaimPresenceMngr();
	static GaimPresenceMngr *StaticInstance;

	static void UpdateBuddy(GaimBuddyList *list, GaimBlistNode *node);

	/* ********** PRESENCE_LIST *********** */
	static std::list<GaimIMPresence *> _GaimIMPresenceList;
	typedef std::list<GaimIMPresence *>::iterator GaimIMPresenceIterator;
	/* ********** PRESENCE_LIST *********** */
	static GaimIMPresence *FindIMPresence(IMAccount &account);
};

#endif	//GAIMPRESENCEMNGR_H
