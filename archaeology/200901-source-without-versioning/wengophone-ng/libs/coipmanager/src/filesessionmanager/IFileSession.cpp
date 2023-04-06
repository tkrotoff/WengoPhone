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

#include <coipmanager/filesessionmanager/IFileSession.h>

#include <util/String.h>

IFileSession::IFileSession()
	: _file(String::null) {

	_lastEvent = IFileSession::IFileSessionEventNoError;
}

IFileSession::IFileSession(const IFileSession & iFileSession)
	: _file(iFileSession._file) {

	_lastEvent = iFileSession._lastEvent;
}

IFileSession::~IFileSession() {
}

void IFileSession::setFile(const File & file) {
	_file = file;
}

File IFileSession::getFile() const {
	return _file;
}

IFileSession::IFileSessionEvent IFileSession::getLastEvent() const {
	return _lastEvent;
}
