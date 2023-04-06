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

#include "AudioCallManager.h"

#include "SessionWindow.h"
#include "sip/SipPrimitives.h"
#include "sip/SipAddress.h"
#include "sip/PhApiCallbacks.h"
#include "util/emit_signal.h"
#include "contact/ContactList.h"
#include "contact/Contact.h"
#include "StatusBar.h"
#include "Softphone.h"
#include "MyWidgetFactory.h"
#include "config/Video.h"

#include <qpushbutton.h>
#include <qstring.h>
#include <qmessagebox.h>

#include <qmainwindow.h>
#include "MainWindow.h"

#include <phapi.h>

#include <cassert>
#include <algorithm>
#include <iostream>
using namespace std;

AudioCallManager::AudioCallManager() {
	PhApiCallbacks::setAudioCallManager(*this);

	_activeAudioCall = AudioCall::NOT_CALLING;
}

AudioCallManager::~AudioCallManager() {
	//Deletes all the AudioCall
	for (unsigned int i = 0; i < _audioCallList.size(); i++) {
		delete _audioCallList[i];
	}

	_audioCallList.clear();
}

void AudioCallManager::update(const Subject & subject, const Event & event) {
	const AudioCallEvent & evt = (const AudioCallEvent &)event;
	AudioCallEvent::Kind kind = evt.getKind();

	const AudioCall & audioCall = (const AudioCall &)subject;

	switch (kind) {
	case AudioCallEvent::CallIdChanged:
		if (_activeAudioCall == audioCall.getPreviousCallId()) {
			_activeAudioCall = audioCall.getCallId();
		}
		break;

	/*default:
		assert(NULL && "Unknown event");*/
	}
}

AudioCall * AudioCallManager::createAudioCall() {
#ifdef ENABLE_VIDEO
	return createAudioCall(Video::getInstance().getEnable());
#else
	return createAudioCall(false);
#endif
}

AudioCall * AudioCallManager::createAudioCall(bool enableVideo) {
	AudioCall * audioCall = new AudioCall();
	audioCall->enableVideo(enableVideo);
	audioCall->addObserver(*this);
	_audioCallList.push_back(audioCall);

	return audioCall;
}

AudioCall * AudioCallManager::createAudioCall(const SipAddress & sipAddress, Contact * contact) {
#ifdef ENABLE_VIDEO
	return createAudioCall(sipAddress, contact, Video::getInstance().getEnable());
#else
	return createAudioCall(sipAddress, contact, false);
#endif
}

AudioCall * AudioCallManager::createAudioCall(const SipAddress & sipAddress, Contact * contact, bool enableVideo) {
	AudioCall * audioCall = getAudioCall(sipAddress.getValidPhoneNumber());
	if (!audioCall) {
		audioCall = createAudioCall(enableVideo);
	}
	audioCall->enableVideo(enableVideo);
	audioCall->getSessionWindow().showOutgoingCallPhonePage(sipAddress, contact);
	return audioCall;
}

AudioCall * AudioCallManager::getAudioCall(const QString & phoneNumber) {
	for (unsigned int i = 0; i < _audioCallList.size(); i++) {
		Contact * contact = _audioCallList[i]->getSessionWindow().getContact();
		if (contact) {
			if (*contact == phoneNumber) {
				return _audioCallList[i];
			}
		}
	}
	return NULL;
}

void AudioCallManager::sendSms(const QString & phoneNumber, const QString & message, Contact * contact) {
	AudioCall * audioCall = getAudioCall(phoneNumber);
	if (!audioCall) {
		audioCall = createAudioCall();
	}

	audioCall->getSessionWindow().sendSms(phoneNumber, message, contact);
}

void AudioCallManager::startChat(Contact & contact, const QString & messageReceived) {
	AudioCall * audioCall = getAudioCall(contact.getWengoPhone());
	if (!audioCall) {
		audioCall = createAudioCall();
	}

	audioCall->getSessionWindow().startChat(contact, messageReceived);
}

void AudioCallManager::playDTMF(char dtmf) const {
	if (_activeAudioCall != AudioCall::NOT_CALLING) {
		SipPrimitives::playDTMF(_activeAudioCall, dtmf);
	}
}

void AudioCallManager::playDTMFSoundFile(const QString & soundfile) const {
	if (_activeAudioCall != AudioCall::NOT_CALLING) {
		phSendSoundFile(_activeAudioCall, soundfile);
	}
}

bool AudioCallManager::isConversationOccuring() const {
	for (unsigned int i = 0; i < _audioCallList.size(); i++) {
		if (_audioCallList[i]->isCalling()) {
			return true;
		}
	}
	return false;
}

Contact & getContact(const SipAddress & sipAddress) {
	ContactList & contactList = ContactList::getInstance();
	Contact * contact = contactList.getContact(sipAddress.getValidPhoneNumber());

	if (!contact) {
		contact = contactList.getContact(sipAddress.getDisplayName());
		if (!contact) {
			contact = new Contact();
			contact->setFirstName(sipAddress.getDisplayName());
			contact->setWengoPhone(sipAddress.getValidPhoneNumber());
		}
	}

	return *contact;
}

void AudioCallManager::callProgress(int callId, const CallStateInfo * info) {
	switch (info->event) {
	case phINCALL: {
		SipAddress from(info->u.remoteUri);
		Contact & contact = getContact(from);

		//FIXME no double call since it is buggy
		if (!isConversationOccuring() && !contact.isBlocked()) {
			cerr << "AudioCallManager::IncomingCall: " << from.getValidPhoneNumber() << endl;
			AudioCall * audioCall = getAudioCall(from.getValidPhoneNumber());
			if (!audioCall) {
				//Wengo name == display name
				audioCall = getAudioCall(from.getDisplayName());
				if (audioCall == NULL) {
					audioCall = createAudioCall();
				}
			}
			audioCall->callProgress(callId, info);
		} else {
			SipPrimitives::rejectCall(callId, 486);
			return;
		}
		break;
	}

	case phDIALING:

	case phRINGING:

	case phNOANSWER:

	case phCALLBUSY:

	case phCALLREDIRECTED:

	case phCALLOK:

	case phCALLHELD:

	case phCALLRESUMED:

	case phHOLDOK:

	case phRESUMEOK:

	case phCALLCLOSED:

	case phCALLERROR: {
		for (unsigned int i = 0; i < _audioCallList.size(); i++) {
			if (_audioCallList[i]->getCallId() == callId) {
				_audioCallList[i]->callProgress(callId, info);
			}
		}
		break;
	}

	default:
		assert(NULL && "This phapi event is not handled by this class");
	}
}

void AudioCallManager::messageProgress(int /*messageId*/, const phMsgStateInfo_t * info) {
	switch (info->event) {
	case phMsgNew: {
		Contact & contact = getContact(SipAddress(info->from));
		if (!contact.isBlocked()) {
			startChat(contact, info->content);
		}
		break;
	}

	case phMsgOk:
		break;

	case phMsgError:
		startChat(getContact(SipAddress(info->to)), QString(tr("<font color=red>Error: message cannot be sent</font>")).utf8());
		break;

	default:
		assert(NULL && "This phapi event is not handled by this class");
	}
}

void AudioCallManager::onFrameReady(int cid, phVideoFrameReceivedEvent_t *ev) {
	AudioCall * ca = getCallFromCallId(cid);
	if (ca != NULL) { // FIXME: ??? && ca->isCalling()) {
		/* Deal with the frame. Deal nicely i mean */
#ifdef ENABLE_VIDEO
		ca->getSessionWindow().getVisioPageWidget().frameReceived(ca, ev);
#endif
	}
}



AudioCall * AudioCallManager::getActiveAudioCall() {
	for (unsigned int i = 0; i < _audioCallList.size(); i++) {
		if (_audioCallList[i]->getCallId() == _activeAudioCall) {
			return _audioCallList[i];
		}
	}
	return NULL;
}

void AudioCallManager::reset() {
	for (unsigned int i = 0; i < _audioCallList.size(); i++) {
		//Contact * contact = _audioCallList[i]->getSessionWindow().getContact();
		//Cannot delete the contact since this is already done inside ContactList
		//delete contact;
		//contact = NULL;
		_audioCallList[i]->getSessionWindow().reset();
	}
}

AudioCall *AudioCallManager::getCallFromCallId(int callId) {
	for (unsigned int i = 0; i < _audioCallList.size(); i++) {
		if (_audioCallList[i]->getCallId() == callId) {
			return _audioCallList[i];
		}
	}
	return NULL;
}
