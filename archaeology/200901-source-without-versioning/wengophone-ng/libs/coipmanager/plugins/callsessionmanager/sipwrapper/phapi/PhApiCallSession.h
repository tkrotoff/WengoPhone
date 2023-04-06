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

#ifndef OWPHAPICALLSESSION_H
#define OWPHAPICALLSESSION_H

#include "../SipWrapperCallSession.h"

class SipWrapper;

/**
 * PhApi implementation of ICallSession and SipWrapperCallSession.
 *
 * @author Tanguy Krotoff
 */
class PhApiCallSession : public SipWrapperCallSession {
public:

	PhApiCallSession(CoIpManager & coIpManager, SipWrapper * phApiWrapper,
		int callId, EnumPhoneCallState::PhoneCallState state, const std::string & from);

	PhApiCallSession(CoIpManager & coIpManager, SipWrapper * phApiWrapper);

	virtual ~PhApiCallSession();

	//Inherited from CoIPModule and SipWrapperCallSession
	virtual void start();
};

#endif	//OWPHAPICALLSESSION_H
