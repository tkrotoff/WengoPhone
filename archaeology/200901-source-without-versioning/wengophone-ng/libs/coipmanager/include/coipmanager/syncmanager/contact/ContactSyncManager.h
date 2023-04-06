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

#ifndef OWCONTACTSYNCMANAGER_H
#define OWCONTACTSYNCMANAGER_H

#include <coipmanager_base/EnumUpdateSection.h>
#include <coipmanager_base/contact/Contact.h>

#include <coipmanager/syncmanager/SyncManager.h>

class ContactManager;
class IContactSyncManagerPlugin;
class IIMContactSyncManagerPlugin;

/**
 * Manage Contact data synchronization with external libs.
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_API ContactSyncManager : public SyncManager {
	Q_OBJECT
public:

	ContactSyncManager(CoIpManager & coIpManager);

	virtual ~ContactSyncManager();

private Q_SLOTS:

	void cmContactAddedSlot(std::string contactId);

	void cmContactRemovedSlot(std::string contactId);

	void cmContactUpdatedSlot(std::string contactId, EnumUpdateSection::UpdateSection section);

	void cmIMContactAddedSlot(std::string contactId, IMContact imContact);

	void cmIMContactRemovedSlot(std::string contactId, IMContact imContact);

	void icsContactAddedSlot(Contact contact);

	void icsContactRemovedSlot(Contact contact);

	void icsContactUpdatedSlot(Contact contact);

	void icsIMContactAddedSlot(IMContact imContact);

	void icsIMContactRemovedSlot(IMContact imContact);

	void icsIMContactUpdatedSlot(IMContact imContact);

};

#endif //OWCONTACTSYNCMANAGER_H
