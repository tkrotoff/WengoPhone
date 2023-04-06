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

#ifndef OWPURPLECONTACTPRESENCEMANAGER_H
#define OWPURPLECONTACTPRESENCEMANAGER_H

#include <coipmanager/presencemanager/contact/IContactPresenceManagerPlugin.h>

extern "C" {
#include <libpurple/account.h>
}

#include <QtCore/QMutex>

#include <map>
#include <string>

typedef struct AuthRequest_s AuthRequest;

/**
 * LibPurple Contact Presence Management.
 *
 * @author Philippe Bernery
 * @author Julien Bossart
 */
class PurpleContactPresenceManager : public IContactPresenceManagerPlugin {
public:

	PurpleContactPresenceManager(CoIpManager & coIpManager);

	~PurpleContactPresenceManager();

	virtual void blockContact(const IMContact & imContact);

	virtual void unblockContact(const IMContact & imContact);

	virtual void authorizeContact(const IMContact & imContact,
		bool authorized, const std::string & message);

	static void addAuthRequest(const std::string &contactId,
		PurpleAccountRequestAuthorizationCb acceptCbk,
		PurpleAccountRequestAuthorizationCb denyCbk,
		void *userData);
	static AuthRequest * getAuthRequest(const std::string contactId);
	static bool blockContactCbk(void * data);
	static bool unblockContactCbk(void * data);
	static bool authorizeContactCbk(void * data);
	static void * authorizationRequestCbk(PurpleAccount * pAccount,
		const char * remoteUser, const char * id, const char * alias, const char * message,
		gboolean onList, PurpleAccountRequestAuthorizationCb authorizeCbk,
		PurpleAccountRequestAuthorizationCb denyCbk, void * userData);

private:
	static QMutex * _mutex;
	static PurpleContactPresenceManager * _instance;

	static std::map<const std::string, AuthRequest *> _authRequestMap;
};

#endif	//OWPURPLECONTACTPRESENCEMANAGER_H
