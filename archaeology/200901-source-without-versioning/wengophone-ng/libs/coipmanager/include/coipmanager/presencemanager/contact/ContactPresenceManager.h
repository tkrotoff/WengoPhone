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

#ifndef OWCONTACTPRESENCEMANAGER_H
#define OWCONTACTPRESENCEMANAGER_H

#include <coipmanager_base/EnumUpdateSection.h>
#include <coipmanager_base/imcontact/IMContact.h>

#include <coipmanager/ICoIpManager.h>

#include <QtCore/QObject>

#include <vector>

class ContactManager;
class IContactPresenceManagerPlugin;

/**
 * Contact Presence Management.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API ContactPresenceManager : public ICoIpManager {
	Q_OBJECT
public:

	ContactPresenceManager(CoIpManager & coIpManager);

	virtual ~ContactPresenceManager();

private Q_SLOTS:

	void localIMContactAddedSlot(std::string contactId, IMContact imContact);
	
	void localIMContactRemovedSlot(std::string contactId, IMContact imContact);

	void remoteIMContactUpdatedSlot(IMContact imContact);

	void authorizationRequestedSlot(IMContact imContact, const std::string & message);

	void accountConnectionStateSlot(std::string accountId,
		EnumConnectionState::ConnectionState state);

private:

	virtual void initialize();

	virtual void uninitialize();

	/** Vector of available IContactPresenceManagerPlugin. */
	std::vector<IContactPresenceManagerPlugin *> _iContactPresenceManagerVector;
};

#endif	//OWCONTACTPRESENCEMANAGER_H
