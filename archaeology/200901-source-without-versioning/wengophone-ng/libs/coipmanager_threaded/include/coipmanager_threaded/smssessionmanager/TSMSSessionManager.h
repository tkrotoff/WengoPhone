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

#ifndef OWTSMSSESSIONMANAGER_H
#define OWTSMSSESSIONMANAGER_H

#include <coipmanager_threaded/ITCoIpSessionManager.h>
#include <coipmanager_threaded/tcoipmanagerdll.h>

class SMSSession;
class SMSSessionManager;
class TSMSSession;

/**
 * Threaded SMS Session Manager.
 *
 * @see SMSSessionManager
 * @ingroup TCoIpManager
 */
class COIPMANAGER_THREADED_API TSMSSessionManager : public ITCoIpSessionManager {
	Q_OBJECT
public:

	TSMSSessionManager(TCoIpManager & tCoIpManager);

	virtual ~TSMSSessionManager();

	/**
	 * @see SMSSessionManager::createSMSSession
	 */
	TSMSSession * createTSMSSession();

private:

	virtual ICoIpSessionManager * getICoIpSessionManager() const;

	SMSSessionManager * getSMSSessionManager() const;

};

#endif //OWTSMSSESSIONMANAGER_H
