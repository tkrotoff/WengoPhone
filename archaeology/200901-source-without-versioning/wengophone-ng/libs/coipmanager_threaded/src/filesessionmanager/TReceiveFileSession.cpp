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

#include <coipmanager_threaded/filesessionmanager/TReceiveFileSession.h>

#include <util/SafeConnect.h>

TReceiveFileSession::TReceiveFileSession(TCoIpManager & tCoIpManager, ReceiveFileSession * receiveFileSession)
	: TIFileSession(tCoIpManager, receiveFileSession) {

	SAFE_CONNECT(receiveFileSession,
		SIGNAL(fileTransferSignal(IFileSession::IFileSessionEvent)),
		SIGNAL(fileTransferSignal(IFileSession::IFileSessionEvent)));
	SAFE_CONNECT(receiveFileSession,
		SIGNAL(fileTransferProgressionSignal(int)),
		SIGNAL(fileTransferProgressionSignal(int)));
}

TReceiveFileSession::~TReceiveFileSession() {
}

ReceiveFileSession * TReceiveFileSession::getReceiveFileSession() const {
	return static_cast<ReceiveFileSession *>(_module);
}

Contact TReceiveFileSession::getContact() const {
	return getReceiveFileSession()->getContact();
}

void TReceiveFileSession::setFilePath(const std::string & path) {
	getReceiveFileSession()->setFilePath(path);
}

std::string TReceiveFileSession::getFilePath() const {
	return getReceiveFileSession()->getFilePath();
}

std::string TReceiveFileSession::getFileName() const {
	return getReceiveFileSession()->getFileName();
}

unsigned TReceiveFileSession::getFileSize() const {
	return getReceiveFileSession()->getFileSize();
}
