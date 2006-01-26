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

#ifndef PHAPIWRAPPER_H
#define PHAPIWRAPPER_H

#include "PhApiCallbacks.h"

#include <model/sipwrapper/SipWrapper.h>

#include <model/imwrapper/EnumPresenceState.h>
#include <model/imwrapper/IMChat.h>
#include <model/imwrapper/IMConnect.h>
#include <model/imwrapper/IMPresence.h>

#include <Event.h>

#include <phapi.h>

/**
 * SIP wrapper for PhApi.
 *
 * PhApi is a SIP implementation developed in C.
 * PhApi is based on eXosip, oSIP and oRTP.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class PhApiWrapper : public SipWrapper {
public:

	Event<void (PhApiWrapper & sender, const std::string & from, const std::string & message)> messageReceivedEvent;

	Event<void (PhApiWrapper & sender, IMChat::MessageStatus status, int messageId)> messageStatusEvent;

	Event<void (PhApiWrapper & sender, IMConnect::LoginStatus status)> loginStatusEvent;

	Event<void (PhApiWrapper & sender, EnumPresenceState::PresenceState state, const std::string & note, const std::string & from)> presenceStateChangedEvent;

	Event<void (PhApiWrapper & sender, EnumPresenceState::MyPresenceStatus status)> myPresenceStatusEvent;

	Event<void (PhApiWrapper & sender, const std::string & contactId, IMPresence::SubscribeStatus status)> subscribeStatusEvent;


	/** Value when phApi returns an error message. */
	static const int PhApiResultNoError = 0;

	static PhApiWrapper * getInstance(SipCallbacks & callbacks) {
		static PhApiCallbacks phApiCallbacks(callbacks);
		static PhApiWrapper instance(phApiCallbacks);

		return &instance;
	}

	~PhApiWrapper();

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

	static void callProgress(int callId, const phCallStateInfo_t * info);

	static void registerProgress(int lineId, int status);

	static void videoFrameReceived(int callId, phVideoFrameReceivedEvent_t * info);

	static void messageProgress(int messageId, const phMsgStateInfo_t * info);

	static void subscriptionProgress(int subscriptionId, const phSubscriptionStateInfo_t * info);

	static void onNotify(const char * event, const char * from, const char * content);

	/*
	 * IMConnect implementation
	 */

	void connect();
	void disconnect();

	/*
	 * IMChat implementation
	 */

	int sendMessage(const std::string & contactId, const std::string & message);

	/*
	 * IMPresence implementation
	 */

	void changeMyPresence(EnumPresenceState::PresenceState state, const std::string & note);

	void subscribeToPresenceOf(const std::string & contactId);

	void blockContact(const std::string & contactId);

	void unblockContact(const std::string & contactId);

	void allowWatcher(const std::string & watcher);

	static PhApiWrapper * PhApiWrapperHack;

	static const std::string PresenceStateOnline;
	static const std::string PresenceStateAway;
	static const std::string PresenceStateDoNotDisturb;

private:

	PhApiWrapper(PhApiCallbacks & callbacks);

	void detectNetworkConfig();

	static PhApiCallbacks * _callbacks;

	bool _isInitialized;

	void publishOnline(const std::string & note);

	void publishOffline(const std::string & note);

	int _wengoVline;

	std::string _wengoSipAddress;

	std::string _realm;
};

#endif	//PHAPIWRAPPER_H
