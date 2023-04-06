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

#include <coipmanager/filesessionmanager/ReceiveFileSession.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>
#include <coipmanager/filesessionmanager/FileSessionManager.h>
#include <coipmanager/filesessionmanager/IReceiveFileSessionPlugin.h>


#include <util/SafeDelete.h>
#include <util/SafeConnect.h>
#include <util/Logger.h>
#include <util/String.h>

ReceiveFileSession::ReceiveFileSession(CoIpManager & coIpManager, IReceiveFileSessionPlugin * fileSessionPlugin)
	: Session(coIpManager) {

	_currentFileSessionPlugin = fileSessionPlugin;
}

ReceiveFileSession::ReceiveFileSession(const ReceiveFileSession & receiveFileSession)
	: Session(receiveFileSession),
	IReceiveFileSession(receiveFileSession) {

	if (receiveFileSession._currentFileSessionPlugin) {
		//_currentFileSessionPlugin = receiveFileSession._currentFileSessionPlugin->clone();
		_currentFileSessionPlugin = (IReceiveFileSessionPlugin *) receiveFileSession._currentFileSessionPlugin->clone();
		//_currentFileSessionPlugin = dynamic_cast<IReceiveFileSessionPlugin *>(receiveFileSession._currentFileSessionPlugin->clone());
		SAFE_CONNECT(_currentFileSessionPlugin, SIGNAL(moduleFinishedSignal()),
			SIGNAL(moduleFinishedSignal()));
		SAFE_CONNECT(_currentFileSessionPlugin, SIGNAL(fileTransferSignal(IFileSession::IFileSessionEvent)),
			SLOT(fileTransferSlot(IFileSession::IFileSessionEvent)));
		SAFE_CONNECT(_currentFileSessionPlugin, SIGNAL(fileTransferProgressionSignal(int)),
			SLOT(fileTransferProgressionSlot(int)));
	} else {
		_currentFileSessionPlugin = NULL;
	}
}

ReceiveFileSession::~ReceiveFileSession() {
	QMutexLocker lock(_mutex);

	OWSAFE_DELETE(_currentFileSessionPlugin);
}

void ReceiveFileSession::start() {
	QMutexLocker lock(_mutex);

	if (_currentFileSessionPlugin) {
		if (_currentFileSessionPlugin->isStarted()) {
			LOG_ERROR("file session already started");
		}

		_currentFileSessionPlugin->start();
	}
}

void ReceiveFileSession::pause() {
	QMutexLocker lock(_mutex);

	if (_currentFileSessionPlugin) {
		_currentFileSessionPlugin->pause();
	}
}

void ReceiveFileSession::resume() {
	QMutexLocker lock(_mutex);

	if (_currentFileSessionPlugin) {
		_currentFileSessionPlugin->resume();
	}
}

void ReceiveFileSession::stop() {
	QMutexLocker lock(_mutex);

	if (_currentFileSessionPlugin) {
		if (!_currentFileSessionPlugin->isStarted()) {
			LOG_ERROR("file session not started");
		}

		_currentFileSessionPlugin->stop();
	}
}

bool ReceiveFileSession::canCreateISession(const Account & account, const ContactList & contactList) const {
	return (_coIpManager.getFileSessionManager().getICoIpManagerPlugin(account, contactList) != NULL);
}

Contact ReceiveFileSession::getContact() const {
	Contact result;

	QMutexLocker lock(_mutex);

	if (_currentFileSessionPlugin) {
		ContactList contactList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getContactList();
		Contact * contact = ContactListHelper::getCopyOfFirstContactThatOwns(contactList, _currentFileSessionPlugin->getIMContact());
		if (contact) {
			result = *contact;
			OWSAFE_DELETE(contact);
		}
	}

	return result;
}

void ReceiveFileSession::setFilePath(const std::string & path) {
	QMutexLocker lock(_mutex);

	if (_currentFileSessionPlugin) {
		_currentFileSessionPlugin->setFilePath(path + File::getPathSeparator());
	}
}

std::string ReceiveFileSession::getFilePath() const {
	QMutexLocker lock(_mutex);

	if (_currentFileSessionPlugin) {
		return _currentFileSessionPlugin->getFilePath();
	} else {
		return String::null;
	}
}

std::string ReceiveFileSession::getFileName() const {
	QMutexLocker lock(_mutex);

	if (_currentFileSessionPlugin) {
		return _currentFileSessionPlugin->getFileName();
	} else {
		return String::null;
	}
}

unsigned ReceiveFileSession::getFileSize() const {
	QMutexLocker lock(_mutex);

	if (_currentFileSessionPlugin) {
		return _currentFileSessionPlugin->getFileSize();
	} else {
		return 0;
	}
}

void ReceiveFileSession::fileTransferSlot(IFileSession::IFileSessionEvent event) {
	QMutexLocker lock(_mutex);

	_lastEvent = event;

	fileTransferSignal(event);
}

void ReceiveFileSession::fileTransferProgressionSlot(int percentage) {
	QMutexLocker lock(_mutex);

	fileTransferProgressionSignal(percentage);
}

EnumSessionError::SessionError ReceiveFileSession::addContact(const Contact & /*contact*/) {
	return EnumSessionError::SessionErrorNoError;
}

void ReceiveFileSession::removeContact(const Contact & /*contact*/) {
}
