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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#ifndef OWSMSSESSIONMANAGER_H
#define OWSMSSESSIONMANAGER_H

#include "ISMSSessionManagerPlugin.h"

#include <coipmanager/ICoIpSessionManager.h>

class Account;
class SMSSession;
class ISMSSessionPlugin;
class ISMSSessionManagerPlugin;

/**
 * SMS Session Manager.
 *
 * Used to manage classes related to SMS.
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 */
class COIPMANAGER_API SMSSessionManager : public ICoIpSessionManager {
	friend class SMSSession;
public:

	SMSSessionManager(CoIpManager & coIpManager);

	virtual ~SMSSessionManager();

	/**
	 * Creates an SMSSession.
	 *
	 * Caller is responsible for deleting the Session.
	 */
	SMSSession * createSMSSession();

	virtual ISMSSessionManagerPlugin * getICoIpManagerPlugin(const Account & account, const ContactList & contactList) const;

private:

	virtual void initialize();

	virtual void uninitialize();

};

#endif //OWSMSSESSIONMANAGER_H
