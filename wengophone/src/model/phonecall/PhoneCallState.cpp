/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#include "PhoneCallState.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <Sound.h>
#include <util/Logger.h>

Sound * PhoneCallState::_soundRingin = NULL;

PhoneCallState::PhoneCallState() {
}

void PhoneCallState::stopSoundRingin() {
	if (_soundRingin) {
		_soundRingin->stop();
		delete _soundRingin;
		_soundRingin = NULL;
	}
}

std::string PhoneCallState::getSoundRinginFile() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	return config.getAudioRingingFile();
}

std::string PhoneCallState::getSoundCallClosedFile() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	return config.getAudioCallClosedFile();
}

std::string PhoneCallState::getRingerAudioDeviceName() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	return config.getAudioRingerDeviceName();
}
