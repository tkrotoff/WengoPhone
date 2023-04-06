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

#include "PhApiReceiveFileSession.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include "PhApiSFPWrapper.h"

#include <util/File.h>
#include <util/Logger.h>
#include <util/SafeConnect.h>

PhApiReceiveFileSession::PhApiReceiveFileSession(CoIpManager & coIpManager)
	: IReceiveFileSessionPlugin(coIpManager) {
}

PhApiReceiveFileSession::PhApiReceiveFileSession(CoIpManager & coIpManager,
	int callId, const std::string & contactId, const std::string & fileName, unsigned fileSize)
	: IReceiveFileSessionPlugin(coIpManager) {

	IMContact imContact(EnumAccountType::AccountTypeWengo, contactId);
	ContactList contactList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getContactList();
	ContactListHelper::getIMContact(contactList, imContact);
	_imContactList.push_back(imContact);

	_fileName = fileName;
	_fileSize = fileSize;
	_currentCallId = callId;

	registerToEvents();
}

PhApiReceiveFileSession::PhApiReceiveFileSession(const PhApiReceiveFileSession & phApiReceiveFileSession)
	: IReceiveFileSessionPlugin(phApiReceiveFileSession) {

	_fileName = phApiReceiveFileSession._fileName;
	_fileSize = phApiReceiveFileSession._fileSize;
	_currentCallId = phApiReceiveFileSession._currentCallId;

	registerToEvents();
}

void PhApiReceiveFileSession::registerToEvents() {
	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(transferCancelledSignal(int, const std::string &, const std::string &, int)),
		SLOT(transferCancelledSlot(int, const std::string &, const std::string &, int)));

	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(transferCancelledByPeerSignal(int, const std::string &, const std::string &, const std::string &, int)),
		SLOT(transferCancelledByPeerSlot(int, const std::string &, const std::string &, const std::string &, int)));

	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(sendingFileBeginSignal(int, const std::string &, const std::string &, const std::string &, int)),
		SLOT(sendingFileBeginSlot(int, const std::string &, const std::string &, const std::string &, int)));

	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(transferFromPeerFinishedSignal(int, const std::string &, const std::string &, const std::string &, int)),
		SLOT(transferFromPeerFinishedSlot(int, const std::string &, const std::string &, const std::string &, int)));

	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(transferToPeerFinishedSignal(int, const std::string &, const std::string &, const std::string &, int)),
		SLOT(transferToPeerFinishedSlot(int, const std::string &, const std::string &, const std::string &, int)));

	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(transferFromPeerFailedSignal(int, const std::string &, const std::string &, const std::string &, int)),
		SLOT(transferFromPeerFailedSlot(int, const std::string &, const std::string &, const std::string &, int)));

	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(transferToPeerFailedSignal(int, const std::string &, const std::string &, const std::string &, int)),
		SLOT(transferToPeerFailedSlot(int, const std::string &, const std::string &, const std::string &, int)));

	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(transferProgressionSignal(int, int)),
		SLOT(transferProgressionSlot(int, int)));

	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(transferPausedByPeerSignal(int, const std::string &, const std::string &, const std::string &, int)),
		SLOT(transferPausedByPeerSlot(int, const std::string &, const std::string &, const std::string &, int)));

	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(transferPausedSignal(int, const std::string &, const std::string &, const std::string &, int)),
		SLOT(transferPausedSlot(int, const std::string &, const std::string &, const std::string &, int)));

	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(transferResumedByPeerSignal(int, const std::string &, const std::string &, const std::string &, int)),
		SLOT(transferResumedByPeerSlot(int, const std::string &, const std::string &, const std::string &, int)));

	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(transferResumedSignal(int, const std::string &, const std::string &, const std::string &, int)),
		SLOT(transferResumedSlot(int, const std::string &, const std::string &, const std::string &, int)));
}

Identifiable * PhApiReceiveFileSession::clone() const {
	return new PhApiReceiveFileSession(*this);
}

PhApiReceiveFileSession::~PhApiReceiveFileSession() {
}

void PhApiReceiveFileSession::start() {
	LOG_DEBUG("starting file transfer");

	PhApiSFPWrapper::getInstance().receiveFile(_currentCallId, File::convertPathSeparators(_filePath + _fileName));

	LOG_DEBUG("end of transfer start");
}

void PhApiReceiveFileSession::pause() {
	LOG_DEBUG("pausing file transfer");

	if (_currentCallId > 0) {
		PhApiSFPWrapper::getInstance().pauseTransfer(_currentCallId);
	}

	LOG_DEBUG("end of transfer pause");
}

void PhApiReceiveFileSession::resume() {
	LOG_DEBUG("resuming file transfer");

	if (_currentCallId > 0) {
		PhApiSFPWrapper::getInstance().resumeTransfer(_currentCallId);
	}

	LOG_DEBUG("end of transfer resume");
}

void PhApiReceiveFileSession::stop() {
	LOG_DEBUG("stopping file transfer");

	if (_currentCallId > 0) {
		PhApiSFPWrapper::getInstance().cancelTransfer(_currentCallId);
	}

	LOG_DEBUG("end of transfer stop");
}

void PhApiReceiveFileSession::transferCancelledSlot(int callId, const std::string & fileName,
	const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		(_fileName == fileName) &&
		(_fileSize == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferCancelled);
		moduleFinishedSignal();
	}
}

void PhApiReceiveFileSession::transferCancelledByPeerSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_fileName == fileName) &&
		(_fileSize == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferCancelledByPeer);
		moduleFinishedSignal();
	}
}

void PhApiReceiveFileSession::sendingFileBeginSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_fileName == fileName) &&
		(_fileSize == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferBegan);
	}
}

void PhApiReceiveFileSession::transferFromPeerFinishedSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_fileName == fileName) &&
		(_fileSize == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferFinished);
		moduleFinishedSignal();
	}
}

void PhApiReceiveFileSession::transferToPeerFinishedSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_fileName == fileName) &&
		(_fileSize == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferFinished);
		moduleFinishedSignal();
	}
}

void PhApiReceiveFileSession::transferFromPeerFailedSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_fileName == fileName) &&
		(_fileSize == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferFailed);
		moduleFinishedSignal();
	}
}

void PhApiReceiveFileSession::transferToPeerFailedSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_fileName == fileName) &&
		(_fileSize == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferFailed);
		moduleFinishedSignal();
	}
}

void PhApiReceiveFileSession::transferProgressionSlot(int callId, int percentage) {
	if ((callId == _currentCallId)) {
		fileTransferProgressionSignal(percentage);
	}
}

void PhApiReceiveFileSession::transferPausedByPeerSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_fileName == fileName) &&
		(_fileSize == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferPausedByPeer);
	}
}

void PhApiReceiveFileSession::transferPausedSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_fileName == fileName) &&
		(_fileSize == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferPaused);
	}
}

void PhApiReceiveFileSession::transferResumedByPeerSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_fileName == fileName) &&
		(_fileSize == (unsigned)fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferResumedByPeer);
	}
}

void PhApiReceiveFileSession::transferResumedSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_fileName == fileName) &&
		(_fileSize == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferResumed);
	}
}
