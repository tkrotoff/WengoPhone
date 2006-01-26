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

#ifndef NULLSIPWRAPPER_H
#define NULLSIPWRAPPER_H

#include "model/sipwrapper/SipWrapper.h"

/**
 * Null SIP wrapper.
 *
 * Stub for the SIP implementation. Permits to test
 * WengoPhone without a SIP implementation.
 *
 * This makes it very easy to port libwengophone-ng and
 * qtwengophone-ng to other platforms.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class NullSipWrapper : public SipWrapper {
public:

	static NullSipWrapper * getInstance() {
		static NullSipWrapper instance;

		return &instance;
	}

	~NullSipWrapper();

	void terminate();

	int addVirtualLine(const std::string & displayName,
		const std::string & username, const std::string & identity,
		const std::string & password, const std::string & realm,
		const std::string & proxyServer, const std::string & registerServer);

	void removeVirtualLine(int lineId);

	int makeCall(int lineId, const std::string & phoneNumber);

	void sendRingingNotification(int callId);

	void acceptCall(int callId);

	void rejectCall(int callId);

	void closeCall(int callId);

	void holdCall(int callId);

	void resumeCall(int callId);

	bool setCallInputAudioDevice(const std::string & deviceName);

	bool setRingerOutputAudioDevice(const std::string & deviceName);

	bool setCallOutputAudioDevice(const std::string & deviceName);

	bool enableAEC(bool enable);

private:

	NullSipWrapper();
};

#endif	//NULLSIPWRAPPER_H
