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

#ifndef OWPHAPIWRAPPER_H
#define OWPHAPIWRAPPER_H

#include <PhApiCallbacks.h>

#include <sipwrapper/SipWrapper.h>

#include <sound/AudioDevice.h>

#include <phapi.h>

#include <string>

class PhApiSFPWrapper;

/**
 * SIP wrapper for PhApi.
 *
 * PhApi is a SIP implementation developed in C.
 * PhApi is based on eXosip, oSIP and oRTP.
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 * @author Julien Bossart
 */
class PHAPIWRAPPER_API PhApiWrapper : public SipWrapper {
	Q_OBJECT
	//PhApiCallbacks is friend with PhApiWrapper
	//so that PhApiCallbacks can emit signals from SipWrapper
	friend class PhApiCallbacks;
public:

	/** Value when phApi returns an error message. */
	static const int PhApiResultNoError = 0;

	static PhApiWrapper * getInstance();

	~PhApiWrapper();

	void terminate();

	int addVirtualLine(const std::string & displayName,
		const std::string & username, const std::string & identity,
		const std::string & password, const std::string & realm,
		const std::string & proxyServer, const std::string & registerServer);

	bool registerVirtualLine(int lineId);

	void removeVirtualLine(int lineId, bool now = false);

	virtual void publishPresence(int lineId, bool online, const std::string & note);

	int makeCall(int lineId, const std::string & sipAddress, bool enableVideo);

	void sendRingingNotification(int callId, bool enableVideo);

	void acceptCall(int callId, bool enableVideo);

	void rejectCall(int callId);

	void closeCall(int callId);

	void holdCall(int callId);

	void resumeCall(int callId);

	void blindTransfer(int callId, const std::string & sipAddress);

	void playDtmf(int callId, char dtmf);

	void playSoundFile(int callId, const std::string & soundFile);

	void setAudioCodecList(const StringList & audioCodecList);

	CodecList::AudioCodec getAudioCodecUsed(int callId);

	CodecList::VideoCodec getVideoCodecUsed(int callId);

	void setVideoDevice(const std::string & deviceName);

	bool setCallInputAudioDevice(const AudioDevice & device);

	bool setRingerOutputAudioDevice(const AudioDevice & device);

	bool setCallOutputAudioDevice(const AudioDevice & device);

	void enableAEC(bool enable);

	void enableHalfDuplex(bool enable);

	void enablePIM(bool enable);

	void setPluginPath(const std::string & path);

	virtual std::string getPluginPath();

	void setHTTPProxy(const std::string & address, unsigned port,
		const std::string & login, const std::string & password);

	void setTunnel(const std::string & address, unsigned port, bool ssl);

	void setNatType(EnumNatType::NatType natType);

	void setVideoQuality(EnumVideoQuality::VideoQuality videoQuality);

	/**
	 * @see SipWrapper::setCallsEncryption
	 */
	virtual void setCallsEncryption(bool enable);

	/**
	 * @see SipWrapper::areCallsEncrypted
	 */
	virtual bool isCallEncrypted(int callId);

	/**
	 * @name Conference
	 * @{
	 */

	int createConference();

	void joinConference(int confId, int callId);

	void splitConference(int confId, int callId);

	/** @} */

	/**
	 * @name PhApi callbacks
	 * @{
	 */

	void init();

	/** @} */

	/**
	 * Helper function for the enum phCallStateEvent.
	 *
	 * @param event a phCallStateEvent to "convert" into a string
	 * @return a string representing the enum
	 */
	static std::string phapiCallStateToString(enum phCallStateEvent event);

	static PhApiWrapper * PhApiWrapperHack;

	bool isRegistered() const;
	void setRegistered(bool registered);

	void flipVideoImage(bool flip);

private Q_SLOTS:

	//FIXME ugly hack for call conference
	void phoneCallStateChangedSlot(int callId,
		EnumPhoneCallState::PhoneCallState state, std::string from);

private:

	/**
	 * Set phApi with parameters previously set with setProxy, etc. methods.
	 */
	void setNetworkParameter();

	PhApiWrapper(PhApiCallbacks & callbacks);

	/** Changes audio devices inside PhApi. */
	bool setAudioDevices();

	PhApiCallbacks * _callbacks;

	/** Checks if phApi has been initialized: if phInit() has been called. */
	bool _isInitialized;

	/** Playback audio device. */
	AudioDevice _outputAudioDevice;

	/** Record audio device. */
	AudioDevice _inputAudioDevice;

	std::string _proxyServer;

	unsigned _proxyPort;

	std::string _proxyLogin;

	std::string _proxyPassword;

	bool _tunnelNeeded;

	std::string _tunnelServer;

	unsigned _tunnelPort;

	bool _tunnelSSL;

	EnumNatType::NatType _natType;

	std::string _pluginPath;

	bool _registered;

	PhApiSFPWrapper & _phApiSFPWrapper;
};

#endif	//OWPHAPIWRAPPER_H
