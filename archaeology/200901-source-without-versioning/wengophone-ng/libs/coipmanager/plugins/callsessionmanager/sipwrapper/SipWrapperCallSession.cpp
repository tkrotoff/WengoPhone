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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  US
 */

#include "SipWrapperCallSession.h"

#include <coipmanager_base/EnumAccountType.h>

#include <sipwrapper/SipWrapper.h>
#include <sipwrapper/EnumPhoneCallState.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>

SipWrapperCallSession::SipWrapperCallSession(CoIpManager & coIpManager, SipWrapper * sipWrapper,
	int callId, EnumPhoneCallState::PhoneCallState state, const std::string & from)
	: ICallSessionPlugin(coIpManager),
	_sipWrapper(sipWrapper),
	_callId(callId),
	_state(state),
	_isIncoming(true) {

	initialize();
}

SipWrapperCallSession::SipWrapperCallSession(CoIpManager & coIpManager, SipWrapper * sipWrapper)
	: ICallSessionPlugin(coIpManager),
	_sipWrapper(sipWrapper),
	_callId(SipWrapper::CallIdError),
	_state(EnumPhoneCallState::PhoneCallStateUnknown),
	_isIncoming(false) {

	initialize();
}

void SipWrapperCallSession::initialize() {
	SAFE_CONNECT(_sipWrapper, SIGNAL(phoneCallStateChangedSignal(int, EnumPhoneCallState::PhoneCallState, std::string)),
		SLOT(phoneCallStateChangedSlot(int, EnumPhoneCallState::PhoneCallState, std::string)));
	SAFE_CONNECT(_sipWrapper, SIGNAL(videoFrameReceivedSignal(int, piximage *, piximage *)),
		SLOT(videoFrameReceivedSlot(int, piximage *, piximage *)));
}

SipWrapperCallSession::~SipWrapperCallSession() {
}

void SipWrapperCallSession::stop() {
	/*_sipWrapper->phoneCallStateChangedEvent.disconnect(this);
	_sipWrapper->videoFrameReceivedEvent.disconnect(this);*/
	if (_callId == SipWrapper::CallIdError) {
		LOG_ERROR("no valid phone call, use start() before");
	}
	_sipWrapper->closeCall(_callId);
}

void SipWrapperCallSession::pause() {
	if (_state != EnumPhoneCallState::PhoneCallStateTalking) {
		LOG_ERROR("phone call not in talking state, cannot hold it");
		return;
	}
	_sipWrapper->holdCall(_callId);
}

void SipWrapperCallSession::resume() {
	if (_state != EnumPhoneCallState::PhoneCallStateHold) {
		LOG_ERROR("phone call not in talking state, cannot resume it");
		return;
	}

	_sipWrapper->resumeCall(_callId);
}

EnumPhoneCallState::PhoneCallState SipWrapperCallSession::getState() const {
	return _state;
}

void SipWrapperCallSession::playTone(EnumTone::Tone tone) {
	if (_state != EnumPhoneCallState::PhoneCallStateTalking) {
		LOG_ERROR("phone call not in talking state, cannot play tone");
		return;
	}

	_sipWrapper->playDtmf(_callId, tone);
}

void SipWrapperCallSession::playSoundFile(const std::string & soundFile) {
	if (_state != EnumPhoneCallState::PhoneCallStateTalking) {
		LOG_ERROR("phone call not in talking state, cannot play sound file");
		return;
	}

	_sipWrapper->playSoundFile(_callId, soundFile);
}

CodecList::AudioCodec SipWrapperCallSession::getAudioCodecUsed() const {
	if (_state != EnumPhoneCallState::PhoneCallStateTalking) {
		LOG_ERROR("phone call not in talking state");
		return CodecList::AudioCodecError;
	}

	return _sipWrapper->getAudioCodecUsed(_callId);
}

CodecList::VideoCodec SipWrapperCallSession::getVideoCodecUsed() const {
	if (_state != EnumPhoneCallState::PhoneCallStateTalking) {
		LOG_ERROR("phone call not in talking state");
		return CodecList::VideoCodecError;
	}

	return _sipWrapper->getVideoCodecUsed(_callId);
}

void SipWrapperCallSession::enableVideo(bool enable) {
	_videoEnabled = enable;
}

bool SipWrapperCallSession::isVideoEnabled() const {
	if (_state != EnumPhoneCallState::PhoneCallStateTalking) {
		LOG_ERROR("phone call not in talking state");
		return false;
	}

	return _videoEnabled;
}

int SipWrapperCallSession::getDuration() const {
	return _duration;
}

void SipWrapperCallSession::phoneCallStateChangedSlot(int callId,
	EnumPhoneCallState::PhoneCallState state, std::string from) {

	if (sender() != _sipWrapper) {
		LOG_ERROR("phoneCallStateChangedSignal() ignored since it is not coming from our wrapper");
		return;
	}

	if (_callId != callId) {
		LOG_ERROR("phoneCallStateChangedSignal() ignored since it is not coming from our phone call");
		return;
	}

	_state = state;
	phoneCallStateChangedSignal(state);
}

void SipWrapperCallSession::videoFrameReceivedSlot(int callId, piximage * remoteVideoFrame, piximage * localVideoFrame) {
	if (sender() != _sipWrapper) {
		LOG_ERROR("videoFrameReceivedSignal() ignored since it is not coming from our wrapper");
		return;
	}

	if (_callId != callId) {
		LOG_DEBUG("videoFrameReceivedSignal() ignored since it is not coming from our phone call");
		return;
	}

	videoFrameReceivedSignal(remoteVideoFrame, localVideoFrame);
}
