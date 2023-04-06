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

#ifndef OWACCOUNTPRESENCEMANAGER_H
#define OWACCOUNTPRESENCEMANAGER_H

#include <coipmanager_base/EnumUpdateSection.h>
#include <coipmanager/ICoIpManager.h>

#include <vector>

class AccountManager;
class IAccountPresenceManagerPlugin;

/**
 * Account Presence Management.
 *
 * @author Philippe Bernery
 */
class AccountPresenceManager : public ICoIpManager {
	Q_OBJECT
public:

	COIPMANAGER_API AccountPresenceManager(CoIpManager & coIpManager);

	COIPMANAGER_API virtual ~AccountPresenceManager();

private Q_SLOTS:

	/**
	 * @see AccountManager::accountUpdatedSignal
	 */
	void accountUpdatedSlot(std::string accountId, EnumUpdateSection::UpdateSection section);

private:

	virtual void initialize();

	virtual void uninitialize();

	/** Vector of available IAccountPresenceManagerPlugin. */
	std::vector<IAccountPresenceManagerPlugin *> _iAccountPresenceManagerVector;
};

#endif	//OWACCOUNTPRESENCEMANAGER_H
