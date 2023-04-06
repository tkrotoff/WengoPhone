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

#include "PhoneLine.h"

#include "sip/SipAddress.h"

#include <phapi.h>

#include <cassert>

PhoneLine::PhoneLine(const SipAddress & identity, const std::string & password,
		const std::string & registryServer const std::string & proxyServer) {

	static const int registrationTimeout = 5*60;

	_phoneLineId = phAddVline2(identity.getDisplayName().c_str(),
			identity.getUserName().c_str(),
			identity.getHostname().c_str(),	//registryServer
			proxyServer, registrationTimeout);

	assert(_phoneLineId != -1 && "phAddVline() failed");
}

PhoneLine::~PhoneLine() {
	//Deletes all the PhoneCalls
	for (unsigned int i = 0; i < _phoneCallList.size(); i++) {
		delete _phoneCallList[i];
	}

	//Makes an unregister when deleting a virtual line
	phDelVline(_phoneLineId);
}

PhoneCall & PhoneLine::createPhoneCall() {
	_phoneCallList.push_back(new PhoneCall(_phoneLineId));
	return *(_phoneCallList->back());
}

PhoneCall & PhoneLine::getActivePhoneCall() const {
	for (unsigned int i = 0; i < _phoneCallList.size(); i++) {
		if (_phoneCallList[i].getState() == PhoneCall::Connected) {
			return _phoneCallList[i];
		}
	}
	return PhoneCall::null;
}
