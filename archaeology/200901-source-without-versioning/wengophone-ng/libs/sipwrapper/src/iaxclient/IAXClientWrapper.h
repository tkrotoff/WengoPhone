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

#ifndef IAXCLIENTWRAPPER_H
#define IAXCLIENTWRAPPER_H

#include <sipwrapper/SipWrapper.h>

#include <sound/AudioDevice.h>
#include <thread/Mutex.h>
#include <thread/Timer.h>
#include <event/Event2.h>

#include <iaxclient.h>

/**
 * IAXClient wrapper.
 *
 *
 * @ingroup model
 * @author Yann Biancheri
 */
class IAXClientWrapper : public SipWrapper {
public:

	static IAXClientWrapper * getInstance() {
		static IAXClientWrapper instance;

		return &instance;
	}

	~IAXClientWrapper();

	void init();

	void terminate();

	int addVirtualLine(const std::string & displayName,
		const std::string & username, const std::string & identity,
		const std::string & password, const std::string & realm,
		const std::string & proxyServer, const std::string & registerServer);

	void removeVirtualLine(int lineId, bool now = false);

	unsigned getLocalSIPPort() const;

	int makeCall(int lineId, const std::string & phoneNumber, bool enableVideo);

	void sendRingingNotification(int callId);

	void acceptCall(int callId, bool enableVideo);

	void rejectCall(int callId);

	void closeCall(int callId);

	void holdCall(int callId);

	void resumeCall(int callId);

	void playTone(int callId, EnumTone::Tone tone);

	void playSoundFile(int callId, const std::string & soundFile);

	void setHTTPProxy(const std::string & address, unsigned port,
		const std::string & login, const std::string & password);

	void setTunnel(const std::string & address, unsigned port, bool ssl);

	void setNatType(EnumNatType::NatType natType);

	void setSIP(const std::string & server, unsigned serverPort, unsigned localPort);


	bool setCallInputAudioDevice(const AudioDevice & device);
	bool setRingerOutputAudioDevice(const AudioDevice & device);
	bool setCallOutputAudioDevice(const AudioDevice & device);

	void enableAEC(bool enable);
	void enableHalfDuplex(bool enable);
	void setPluginPath(const std::string & path);

	void blindTransfer(int callId, const std::string & sipAddress);
	CodecList::AudioCodec getAudioCodecUsed(int callId);
	CodecList::VideoCodec getVideoCodecUsed(int callId);
	void setVideoDevice(const std::string & deviceName);
	void setVideoQuality(EnumVideoQuality::VideoQuality videoQuality);
	int createConference();
	void joinConference(int confId, int callId);
	void splitConference(int confId, int callId);

	virtual void flipVideoImage(bool flip) {}

	virtual void sendRingingNotification(int callId, bool enableVideo) {}

private:

	static IAXClientWrapper * _instance;

	std::string _server;
	unsigned _serverPort;
	unsigned _localPort;
	std::string _displayName;
	std::string _username;
	std::string _password;
	int _lineId;
	bool _init;
	bool _registered;

	/** Ring audio device. */
	AudioDevice _ringAudioDevice;

	/** Playback audio device. */
	AudioDevice _outputAudioDevice;

	/** Record audio device. */
	AudioDevice _inputAudioDevice;


	void registrationEvent(const iaxc_ev_registration & reg);
	void stateEvent(const iaxc_ev_call_state & state);
	bool setAudioDevices();

	/** IAXClient callback */
	static int iaxc_callback( iaxc_event e );

	IAXClientWrapper();
};

#endif
