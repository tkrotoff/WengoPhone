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

#include <coipmanager/filesessionmanager/SendFileSession.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>
#include <coipmanager/filesessionmanager/FileSessionManager.h>
#include <coipmanager/filesessionmanager/ISendFileSessionPlugin.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

SendFileSession::SendFileSession(CoIpManager & coIpManager)
	: Session(coIpManager) {
	_currentFileSessionPlugin = NULL;
}

SendFileSession::~SendFileSession() {
	QMutexLocker lock(_mutex);

	OWSAFE_DELETE(_currentFileSessionPlugin);
}

void SendFileSession::start() {
	QMutexLocker lock(_mutex);

	// Check if a FileSession is not currently running.
	if (_currentFileSessionPlugin) {
		LOG_ERROR("session already started");
		return;
	}

	// Check if we have files to send and contacts to send to
	if (_file.getFilename().empty() || _contactList.empty()) {
		moduleFinishedSignal();
		return;
	}

	//outgoing call
	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	Account * account = AccountListHelper::getCopyOfAccount(accountList, _accountToUse);
	if (account) {
		IFileSessionManagerPlugin *iFileSessionManager =
			_coIpManager.getFileSessionManager().getICoIpManagerPlugin(*account, _contactList);
		if (iFileSessionManager) {
			_currentFileSessionPlugin = iFileSessionManager->createISendFileSessionPlugin();

			SAFE_CONNECT(_currentFileSessionPlugin, SIGNAL(moduleFinishedSignal()),
				SIGNAL(moduleFinishedSignal()));
			SAFE_CONNECT(_currentFileSessionPlugin, SIGNAL(fileTransferSignal(IFileSession::IFileSessionEvent)),
				SLOT(fileTransferSlot(IFileSession::IFileSessionEvent)));
			SAFE_CONNECT(_currentFileSessionPlugin, SIGNAL(fileTransferProgressionSignal(int)),
				SLOT(fileTransferProgressionSlot(int)));

			IMContactList imContactList = iFileSessionManager->getValidIMContacts(*account, _contactList);
			_currentFileSessionPlugin->setIMContactList(imContactList);
			_currentFileSessionPlugin->setAccount(*account);
			_currentFileSessionPlugin->setFile(_file);

			_currentFileSessionPlugin->start();
		}
		OWSAFE_DELETE(account);
	} else {
		LOG_ERROR("account \"" + _accountToUse + "\" not found");
	}
}

void SendFileSession::pause() {
	QMutexLocker lock(_mutex);

	if (_currentFileSessionPlugin) {
		_currentFileSessionPlugin->pause();
	} else {
		LOG_ERROR("session not started");
	}
}

void SendFileSession::resume() {
	QMutexLocker lock(_mutex);

	if (_currentFileSessionPlugin) {
		_currentFileSessionPlugin->resume();
	} else {
		LOG_ERROR("session not started");
	}
}

void SendFileSession::stop() {
	QMutexLocker lock(_mutex);

	if (_currentFileSessionPlugin) {
		_currentFileSessionPlugin->stop();
	} else {
		LOG_ERROR("session not started");
	}
}

bool SendFileSession::canCreateISession(const Account & account, const ContactList & contactList) const {
	return (_coIpManager.getFileSessionManager().getICoIpManagerPlugin(account, contactList) != NULL);
}

void SendFileSession::fileTransferSlot(IFileSession::IFileSessionEvent event) {
	_lastEvent = event;

	fileTransferSignal(event);
}

void SendFileSession::fileTransferProgressionSlot(int percentage) {
	fileTransferProgressionSignal(percentage);
}
