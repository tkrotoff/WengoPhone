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

#include <coipmanager_threaded/filesessionmanager/TFileSessionManager.h>

#include <coipmanager_threaded/TCoIpManager.h>
#include <coipmanager_threaded/filesessionmanager/TReceiveFileSession.h>
#include <coipmanager_threaded/filesessionmanager/TSendFileSession.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/filesessionmanager/FileSessionManager.h>

#include <util/SafeConnect.h>

TFileSessionManager::TFileSessionManager(TCoIpManager & tCoIpManager)
	: ITCoIpSessionManager(tCoIpManager) {

	SAFE_CONNECT(getFileSessionManager(),
		SIGNAL(newReceiveFileSessionCreatedSignal(ReceiveFileSession *)),
		SLOT(newReceiveFileSessionCreatedSlot(ReceiveFileSession *)));
	SAFE_CONNECT(getFileSessionManager(),
		SIGNAL(needsUpgradeSignal()),
		SIGNAL(needsUpgradeSignal()));
	SAFE_CONNECT(getFileSessionManager(),
		SIGNAL(peerNeedsUpgradeSignal(Contact)),
		SIGNAL(peerNeedsUpgradeSignal(Contact)));
}

TFileSessionManager::~TFileSessionManager() {
}

FileSessionManager * TFileSessionManager::getFileSessionManager() const {
	return &_tCoIpManager.getCoIpManager().getFileSessionManager();
}

ICoIpSessionManager * TFileSessionManager::getICoIpSessionManager() const {
	return getFileSessionManager();
}

TSendFileSession * TFileSessionManager::createTSendFileSession() {
	return new TSendFileSession(_tCoIpManager);
}

void TFileSessionManager::newReceiveFileSessionCreatedSlot(ReceiveFileSession * fileSession) {
	newTReceiveFileSessionCreatedSignal(new TReceiveFileSession(_tCoIpManager, fileSession));
}
