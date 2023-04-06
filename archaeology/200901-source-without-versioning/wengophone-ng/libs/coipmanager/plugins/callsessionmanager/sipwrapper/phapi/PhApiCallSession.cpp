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

#include "PhApiCallSession.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager/CoIpManagerConfig.h>

#include <coipmanager_base/account/SipAccount.h>

#include <sipwrapper/SipWrapper.h>

#include <sound/AudioDevice.h>

#include <util/String.h>
#include <util/Logger.h>

PhApiCallSession::PhApiCallSession(CoIpManager & coIpManager, SipWrapper * phApiWrapper,
	int callId, EnumPhoneCallState::PhoneCallState state, const std::string & from)
	: SipWrapperCallSession(coIpManager, phApiWrapper, callId, state, from) {

	IMContact contact(EnumAccountType::AccountTypeSIP, from);
	addIMContact(contact);
}

PhApiCallSession::PhApiCallSession(CoIpManager & coIpManager, SipWrapper * phApiWrapper)
	: SipWrapperCallSession(coIpManager, phApiWrapper) {
}

PhApiCallSession::~PhApiCallSession() {
}

void PhApiCallSession::start() {
	LOG_DEBUG("starts the call session");

	// Sets devices to use
	CoIpManagerConfig &config = _coIpManager.getCoIpManagerConfig();
	_sipWrapper->setCallOutputAudioDevice(AudioDevice(config.getAudioOutputDeviceId()));
	_sipWrapper->setCallInputAudioDevice(AudioDevice(config.getAudioInputDeviceId()));
	_sipWrapper->setRingerOutputAudioDevice(AudioDevice(config.getAudioRingerDeviceId()));

	_sipWrapper->setVideoDevice(config.getVideoWebcamDevice());
	_sipWrapper->setVideoQuality(EnumVideoQuality::toVideoQuality(config.getVideoQuality()));
	////

	if (_isIncoming) {
		if (_state != EnumPhoneCallState::PhoneCallStateIncoming) {
			LOG_ERROR("phone call already answered");
			return;
		}
		_sipWrapper->acceptCall(_callId, false);
	}

	//Outgoing
	else {
		SipAccount * sipAccount = NULL;
		Account account = getAccount();
		if (account.getAccountType() == EnumAccountType::AccountTypeSIP || account.getAccountType() == EnumAccountType::AccountTypeWengo) {
			sipAccount = static_cast<SipAccount *>(account.getPrivateAccount());
		} else {
			LOG_FATAL("Account is not a SipAccount");
		}

		//Mutiple contacts = conference mode
		if (_imContactList.size() > 1) {
			LOG_DEBUG("create conference call with " + String::fromNumber(_imContactList.size()) + " people");
			int confId = _sipWrapper->createConference();
			if (confId == SipWrapper::ConfIdError) {
				LOG_ERROR("couldn't create the conference");
				return;
			}

			for (IMContactList::const_iterator it = _imContactList.begin(); it != _imContactList.end(); it++) {
				int callId = _sipWrapper->makeCall(sipAccount->getVirtualLineId(), (*it).getContactId(), _videoEnabled);
				if (callId == SipWrapper::CallIdError) {
					LOG_ERROR("couldn't create the phone call");
					return;
				}
				_sipWrapper->holdCall(callId);

				LOG_DEBUG("join conference=" + String::fromNumber(callId));
				_sipWrapper->joinConference(confId, callId);
			}
		}

		else {
			_callId = _sipWrapper->makeCall(sipAccount->getVirtualLineId(), _imContactList.front().getContactId(), _videoEnabled);
			if (_callId == SipWrapper::CallIdError) {
				LOG_ERROR("couldn't make call");
			}
		}
	}
}
