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

#ifndef OWIAXCLIENTCALLSESSIONMANAGER_H
#define OWIAXCLIENTCALLSESSIONMANAGER_H

#include <coipmanager/callsessionmanager/ICallSessionManagerPlugin.h>

#include <sipwrapper/EnumPhoneCallState.h>

class SipWrapper;

/**
 * IAXClient implementation of ICallSessionManager.
 *
 * Keep synchronized with PhApiCallSessionManager
 *
 * @author Yann Biancheri
 * @author Tanguy Krotoff
 */
class IAXClientCallSessionManager : public ICallSessionManagerPlugin {
public:

	IAXClientCallSessionManager(CoIpManager & coIpManager);

	virtual ~IAXClientCallSessionManager();

	//Inherited from ICallSessionManager
	virtual ICallSessionPlugin * createICallSessionPlugin();

private:

	SipWrapper * _iaxClientWrapper;

	void phoneCallStateChangedEventHandler(SipWrapper * sender, int callId,
		EnumPhoneCallState::PhoneCallState state, const std::string & from);
};

#endif	//OWIAXCLIENTCALLSESSIONMANAGER_H
