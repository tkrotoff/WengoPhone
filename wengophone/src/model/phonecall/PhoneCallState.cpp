/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include <model/WengoPhone.h>

#include <Sound.h>
#include <File.h>
#include <Logger.h>

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
	std::string file = File::convertPathSeparators(WengoPhone::getConfigFilesPath() + "../extensions/{debaffee-a972-4d8a-b426-8029170f2a89}/sounds/ringin.wav");
	LOG_DEBUG_C("ringin.wav" + file);
	return file;
}

std::string PhoneCallState::getSoundCallClosedFile() {
	std::string file = File::convertPathSeparators(WengoPhone::getConfigFilesPath() + "../extensions/{debaffee-a972-4d8a-b426-8029170f2a89}/sounds/callclosed.wav");
	LOG_DEBUG_C("callclosed.wav=" + file);
	return file;
}
