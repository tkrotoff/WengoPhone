/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007 Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef OWWENGOSMSSESSIONMANAGER_H
#define OWWENGOSMSSESSIONMANAGER_H

#include <coipmanager/smssessionmanager/ISMSSessionManagerPlugin.h>

/**
 * Wengo implementation of SMS Session Manager.
 *
 * @author Philippe Bernery
 */
class WengoSMSSessionManager : public ISMSSessionManagerPlugin {
public:

	WengoSMSSessionManager(CoIpManager & coIpManager);

	virtual ~WengoSMSSessionManager();

	virtual ISMSSessionPlugin * createISMSSessionPlugin();

	virtual bool canCreateISession(const Account & account, const ContactList & contactList) const;

	virtual IMContactList getValidIMContacts(const Account & account, const ContactList & contactList) const;

};

#endif //OWWENGOSMSSESSIONMANAGER_H
