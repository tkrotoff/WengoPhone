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

#ifndef OWSYNCMANAGER_H
#define OWSYNCMANAGER_H

#include <coipmanager/ICoIpManager.h>
#include <coipmanager/syncmanager/ISyncManagerPluginList.h>

/**
 * Interface for all Synchronization managers.
 *
 * @author Philippe Bernery
 */
class SyncManager : public ICoIpManager {
public:

	COIPMANAGER_API SyncManager(CoIpManager & coIpManager);

	COIPMANAGER_API virtual ~SyncManager();

	COIPMANAGER_API ISyncManagerPluginList getISyncManagerPluginList() const;

protected:

	virtual void initialize();

	virtual void uninitialize();

	ISyncManagerPluginList _iSyncManagerList;
};

#endif	//OWSYNCMANAGER_H
