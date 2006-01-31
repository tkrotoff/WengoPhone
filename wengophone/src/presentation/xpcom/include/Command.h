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

#ifndef COMMAND_H
#define COMMAND_H

#include "Listener.h"

#include <dllexport.h>

class WengoPhone;
class CWengoPhone;
class SoundMixer;
class MemoryDump;

#ifdef XPCOM_DLL
	#ifdef BUILD_XPCOM_DLL
		#define API DLLEXPORT
	#else
		#define API DLLIMPORT
	#endif
#else
	#define API
#endif

/**
 * Commands to send to WengoPhone.
 *
 * @author Tanguy Krotoff
 */
class API Command {
public:

	Command(const std::string & configFilesPath);

	~Command();

	/**
	 * Starts the model thread + the SIP stack.
	 *
	 * To do before everything else.
	 */
	void start();

	/**
	 * HTTP proxy settings to use for all the HTTP requests.
	 *
	 * @param hostname HTTP proxy hostname
	 * @param port HTTP proxy port number
	 * @param login HTTP proxy login (can be empty)
	 * @param password HTTP proxy password (can be empty)
	 */
	void setHttpProxySettings(const std::string & hostname, unsigned port,
				const std::string & login, const std::string & password);

	/**
	 * Terminates the model thread + the SIP stack.
	 */
	void terminate();

	/**
	 * Adds a Wengo account.
	 *
	 * @param login Wengo account login
	 * @param password Wengo account password
	 * @param autoLogin if true login the user automatically (save it to the file)
	 */
	void addWengoAccount(const std::string & login, const std::string & password, bool autoLogin);

	/**
	 * @name Listener Methods
	 * @{
	 */

	/**
	 * @see Listener
	 */
	void addListener(Listener * listener);

	/**
	 * @see Listener
	 */
	bool removeListener(Listener * listener);

	/**
	 * @see Listener
	 */
	void removeAllListeners();

	/** @} */

	/**
	 * @name Call Methods
	 * @{
	 */

	/** An error occured while creating a phone call. */
	static const int CallIdError = -1;

	/**
	 * Makes a call given a phone number.
	 *
	 * Creates a new phone call with a new call id (increased each time).
	 *
 	 * Warning!
	 * Be sure to receive PhoneLineState::LineOk before to call this method.
	 *
	 * @param phoneNumber phone number to call
	 * @param lineId phone line id
	 * @return the phone call id (callId) or CallIdError if an error occured
	 */
	int makeCall(const std::string & phoneNumber, int lineId);

	/**
	 * Raccrocher.
	 *
	 * Can correspond to reject or close call depending on PhoneCallState.
	 *
	 * @param callId phone call id
	 */
	void hangUp(int callId);

	/**
	 * S'il est possible de raccrocher le telephone.
	 *
	 * @param callId phone call id
	 */
	bool canHangUp(int callId) const;

	/**
	 * Decrocher.
	 *
	 * Corresponds to accept call.
	 *
	 * @param callId phone call id
	 */
	void pickUp(int callId);

	/**
	 * S'il est possible de decrocher le telephone.
	 *
	 * @param callId phone call id
	 */
	bool canPickUp(int callId) const;

	/** @} */

	/**
	 * @name Presence Methods
	 * @{
	 */

	/**
	 * Subscribes for the presence state of somebody.
	 *
	 * Warning!
	 * Be sure to receive PhoneLineState::LineOk before to call this method.
	 *
	 * @param contactId person I want to see
	 */
	void subscribeToPresenceOf(const std::string & contactId);

	/**
	 * Sets my presence status.
	 *
	 * Warning!
	 * Be sure to receive PhoneLineState::LineOk before to call this method.
	 *
	 * @param note personnalized status string ("I'm under the shower"), used only with PresenceUserDefined
	 */
	void publishMyPresence(Listener::PresenceState state, const std::string & note);

	/** @} */

	/** 
	 * @name Chat Methods
	 * @{
	 */

	/**
	 * Sends a chat message.
	 *
	 * Warning!
	 * Be sure to receive PhoneLineState::LineOk before to call this method.
	 *
	 * @param contactId message receiver
	 * @param message message to send
	 * @return message id
	 */
	int sendChatMessage(const std::string & contactId, const std::string & message);

	/** @} */

	/** 
	 * @name Sound Mixer Configuration
	 * @{
	 */

	/**
	 * Sets the microphone volume.
	 *
	 * @param volume new input volume (0 to 100)
	 */
	void setInputVolume(int volume);

	/**
	 * Gets the input (microphone) volume.
	 *
	 * @return the input volume (0 to 100), -1 if an error occured
	 */
	int getInputVolume();

	/**
	 * Gets the output (master or wave out) volume.
	 *
	 * @return the output volume (0 to 100), -1 if an error occured
	 */
	int getOutputVolume();

	/**
	 * Sets the master or wave out volume.
	 *
	 * @param volume new output volume (0 to 100)
	 */
	void setOutputVolume(int volume);

	/** @} */

	/** 
	 * @name SMS Methods
	 * @{
	 */

	void sendSMS(const std::string & phoneNumber, const std::string & message) { }

	/** @} */

private:

	WengoPhone * _wengoPhone;

	CWengoPhone * _cWengoPhone;

	SoundMixer * _soundMixer;

	MemoryDump * _memoryDump;
};

#endif	//COMMAND_H
