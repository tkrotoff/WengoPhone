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

#ifndef PHONELINE_H
#define PHONELINE_H

#include <NonCopyable.h>

#include <string>
#include <vector>

class SipAddress;

/**
 * Phone line.
 *
 * @author Tanguy Krotoff
 */
class PhoneLine : NonCopyable {
public:

	PhoneLine(const SipAddress & identity,
		const std::string & password,
		const std::string & registryServer,
		const std::string & proxyServer);

	~PhoneLine();

	/**
	 * Creates a new call in the "idle" state, ready to dial.
	 *
	 * @return a new call
	 */
	PhoneCall & createPhoneCall();

	/**
	 * Returns the currently active call.
	 *
	 * That is, the call that is connected and not on hold.
	 * PhoneCall::null if there is no active call.
	 */
	PhoneCall & getActivePhoneCall() const;

private:

	/**
	 * Defines the vector of PhoneCall.
	 */
	typedef std::vector<PhoneCall *> PhoneCalls;

	/**
	 * List of PhoneCalls.
	 */
	PhoneCalls _phoneCallList;
};

#endif	//PHONELINE_H
