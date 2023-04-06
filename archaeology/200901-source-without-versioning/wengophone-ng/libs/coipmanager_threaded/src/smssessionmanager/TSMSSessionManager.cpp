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

#include <coipmanager_threaded/smssessionmanager/TSMSSessionManager.h>

#include <coipmanager_threaded/TCoIpManager.h>
#include <coipmanager_threaded/smssessionmanager/TSMSSession.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/smssessionmanager/SMSSessionManager.h>

TSMSSessionManager::TSMSSessionManager(TCoIpManager & tCoIpManager)
	: ITCoIpSessionManager(tCoIpManager) {
}

TSMSSessionManager::~TSMSSessionManager() {
}

TSMSSession * TSMSSessionManager::createTSMSSession() {
	return new TSMSSession(_tCoIpManager,
		getSMSSessionManager()->createSMSSession());
}

ICoIpSessionManager * TSMSSessionManager::getICoIpSessionManager() const {
	return getSMSSessionManager();
}

SMSSessionManager * TSMSSessionManager::getSMSSessionManager() const {
	return & _tCoIpManager.getCoIpManager().getSMSSessionManager();
}
