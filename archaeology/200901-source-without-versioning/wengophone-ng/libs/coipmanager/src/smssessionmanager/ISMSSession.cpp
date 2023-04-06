/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007 Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <coipmanager/smssessionmanager/ISMSSession.h>

std::string ISMSSession::getMessage() const {
	return _smsMessage;
}

void ISMSSession::setMessage(const std::string & message) {
	_smsMessage = message;
}

void ISMSSession::addPhoneNumber(const std::string & phoneNumber) {
	_phoneNumberList.push_back(phoneNumber);
}

void ISMSSession::setPhoneNumberList(const StringList & phoneNumberList) {
	_phoneNumberList = phoneNumberList;
}

StringList ISMSSession::getPhoneNumberList() const {
	return _phoneNumberList;
}
