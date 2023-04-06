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

#include "IAXClientWrapper.h"

#include <thread/Thread.h>
#include <util/StringList.h>
#include <util/Logger.h>
#include <sound/AudioDevice.h>
#include <sound/AudioDeviceManager.h>

#include "iaxclient.h"

#include <string>

IAXClientWrapper * IAXClientWrapper::_instance = NULL;

IAXClientWrapper::IAXClientWrapper()
	: _init(false),
	  _registered(false)
{
	LOG_DEBUG("IAXClient initialization");
	_instance = this;
}

IAXClientWrapper::~IAXClientWrapper() { }

void IAXClientWrapper::init()
{
	//TODO handle thread safety
	if (_init)
	{
		LOG_DEBUG("IAXClient is already initialized");
		return;
	}

	if ( iaxc_initialize( AUDIO_INTERNAL_PA, 1 ) < 0 )
	{
		LOG_DEBUG("Cannot initialize iaxclient!");
		return;
	}
/*
	iaxc_set_formats( IAXC_FORMAT_SPEEX, IAXC_FORMAT_SPEEX );


	iaxc_set_filters( IAXC_FILTER_DENOISE
					|IAXC_FILTER_AGC
					|IAXC_FILTER_AAGC
					|IAXC_FILTER_CN );
	//decode_enhance,quality,bitrate,vbr,abr,complexity
	iaxc_set_speex_settings(1,-1, -1, 0, 8000, 3);

	iaxc_set_silence_threshold( -99 );
  */

	iaxc_set_event_callback(&iaxc_callback);

	if ( iaxc_start_processing_thread() < 0) {
		LOG_DEBUG("There was an error starting iaxclient");
		return;
	}

	_init = true;
}

void IAXClientWrapper::terminate()
{
	//We should maybe unregister from the server
	iaxc_stop_processing_thread();
	iaxc_shutdown();
}

int IAXClientWrapper::addVirtualLine(const std::string & displayName,
	const std::string & username,
	const std::string & identity,
	const std::string & password,
	const std::string & realm,
	const std::string & proxyServer,
	const std::string & registerServer)
{
	_username = username;
	_displayName = displayName;
	_password = password;

	iaxc_set_callerid(const_cast<char*>(_displayName.c_str()),
					  const_cast<char*>(_displayName.c_str()));

	_lineId = iaxc_register(const_cast<char*>(_username.c_str()),
							const_cast<char*>(_password.c_str()),
							const_cast<char*>(_server.c_str()));

	phoneLineStateChangedEvent(this, _lineId, EnumPhoneLineState::PhoneLineStateProgress);

	return _lineId;
}

void IAXClientWrapper::removeVirtualLine(int lineId, bool now)
{
	iaxc_unregister(lineId);
	phoneLineStateChangedEvent(this, lineId, EnumPhoneLineState::PhoneLineStateClosed);
}

unsigned IAXClientWrapper::getLocalSIPPort() const
{
	return 0;
}

int IAXClientWrapper::makeCall(int lineId, const std::string & phoneNumber, bool enableVideo)
{
	std::string dialstring = _username + ":" + _password + "@" + _server + "/" + phoneNumber;
	LOG_DEBUG("call=" + dialstring);
	return iaxc_call(const_cast<char*>(dialstring.c_str()));
}

void IAXClientWrapper::sendRingingNotification(int callId)
{
	//Done my make call in IAXClient
}

void IAXClientWrapper::acceptCall(int callId, bool enableVideo)
{
	iaxc_answer_call(callId);
}

void IAXClientWrapper::rejectCall(int callId)
{
	//TODO what if not the active call?
	iaxc_dump_call();
}

void IAXClientWrapper::closeCall(int callId)
{
	//maybe we should dump call
	iaxc_reject_call_number(callId);
}

void IAXClientWrapper::holdCall(int callId)
{
	//Hold with music
	iaxc_quelch(callId, 1);

	//TODO persist the from
	phoneCallStateChangedEvent(this, callId, EnumPhoneCallState::PhoneCallStateHold, "");
}

void IAXClientWrapper::resumeCall(int callId)
{
	iaxc_unquelch(callId);

	//TODO persist the from
	phoneCallStateChangedEvent(this, callId, EnumPhoneCallState::PhoneCallStateTalking, "");
}

void IAXClientWrapper::playTone(int callId, EnumTone::Tone tone)
{
	char cTone = ' ';
	switch( tone) {
		case EnumTone::Tone0: cTone = '0'; break;
		case EnumTone::Tone1: cTone = '1'; break;
		case EnumTone::Tone2: cTone = '2'; break;
		case EnumTone::Tone3: cTone = '3'; break;
		case EnumTone::Tone4: cTone = '4'; break;
		case EnumTone::Tone5: cTone = '5'; break;
		case EnumTone::Tone6: cTone = '6'; break;
		case EnumTone::Tone7: cTone = '7'; break;
		case EnumTone::Tone8: cTone = '8'; break;
		case EnumTone::Tone9: cTone = '9'; break;
		case EnumTone::ToneStar: cTone = '*'; break;
		default: cTone = ' ';
		/*
		TonePound
		ToneDialtone
		ToneBusy
		ToneRingback
		ToneRingtone
		ToneCallFailed,
		ToneSilence
		ToneBackspace
		ToneCallWaiting
		ToneCallHeld
		ToneLoudFastBusy
		*/
	}
	iaxc_send_dtmf(cTone);
}

void IAXClientWrapper::playSoundFile(int callId, const std::string & soundFile)
{
	//NOT SUPPORTED BY IAXClient
}

void IAXClientWrapper::setHTTPProxy(const std::string & address,
								unsigned port,
								const std::string & login,
								const std::string & password)
{
	//NOT SUPPORTED BY IAXClient
}

void IAXClientWrapper::setTunnel(const std::string & address, unsigned port, bool ssl)
{
	//NOT SUPPORTED BY IAXClient
}

void IAXClientWrapper::setNatType(EnumNatType::NatType natType)
{
	//NO OPTION IN IAXClient
}

void IAXClientWrapper::setSIP(const std::string & server, unsigned serverPort, unsigned localPort)
{
	_server = server;
	_serverPort = serverPort;
	_localPort = localPort;
}

bool IAXClientWrapper::setCallInputAudioDevice(const AudioDevice & device)
{
	_inputAudioDevice = device;
	return setAudioDevices();
}

bool IAXClientWrapper::setRingerOutputAudioDevice(const AudioDevice & device)
{
	_ringAudioDevice = device;
	return setAudioDevices();
}

bool IAXClientWrapper::setCallOutputAudioDevice(const AudioDevice & device)
{
	_outputAudioDevice = device;
	return setAudioDevices();
}

bool IAXClientWrapper::setAudioDevices()
{
	int inputID, outputID, ringID;
	int nDevs;
	iaxc_audio_device* devs;

	iaxc_audio_devices_get( &devs, &nDevs, &inputID, &outputID, &ringID );
	for( int i = 0 ; i < nDevs ; i++ )
	{
		if (((String)_inputAudioDevice.getName()).contains(devs[i].name) && (devs[i].capabilities & IAXC_AD_INPUT)) {
			inputID = devs[i].devID;
			LOG_DEBUG( "inputID: " + String::fromNumber(inputID));
		}
		if (((String)_outputAudioDevice.getName()).contains(devs[i].name) && (devs[i].capabilities & IAXC_AD_OUTPUT)) {
			outputID = devs[i].devID;
			LOG_DEBUG( "outputID: " + String::fromNumber(outputID));
		}
		if (((String)_ringAudioDevice.getName()).contains(devs[i].name) && (devs[i].capabilities & IAXC_AD_RING)) {
			ringID = devs[i].devID;
			LOG_DEBUG( "ringID: " + String::fromNumber(ringID));
		}
	}

	return iaxc_audio_devices_set(inputID, outputID, ringID);
}

void IAXClientWrapper::enableAEC(bool enable)
{
	//Currently not supported by IAXCLient
}

void IAXClientWrapper::enableHalfDuplex(bool enable)
{
	//??
}

void IAXClientWrapper::setPluginPath(const std::string & path)
{
	//??
}

void IAXClientWrapper::blindTransfer(int callId, const std::string & sipAddress)
{
	iaxc_blind_transfer_call(callId, const_cast<char*>(sipAddress.c_str()));
}

CodecList::AudioCodec IAXClientWrapper::getAudioCodecUsed(int callId)
{
	//Currently not supported
	return CodecList::AudioCodecError;
}

CodecList::VideoCodec IAXClientWrapper::getVideoCodecUsed(int callId)
{
	//Currently not supported
	return CodecList::VideoCodecError;
}

void IAXClientWrapper::setVideoDevice(const std::string & deviceName)
{
	//Currently not supported
}

void IAXClientWrapper::setVideoQuality(EnumVideoQuality::VideoQuality videoQuality)
{
	//Currently not supported
}

int IAXClientWrapper::createConference()
{
	//Currently not supported
	return SipWrapper::ConfIdError;
}

void IAXClientWrapper::joinConference(int confId, int callId)
{
	//Currently not supported
}

void IAXClientWrapper::splitConference(int confId, int callId)
{
	//Currently not supported
}


//static callback method
int IAXClientWrapper::iaxc_callback(iaxc_event e)
{
	switch(e.type)
	{
		case IAXC_EVENT_REGISTRATION:
			_instance->registrationEvent( e.ev.reg );
			break;
		case IAXC_EVENT_STATE:
			_instance->stateEvent( e.ev.call );
			break;
		case IAXC_EVENT_LEVELS:
		case IAXC_EVENT_NETSTAT:
		case IAXC_EVENT_TEXT:
		case IAXC_EVENT_URL:
		case IAXC_EVENT_VIDEO:
			return 0;
	}
	return 0;
}

void IAXClientWrapper::registrationEvent(const iaxc_ev_registration &reg)
{
	switch( reg.reply )
	{
		case IAXC_REGISTRATION_REPLY_ACK:
			if (!_registered) {
				_registered = true;
				phoneLineStateChangedEvent(this, _lineId, EnumPhoneLineState::PhoneLineStateOk);
			}
			break;
		case IAXC_REGISTRATION_REPLY_REJ:
			_registered = false;
			phoneLineStateChangedEvent(this, _lineId, EnumPhoneLineState::PhoneLineStateServerError);
			break;
		case  IAXC_REGISTRATION_REPLY_TIMEOUT:
			_registered = false;
			phoneLineStateChangedEvent(this, _lineId, EnumPhoneLineState::PhoneLineStateTimeout);
			break;
		default:
			_registered = false;
			phoneLineStateChangedEvent(this, _lineId, EnumPhoneLineState::PhoneLineStateUnknown);
			LOG_DEBUG("Unhandled registration event");
			break;
	}
}

void IAXClientWrapper::stateEvent(const iaxc_ev_call_state & state)
{
	switch (state.state)
	{
		case IAXC_CALL_STATE_FREE:
			LOG_DEBUG("call hangup");
			phoneCallStateChangedEvent(this, state.callNo, EnumPhoneCallState::PhoneCallStateClosed, state.remote);
			break;
		case (IAXC_CALL_STATE_ACTIVE | IAXC_CALL_STATE_RINGING):
			LOG_DEBUG("incoming from call");
			phoneCallStateChangedEvent(this, state.callNo, EnumPhoneCallState::PhoneCallStateIncoming, state.remote);
			phoneCallStateChangedEvent(this, state.callNo, EnumPhoneCallState::PhoneCallStateRinging, state.remote);
			break;
		case (IAXC_CALL_STATE_ACTIVE | IAXC_CALL_STATE_COMPLETE):
			LOG_DEBUG("incoming answered");
			phoneCallStateChangedEvent(this, state.callNo, EnumPhoneCallState::PhoneCallStateTalking, state.remote);
			break;
		case (IAXC_CALL_STATE_ACTIVE | IAXC_CALL_STATE_OUTGOING | IAXC_CALL_STATE_RINGING):
			LOG_DEBUG("outgoing ringing");
			phoneCallStateChangedEvent(this, state.callNo, EnumPhoneCallState::PhoneCallStateRinging, state.remote);
			break;
		case (IAXC_CALL_STATE_ACTIVE | IAXC_CALL_STATE_COMPLETE | IAXC_CALL_STATE_OUTGOING):
			LOG_DEBUG("outgoing answered");
			phoneCallStateChangedEvent(this, state.callNo, EnumPhoneCallState::PhoneCallStateTalking, state.remote);
			break;
		default:
			LOG_DEBUG("UnHandle state event " + state.state);
			phoneCallStateChangedEvent(this, state.callNo, EnumPhoneCallState::PhoneCallStateUnknown, state.remote);
	}
}


