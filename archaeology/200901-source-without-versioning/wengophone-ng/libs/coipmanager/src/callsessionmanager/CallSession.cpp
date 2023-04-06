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

#include <coipmanager/callsessionmanager/CallSession.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/callsessionmanager/CallSessionManager.h>
#include <coipmanager/callsessionmanager/ICallSessionPlugin.h>
#include <coipmanager/datamanager/UserProfileManager.h>
#include <coipmanager/notification/Notification.h>

#include <coipmanager_base/EnumAccountType.h>
#include <coipmanager_base/account/Account.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

CallSession::CallSession(CoIpManager & coIpManager)
	: Session(coIpManager) {
	_currentCallSession = NULL;
	_videoEnabled = false;
}

CallSession::CallSession(CoIpManager & coIpManager, ICallSessionPlugin * currentCallSession)
	: Session(coIpManager) {
	_currentCallSession = currentCallSession;
	SAFE_CONNECT(_currentCallSession, SIGNAL(phoneCallStateChangedSignal(EnumPhoneCallState::PhoneCallState)),
		SLOT(phoneCallStateChangedSlot(EnumPhoneCallState::PhoneCallState)));
	SAFE_CONNECT(_currentCallSession, SIGNAL(videoFrameReceivedSignal(piximage *, piximage *)),
		SIGNAL(videoFrameReceivedSignal(piximage *, piximage *)));
}

CallSession::~CallSession() {
	OWSAFE_DELETE(_currentCallSession);
}

void CallSession::start() {
	QMutexLocker lock(_mutex);

	if (_currentCallSession) {
		if (_currentCallSession->isStarted()) {
			LOG_ERROR("CallSession already started");
			return;
		}

		//incoming call
		_currentCallSession->start();
		_currentCallSession->setStarted(true);
		return;
	}

	//outgoing call
	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	Account * account = AccountListHelper::getCopyOfAccount(accountList, _accountToUse);
	if (account) {
		ICallSessionManagerPlugin *iCallSessionManager =
			_coIpManager.getCallSessionManager().getICoIpManagerPlugin(*account, _contactList);
		if (iCallSessionManager) {
			_currentCallSession = iCallSessionManager->createICallSessionPlugin();
			IMContactList imContactList = iCallSessionManager->getValidIMContacts(*account, _contactList);
			_currentCallSession->setIMContactList(imContactList);
			_currentCallSession->setAccount(*account);

			SAFE_CONNECT(_currentCallSession, SIGNAL(phoneCallStateChangedSignal(EnumPhoneCallState::PhoneCallState)),
				SLOT(phoneCallStateChangedSlot(EnumPhoneCallState::PhoneCallState)));
			SAFE_CONNECT(_currentCallSession, SIGNAL(videoFrameReceivedSignal(piximage *, piximage *)),
				SIGNAL(videoFrameReceivedSignal(piximage *, piximage *)));

			_currentCallSession->enableVideo(_videoEnabled);
			_currentCallSession->start();
			_currentCallSession->setStarted(true);
		}
		OWSAFE_DELETE(account);
	} else {
		LOG_ERROR("account \"" + _accountToUse + "\" does not exist");
	}
}

void CallSession::stop() {
	QMutexLocker lock(_mutex);

	if (_currentCallSession) {
		if (!_currentCallSession->isStarted()) {
			LOG_ERROR("CallSession not started");
		} else {
			_currentCallSession->setStarted(false);
			_currentCallSession->stop();
		}
	} else {
		LOG_ERROR("no current CallSession");
	}
}

void CallSession::pause() {
	QMutexLocker lock(_mutex);

	if (_currentCallSession) {
		if (!_currentCallSession->isStarted()) {
			LOG_ERROR("CallSession not started");
		} else {
			_currentCallSession->pause();
		}
	} else {
		LOG_ERROR("no current CallSession");
	}
}

void CallSession::resume() {
	QMutexLocker lock(_mutex);

	if (_currentCallSession) {
		if (!_currentCallSession->isStarted()) {
			LOG_ERROR("CallSession not started");
		} else {
			_currentCallSession->resume();
		}
	} else {
		LOG_ERROR("no current CallSession");
	}
}

bool CallSession::canCreateISession(const Account & account, const ContactList & contactList) const {
	return (_coIpManager.getCallSessionManager().getICoIpManagerPlugin(account, contactList) != NULL);
}

EnumPhoneCallState::PhoneCallState CallSession::getState() const {
	QMutexLocker lock(_mutex);

	EnumPhoneCallState::PhoneCallState result = EnumPhoneCallState::PhoneCallStateUnknown;

	if (_currentCallSession) {
		if (!_currentCallSession->isStarted()) {
			LOG_ERROR("CallSession not started");
		} else {
			result = _currentCallSession->getState();
		}
	} else {
		LOG_ERROR("no current CallSession");
	}

	return result;
}

void CallSession::playTone(EnumTone::Tone tone) {
	QMutexLocker lock(_mutex);

	if (_currentCallSession) {
		if (!_currentCallSession->isStarted()) {
			LOG_ERROR("CallSession not started");
		} else {
			_currentCallSession->playTone(tone);
		}
	} else {
		LOG_ERROR("no current CallSession");
	}
}

void CallSession::playSoundFile(const std::string & soundFile) {
	QMutexLocker lock(_mutex);

	if (_currentCallSession) {
		if (!_currentCallSession->isStarted()) {
			LOG_ERROR("CallSession not started");
		} else {
			_currentCallSession->playSoundFile(soundFile);
		}
	} else {
		LOG_ERROR("no current CallSession");
	}
}

CodecList::AudioCodec CallSession::getAudioCodecUsed() const {
	QMutexLocker lock(_mutex);

	CodecList::AudioCodec result = CodecList::AudioCodecError;

	if (_currentCallSession) {
		if (!_currentCallSession->isStarted()) {
			LOG_ERROR("CallSession not started");
		} else {
			result = _currentCallSession->getAudioCodecUsed();
		}
	} else {
		LOG_ERROR("no current CallSession");
	}

	return result;
}

CodecList::VideoCodec CallSession::getVideoCodecUsed() const {
	QMutexLocker lock(_mutex);

	CodecList::VideoCodec result = CodecList::VideoCodecError;

	if (_currentCallSession) {
		if (!_currentCallSession->isStarted()) {
			LOG_ERROR("CallSession not started");
		} else {
			result = _currentCallSession->getVideoCodecUsed();
		}
	} else {
		LOG_ERROR("no current CallSession");
	}

	return result;
}

void CallSession::enableVideo(bool enable) {
	QMutexLocker lock(_mutex);

	if (_currentCallSession) {
		_currentCallSession->enableVideo(enable);
	} else {
		LOG_ERROR("no current CallSession");
	}
	_videoEnabled = enable;
}

bool CallSession::isVideoEnabled() const {
	QMutexLocker lock(_mutex);

	bool result = false;

	if (_currentCallSession) {
		if (!_currentCallSession->isStarted()) {
			LOG_ERROR("CallSession not started");
		} else {
			result = _currentCallSession->isVideoEnabled();
		}
	} else {
		LOG_ERROR("no current CallSession");
	}

	return result;
}

int CallSession::getDuration() const {
	QMutexLocker lock(_mutex);

	int result = 0;

	if (_currentCallSession) {
		if (!_currentCallSession->isStarted()) {
			LOG_ERROR("CallSession not started");
		} else {
			result = _currentCallSession->getDuration();
		}
	} else {
		LOG_ERROR("no current CallSession");
	}

	return result;
}

void CallSession::phoneCallStateChangedSlot(EnumPhoneCallState::PhoneCallState state) {
	switch (state) {
	case EnumPhoneCallState::PhoneCallStateIncoming:
		Notification::getInstance().postNotification(EnumNotificationType::NotificationTypeCallSessionIncoming, getUUID());
		break;

	case EnumPhoneCallState::PhoneCallStateClosed:
		Notification::getInstance().postNotification(EnumNotificationType::NotificationTypeCallSessionClosed, getUUID());
		//Stops the current CallSession
		stop();
		break;
	default:
		// Nothing to notify
		break;
	};

	phoneCallStateChangedSignal(state);
}
