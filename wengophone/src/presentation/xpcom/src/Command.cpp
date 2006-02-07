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

#include <Command.h>
#include <Listener.h>

#include "XPCOMWengoPhone.h"
#include "XPCOMPhoneLine.h"
#include "XPCOMPhoneCall.h"
#include "XPCOMFactory.h"
#include "XPCOMIMHandler.h"
#include "ListenerList.h"

#include <control/CWengoPhone.h>
#include <model/WengoPhone.h>
#include <model/contactlist/Contact.h>

#include <IMWrapperFactory.h>
#include <SoundMixer.h>
#include <AudioDevice.h>

#if defined(CC_MSVC)
	#include <win32/MemoryDump.h>
#endif

#ifdef PHAPIWRAPPER
	#include <PhApiFactory.h>
#elif defined(SIPXWRAPPER)
	#include <SipXFactory.h>
	#include <NullIMFactory.h>
#else
	#include <NullSipFactory.h>
	#include <NullIMFactory.h>
#endif

Command::Command(const std::string & configFilesPath) {

#ifdef CC_MSVC
	_memoryDump = new MemoryDump("WengoPhoneNG");
#endif

	//Graphical interface implementation
	PFactory::setFactory(new XPCOMFactory());

	//SIP implementation
	SipWrapperFactory * sipFactory = NULL;

	//IM implementation
	IMWrapperFactory * imFactory = NULL;

#ifdef SIPXWRAPPER
	sipFactory = new SipXFactory();
	imFactory = new NullIMFactory();
#elif defined(PHAPIWRAPPER)
	PhApiFactory * phApiFactory = new PhApiFactory();
	sipFactory = phApiFactory;
	imFactory = phApiFactory;
#else
	sipFactory = new NullSipFactory();
	imFactory = new NullIMFactory();
#endif
	SipWrapperFactory::setFactory(sipFactory);
	IMWrapperFactory::setFactory(imFactory);

	_wengoPhone = new WengoPhone();
	_cWengoPhone = new CWengoPhone(*_wengoPhone);

	try {
		_soundMixer = new SoundMixer(AudioDevice::getDefaultRecordDevice(),
					AudioDevice::getDefaultPlaybackDevice());
	} catch(Exception &) {
		_soundMixer = NULL;
	}

	WengoPhone::CONFIG_FILES_PATH = configFilesPath;
}

Command::~Command() {
	delete _wengoPhone;
	delete _cWengoPhone;
	delete _soundMixer;
#ifdef CC_MSVC
	delete _memoryDump;
#endif
}

void Command::start() {
	//Starts the model component thread
	_cWengoPhone->start();
}

void Command::setHttpProxySettings(const std::string & hostname, unsigned port,
				const std::string & login, const std::string & password) {

	WengoPhone::HTTP_PROXY_HOSTNAME = hostname;
	WengoPhone::HTTP_PROXY_PORT = port;
}

void Command::terminate() {
	_cWengoPhone->terminate();
}

void Command::addWengoAccount(const std::string & login, const std::string & password, bool autoLogin) {
	_cWengoPhone->addWengoAccount(login, password, autoLogin);
}

void Command::addListener(Listener * listener) {
	ListenerList & listenerList = ListenerList::getInstance();
	listenerList.add(listener);
}

bool Command::removeListener(Listener * listener) {
	ListenerList & listenerList = ListenerList::getInstance();
	return listenerList.remove(listener);
}

void Command::removeAllListeners() {
	ListenerList & listenerList = ListenerList::getInstance();
	listenerList.clear();
}

int Command::makeCall(const std::string & phoneNumber, int lineId) {
	PhoneLineList & phoneLineList = PhoneLineList::getInstance();
	for (unsigned i = 0; i < phoneLineList.size(); i++) {
		if (i == (lineId - 1)) {
			XPCOMPhoneLine * phoneLine = phoneLineList[i];
			return phoneLine->makeCall(phoneNumber);
		}
	}

	return CallIdError;
}

void Command::hangUp(int callId) {
	PhoneCallMap & phoneCallMap = PhoneCallMap::getInstance();
	XPCOMPhoneCall * phoneCall = phoneCallMap[callId];
	if (phoneCall) {
		phoneCall->hangUp();
	}
}

bool Command::canHangUp(int callId) const {
	PhoneCallMap & phoneCallMap = PhoneCallMap::getInstance();
	XPCOMPhoneCall * phoneCall = phoneCallMap[callId];
	if (phoneCall) {
		return phoneCall->canHangUp();
	}

	return false;
}

void Command::pickUp(int callId) {
	PhoneCallMap & phoneCallMap = PhoneCallMap::getInstance();
	XPCOMPhoneCall * phoneCall = phoneCallMap[callId];
	if (phoneCall) {
		phoneCall->pickUp();
	}
}

bool Command::canPickUp(int callId) const {
	PhoneCallMap & phoneCallMap = PhoneCallMap::getInstance();
	XPCOMPhoneCall * phoneCall = phoneCallMap[callId];
	if (phoneCall) {
		return phoneCall->canPickUp();
	}

	return false;
}

void Command::subscribeToPresenceOf(const std::string & contactId) {
	XPCOMIMHandler & imHandler = XPCOMIMHandler::getInstance();
	imHandler.subscribeToPresenceOf(contactId);
}

void Command::publishMyPresence(Listener::PresenceState state, const std::string & note) {
	XPCOMIMHandler & imHandler = XPCOMIMHandler::getInstance();
	imHandler.publishMyPresence(state, note);
}

int Command::sendChatMessage(const std::string & sipAddress, const std::string & message) {
	XPCOMIMHandler & imHandler = XPCOMIMHandler::getInstance();
	return imHandler.sendChatMessage(sipAddress, message);
}

void Command::setInputVolume(int volume) {
	if (_soundMixer) {
		_soundMixer->setInputVolume(volume);
	}
}

void Command::setOutputVolume(int volume) {
	if (_soundMixer) {
		_soundMixer->setOutputVolume(volume);
	}
}

int Command::getInputVolume() {
	if (_soundMixer) {
		return _soundMixer->getInputVolume();
	}
	return -1;
}

int Command::getOutputVolume() {
	if (_soundMixer) {
		return _soundMixer->getOutputVolume();
	}
	return -1;
}
