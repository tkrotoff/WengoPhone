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

#include <coipmanager/ICoIpManager.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/ICoIpManagerPlugin.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

ICoIpManager::ICoIpManager(CoIpManager & coIpManager)
	: CoIpManagerUser(coIpManager) {

	_mutex = new QMutex(QMutex::Recursive);
}

ICoIpManager::~ICoIpManager() {
	for (std::vector<ICoIpManagerPlugin *>::const_iterator it = _iCoIpManagerPluginList.begin();
		it != _iCoIpManagerPluginList.end();
		it++) {
		delete (*it);
	}

	OWSAFE_DELETE(_mutex);
}

void ICoIpManager::coIpManagerIsInitializing() {
	retain();

	initialize();
}

void ICoIpManager::releaseCoIpManager() {
	uninitialize();

	release();
}

void ICoIpManager::initialize() {
	for (ICoIpManagerPluginList::const_iterator it = _iCoIpManagerPluginList.begin();
		it != _iCoIpManagerPluginList.end();
		it++) {
		(*it)->initialize();
	}
}

void ICoIpManager::uninitialize() {
	for (ICoIpManagerPluginList::const_iterator it = _iCoIpManagerPluginList.begin();
		it != _iCoIpManagerPluginList.end();
		it++) {
		(*it)->uninitialize();
	}
}
