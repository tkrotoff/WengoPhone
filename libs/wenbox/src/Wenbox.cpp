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

#include "Wenbox.h"

#include <Logger.h>
#include <File.h>
#include <SharedLibLoader.h>

IWenbox * getInstance() {
	return NULL;
}

Wenbox::Wenbox() {
	/*File file(File::getApplicationDirPath() + "wenbox" + File::getPathSeparator());
	StringList files = file.getFileList();
	for (int i = 0; i < files.size(); i++) {
		files[i].contains(".dll");
	}*/

	_open = false;
	_wenboxPrivate = NULL;
	typedef IWenbox * (*GetInstanceFunction)();

	std::string dllPath = File::getApplicationDirPath() + "yealinkwenbox";
	LOG_DEBUG(dllPath);

	GetInstanceFunction getInstance = (GetInstanceFunction) SharedLibLoader::resolve(dllPath, "getInstance");

	if (getInstance) {
		LOG_DEBUG("Wenbox dll loaded");
		_wenboxPrivate = getInstance();
		_wenboxPrivate->keyPressedEvent += keyPressedEvent;
	} else {
		LOG_ERROR("Wenbox dll not loaded");
	}
}

Wenbox::~Wenbox() {
	close();
	delete _wenboxPrivate;
}

bool Wenbox::open() {
	if (_wenboxPrivate) {
		LOG_DEBUG("open device");
		_open = _wenboxPrivate->open();
		return _open;
	}
	return false;
}

bool Wenbox::close() {
	if (_wenboxPrivate) {
		_open = false;
		LOG_DEBUG("close device");
		return _wenboxPrivate->close();
	}
	return false;
}

std::string Wenbox::getDeviceName() {
	if (_wenboxPrivate) {
		return _wenboxPrivate->getDeviceName();
	}
	return String::null;
}

StringList Wenbox::getAudioDeviceNameList() const {
	StringList strList;

	if (_wenboxPrivate) {
		return _wenboxPrivate->getAudioDeviceNameList();
	}

	return strList;
}

bool Wenbox::setDefaultMode(Mode mode) {
	if (_wenboxPrivate) {
		return _wenboxPrivate->setDefaultMode(mode);
	}
	return false;
}

bool Wenbox::switchMode(Mode mode) {
	if (_wenboxPrivate) {
		return _wenboxPrivate->switchMode(mode);
	}
	return false;
}

bool Wenbox::setLCDMessage(const std::string & message) {
	if (_wenboxPrivate) {
		LOG_DEBUG("LCD message changed");
		return _wenboxPrivate->setLCDMessage(message);
	}
	return false;
}

bool Wenbox::setRingingTone(int tone) {
	if (_wenboxPrivate) {
		LOG_DEBUG("ringing tone changed");
		return _wenboxPrivate->setRingingTone(tone);
	}
	return false;
}

bool Wenbox::setState(PhoneCallState state, const std::string & phoneNumber) {
	if (_wenboxPrivate) {
		return _wenboxPrivate->setState(state, phoneNumber);
	}
	return false;
}
