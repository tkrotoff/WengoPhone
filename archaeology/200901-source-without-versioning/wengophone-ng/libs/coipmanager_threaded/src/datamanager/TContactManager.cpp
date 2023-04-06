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

#include <coipmanager_threaded/datamanager/TContactManager.h>

#include <coipmanager_threaded/TCoIpManager.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <util/SafeConnect.h>

TContactManager::TContactManager(TCoIpManager & tCoIpManager)
	: _tCoIpManager(tCoIpManager) {

	ContactManager * contactManager = getContactManager();

	SAFE_CONNECT(contactManager, SIGNAL(contactAddedSignal(std::string)),
		SIGNAL(contactAddedSignal(std::string)));
	SAFE_CONNECT(contactManager, SIGNAL(contactRemovedSignal(std::string)),
		SIGNAL(contactRemovedSignal(std::string)));
	SAFE_CONNECT(contactManager, SIGNAL(contactUpdatedSignal(std::string, EnumUpdateSection::UpdateSection)),
		SIGNAL(contactUpdatedSignal(std::string, EnumUpdateSection::UpdateSection)));
}

TContactManager::~TContactManager() {
}

ContactManager * TContactManager::getContactManager() {
	return &_tCoIpManager.getCoIpManager().getUserProfileManager().getContactManager();
}

void TContactManager::add(const Contact & contact) {
	getContactManager()->add(contact);
}

void TContactManager::remove(const std::string & contactId) {
	getContactManager()->remove(contactId);}

void TContactManager::update(const Contact & contact,
	EnumUpdateSection::UpdateSection section) {

	getContactManager()->update(contact, section);
}
