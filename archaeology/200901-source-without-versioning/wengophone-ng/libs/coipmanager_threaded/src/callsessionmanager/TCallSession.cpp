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

#include <coipmanager_threaded/callsessionmanager/TCallSession.h>

#include <coipmanager_threaded/TCoIpManager.h>
#include <coipmanager_threaded/callsessionmanager/TCallSessionManager.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/callsessionmanager/CallSession.h>
#include <coipmanager/callsessionmanager/CallSessionManager.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/SafeConnect.h>

TCallSession::TCallSession(TCoIpManager & tCoIpManager, CallSession * callSession)
	: TSession(tCoIpManager, callSession) {

	SAFE_CONNECT(callSession, SIGNAL(phoneCallStateChangedSignal(EnumPhoneCallState::PhoneCallState)),
		SIGNAL(phoneCallStateChangedSignal(EnumPhoneCallState::PhoneCallState)));
	SAFE_CONNECT(callSession, SIGNAL(videoFrameReceivedSignal(piximage *, piximage *)),
		SLOT(videoFrameReceivedSlot(piximage *, piximage *)));
}

TCallSession::~TCallSession() {
}

void TCallSession::videoFrameReceivedSlot(piximage * remoteImage, piximage * localImage) {
	piximage * remoteImageRGB = pix_alloc(PIX_OSI_ARGB32, remoteImage->width, remoteImage->height);
	pix_convert(PIX_NO_FLAG, remoteImageRGB, remoteImage);

	piximage * localImageRGB = pix_alloc(PIX_OSI_ARGB32, localImage->width, localImage->height);
	pix_convert(PIX_NO_FLAG, localImageRGB, localImage);
	videoFrameReceivedSignal(QImage(remoteImageRGB->data, remoteImageRGB->width, remoteImageRGB->height, QImage::Format_ARGB32),
		QImage(localImageRGB->data, localImageRGB->width, localImageRGB->height, QImage::Format_ARGB32));
}

CallSession * TCallSession::getCallSession() const {
	return static_cast<CallSession *>(_module);
}

EnumPhoneCallState::PhoneCallState TCallSession::getState() const {
	return getCallSession()->getState();
}

void TCallSession::playTone(EnumTone::Tone tone) {
	getCallSession()->playTone(tone);
}

void TCallSession::playSoundFile(const std::string & soundFile) {
	getCallSession()->playSoundFile(soundFile);
}

CodecList::AudioCodec TCallSession::getAudioCodecUsed() const {
	return getCallSession()->getAudioCodecUsed();
}

CodecList::VideoCodec TCallSession::getVideoCodecUsed() const {
	return getCallSession()->getVideoCodecUsed();
}

void TCallSession::enableVideo(bool enable) {
	getCallSession()->enableVideo(enable);
}

bool TCallSession::isVideoEnabled() const {
	return getCallSession()->isVideoEnabled();
}

int TCallSession::getDuration() const {
	return getCallSession()->getDuration();
}
