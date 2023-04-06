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

#include "PhApiSendFileSession.h"

#include <coipmanager_base/account/WengoAccount.h>
#include <coipmanager_base/imcontact/WengoIMContact.h>
#include <coipmanager_base/userprofile/UserProfile.h>

#include "PhApiSFPWrapper.h"

#include <util/File.h>
#include <util/Logger.h>
#include <util/SafeConnect.h>

PhApiSendFileSession::PhApiSendFileSession(CoIpManager & coIpManager)
	: ISendFileSessionPlugin(coIpManager) {

	_currentCallId = -1;

	registerToEvents();
}

PhApiSendFileSession::PhApiSendFileSession(const PhApiSendFileSession & phApiSendFileSession)
	: ISendFileSessionPlugin(phApiSendFileSession) {

	_currentCallId = phApiSendFileSession._currentCallId;

	registerToEvents();
}

void PhApiSendFileSession::registerToEvents() {
	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(inviteToTransferSignal(int, const std::string &, const std::string &, const std::string &, int)),
		SLOT(inviteToTransferSlot(int, const std::string &, const std::string &, const std::string &, int)));

	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(waitingForAnswerSignal(int, const std::string &)),
		SLOT(waitingForAnswerSlot(int, const std::string &)));

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

Identifiable * PhApiSendFileSession::clone() const {
	return new PhApiSendFileSession(*this);
}

PhApiSendFileSession::~PhApiSendFileSession() {
}

void PhApiSendFileSession::start() {
	if (_imContactList.size() > 0) {
		WengoAccount *wAccount = static_cast<WengoAccount *>(_account.getPrivateAccount());
		WengoIMContact *imContact =static_cast<WengoIMContact *>((*_imContactList.begin()).getPrivateIMContact());
		PhApiSFPWrapper &wrapper = PhApiSFPWrapper::getInstance();
		int lineId = wAccount->getVirtualLineId();
		std::string fullIdentityWithDispName = imContact->toString();
		std::string fileType = _file.getExtension();

		if (fileType.empty()) {
			fileType = "*";
		}

		_currentCallId = wrapper.sendFile(lineId,
			fullIdentityWithDispName,
			imContact->getFullIdentity(),
			_file.getFullPath(),
			_file.getFilename(),
			fileType,
			_file.getSize());
	} else {
		LOG_ERROR("no contact in session");
	}
}

void PhApiSendFileSession::pause() {
	if (_currentCallId > 0) {
		PhApiSFPWrapper::getInstance().pauseTransfer(_currentCallId);
	}
}

void PhApiSendFileSession::resume() {
	if (_currentCallId > 0) {
		PhApiSFPWrapper::getInstance().resumeTransfer(_currentCallId);
	}
}

void PhApiSendFileSession::stop() {
	if (_currentCallId > 0) {
		PhApiSFPWrapper::getInstance().cancelTransfer(_currentCallId);
	}
}

void PhApiSendFileSession::inviteToTransferSlot(int callId, const std::string & uri,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == extractContactIdFromURI(uri)) &&
		(_file.getFilename() == fileName) &&
		(_file.getSize() == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventInviteToTransfer);
	}
}

void PhApiSendFileSession::waitingForAnswerSlot(int callId, const std::string & uri) {
	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == extractContactIdFromURI(uri))) {
		fileTransferSignal(IFileSession::IFileSessionEventWaitingForAnswer);
	}
}

void PhApiSendFileSession::transferCancelledSlot(int callId, const std::string & fileName,
	const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		(_file.getFilename() == fileName) &&
		(_file.getSize() == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferCancelled);
		moduleFinishedSignal();
	}
}

void PhApiSendFileSession::transferCancelledByPeerSlot(int callId, const std::string & /*contactId*/,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		//(*_currentContact).getContactId().compare(contactId) == 0 &&
		(_file.getFilename() == fileName) &&
		(_file.getSize() == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferCancelledByPeer);
		moduleFinishedSignal();
	}
}

void PhApiSendFileSession::sendingFileBeginSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_file.getFilename() == fileName) &&
		(_file.getSize() == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferBegan);
	}
}

void PhApiSendFileSession::transferFromPeerFinishedSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_file.getFilename() == fileName) &&
		(_file.getSize() == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferFinished);
		moduleFinishedSignal();
	}
}

void PhApiSendFileSession::transferToPeerFinishedSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_file.getFilename() == fileName) &&
		(_file.getSize() == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferFinished);
		_currentCallId = -1;
		moduleFinishedSignal();
	}
}

void PhApiSendFileSession::transferFromPeerFailedSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_file.getFilename() == fileName) &&
		(_file.getSize() == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferFailed);
		moduleFinishedSignal();
	}
}

void PhApiSendFileSession::transferToPeerFailedSlot(int callId, const std::string & /*contactId*/,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		//(*_currentContact).getContactId().compare(contactId) == 0 &&
		(_file.getFilename() == fileName) &&
		(_file.getSize() == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferFailed);
		moduleFinishedSignal();
	}
}

void PhApiSendFileSession::transferProgressionSlot( int callId, int percentage) {
	if (callId == _currentCallId) {
		fileTransferProgressionSignal(percentage);
	}
}

void PhApiSendFileSession::transferPausedByPeerSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_file.getFilename() == fileName) &&
		(_file.getSize() == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferPausedByPeer);
	}
}

void PhApiSendFileSession::transferPausedSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_file.getFilename() == fileName) &&
		(_file.getSize() == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferPaused);
	}
}

void PhApiSendFileSession::transferResumedByPeerSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_file.getFilename() == fileName) &&
		(_file.getSize() == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferResumedByPeer);
	}
}

void PhApiSendFileSession::transferResumedSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	if ((callId == _currentCallId) &&
		((*_imContactList.begin()).getContactId() == contactId) &&
		(_file.getFilename() == fileName) &&
		(_file.getSize() == (unsigned) fileSize)) {
		fileTransferSignal(IFileSession::IFileSessionEventFileTransferResumed);
	}
}

std::string PhApiSendFileSession::extractContactIdFromURI(const std::string & uri) const {
	std::string::size_type usernameStartIndex = uri.find(":");
	std::string::size_type usernameEndIndex = uri.find("@");

	if (usernameStartIndex == std::string::npos
		|| usernameStartIndex == std::string::npos
		|| usernameStartIndex > usernameEndIndex) {
		return "";
	}

	return uri.substr(usernameStartIndex, usernameEndIndex - usernameStartIndex + 1);
}
