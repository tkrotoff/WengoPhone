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

#include <coipmanager/CoIpManagerSaver.h>

#include <coipmanager_base/storage/UserProfileFileStorage.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/CoIpManagerConfig.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

const time_t SAVE_INTERVAL = 5;

CoIpManagerSaver::CoIpManagerSaver(CoIpManager & coIpManager)
	: ICoIpManager(coIpManager) {

	SAFE_CONNECT(&_coIpManager.getUserProfileManager(), SIGNAL(userProfileUpdatedSignal(UserProfile)),
		SLOT(userProfileUpdatedSlot(UserProfile)));

	SAFE_CONNECT(&_coIpManager.getUserProfileManager().getAccountManager(), SIGNAL(accountAddedSignal(std::string)),
		SLOT(accountAddedSlot(std::string)));
	SAFE_CONNECT(&_coIpManager.getUserProfileManager().getAccountManager(), SIGNAL(accountRemovedSignal(std::string)),
		SLOT(accountRemovedSlot(std::string)));
	SAFE_CONNECT(&_coIpManager.getUserProfileManager().getAccountManager(), SIGNAL(accountUpdatedSignal(std::string, EnumUpdateSection::UpdateSection)),
		SLOT(accountUpdatedSlot(std::string, EnumUpdateSection::UpdateSection)));

	SAFE_CONNECT(&_coIpManager.getUserProfileManager().getContactManager(), SIGNAL(contactAddedSignal(std::string)),
		SLOT(contactAddedSlot(std::string)));
	SAFE_CONNECT(&_coIpManager.getUserProfileManager().getContactManager(), SIGNAL(contactRemovedSignal(std::string)),
		SLOT(contactRemovedSlot(std::string)));
	SAFE_CONNECT(&_coIpManager.getUserProfileManager().getContactManager(), SIGNAL(contactUpdatedSignal(std::string, EnumUpdateSection::UpdateSection)),
		SLOT(contactUpdatedSlot(std::string, EnumUpdateSection::UpdateSection)));

	time(&_lastSaveTime);
	_mutex = new QMutex(QMutex::Recursive);
}

CoIpManagerSaver::~CoIpManagerSaver() {
	OWSAFE_DELETE(_mutex);
}

ICoIpManagerPlugin * CoIpManagerSaver::getICoIpManagerPlugin(const Account & account, const ContactList & contactList) const {
	return NULL;
}

void CoIpManagerSaver::initialize() {
}

void CoIpManagerSaver::uninitialize() {
	if (_coIpManager.getCoIpManagerConfig().autoSave()) {
		save();
	}
}

void CoIpManagerSaver::userProfileUpdatedSlot(UserProfile) {
	dataChangedSlot(EnumUpdateSection::UpdateSectionUnknown);
}

void CoIpManagerSaver::accountAddedSlot(std::string) {
	dataChangedSlot(EnumUpdateSection::UpdateSectionUnknown);
}

void CoIpManagerSaver::accountRemovedSlot(std::string) {
	dataChangedSlot(EnumUpdateSection::UpdateSectionUnknown);
}

void CoIpManagerSaver::accountUpdatedSlot(std::string, EnumUpdateSection::UpdateSection section) {
	dataChangedSlot(section);
}

void CoIpManagerSaver::contactAddedSlot(std::string) {
	dataChangedSlot(EnumUpdateSection::UpdateSectionUnknown);
}

void CoIpManagerSaver::contactRemovedSlot(std::string) {
	dataChangedSlot(EnumUpdateSection::UpdateSectionUnknown);
}

void CoIpManagerSaver::contactUpdatedSlot(std::string, EnumUpdateSection::UpdateSection section) {
	dataChangedSlot(section);
}

void CoIpManagerSaver::dataChangedSlot(EnumUpdateSection::UpdateSection section) {
	if ((section != EnumUpdateSection::UpdateSectionVolatileData)
		&& (section != EnumUpdateSection::UpdateSectionPresenceState)) {
		if (_coIpManager.getCoIpManagerConfig().autoSave()) {
			askForSave();
		}
	}
}

void CoIpManagerSaver::askForSave() {
	QMutexLocker lock(_mutex);

	time_t currentTime;
	time(&currentTime);

	if ((currentTime - _lastSaveTime) > SAVE_INTERVAL) {
		save();
	}
}

void CoIpManagerSaver::save() {
	QMutexLocker lock(_mutex);

	UserProfileFileStorage userProfileFileStorage(_coIpManager.getCoIpManagerConfig().getConfigPath());
	UserProfile userProfile = _coIpManager.getUserProfileManager().getCopyOfUserProfile();
	if (!userProfile.isEmpty()) {
		userProfileFileStorage.save(userProfile);
	}
	time(&_lastSaveTime);
}
