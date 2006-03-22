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

#include "WenboxPlugin.h"

#include <model/WengoPhone.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/phonecall/PhoneCall.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <sound/AudioDevice.h>

#include <util/Logger.h>

using namespace std;

WenboxPlugin::WenboxPlugin(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone) {

	_wenbox = new Wenbox();
	if (_wenbox->open()) {
		switchCurrentAudioDeviceToWenbox();
		_wenbox->setDefaultMode(Wenbox::ModeUSB);
		_wenbox->switchMode(Wenbox::ModeUSB);
		_wenbox->keyPressedEvent += boost::bind(&WenboxPlugin::keyPressedEventHandler, this, _1, _2);
	}
}

WenboxPlugin::~WenboxPlugin() {
	_wenbox->close();
	delete _wenbox;
}

void WenboxPlugin::keyPressedEventHandler(IWenbox & sender, IWenbox::Key key) {
	PhoneCall * phoneCall = getActivePhoneCall();

	switch (key) {
	case IWenbox::KeyPickUp:
		if (phoneCall) {
			phoneCall->accept();
		}
		break;

	case IWenbox::KeyHangUp:
		_phoneNumberBuffer = "";
		phoneNumberBufferUpdatedEvent(*this, _phoneNumberBuffer);
		if (phoneCall) {
			phoneCall->close();
		}
		break;

	case Wenbox::Key0:
		_phoneNumberBuffer += "0";
		phoneNumberBufferUpdatedEvent(*this, _phoneNumberBuffer);
		break;

	case Wenbox::Key1:
		_phoneNumberBuffer += "1";
		phoneNumberBufferUpdatedEvent(*this, _phoneNumberBuffer);
		break;

	case Wenbox::Key2:
		_phoneNumberBuffer += "2";
		phoneNumberBufferUpdatedEvent(*this, _phoneNumberBuffer);
		break;

	case Wenbox::Key3:
		_phoneNumberBuffer += "3";
		phoneNumberBufferUpdatedEvent(*this, _phoneNumberBuffer);
		break;

	case Wenbox::Key4:
		_phoneNumberBuffer += "4";
		phoneNumberBufferUpdatedEvent(*this, _phoneNumberBuffer);
		break;

	case Wenbox::Key5:
		_phoneNumberBuffer += "5";
		phoneNumberBufferUpdatedEvent(*this, _phoneNumberBuffer);
		break;

	case Wenbox::Key6:
		_phoneNumberBuffer += "6";
		phoneNumberBufferUpdatedEvent(*this, _phoneNumberBuffer);
		break;

	case Wenbox::Key7:
		_phoneNumberBuffer += "7";
		phoneNumberBufferUpdatedEvent(*this, _phoneNumberBuffer);
		break;

	case Wenbox::Key8:
		_phoneNumberBuffer += "8";
		phoneNumberBufferUpdatedEvent(*this, _phoneNumberBuffer);
		break;

	case Wenbox::Key9:
		_phoneNumberBuffer += "9";
		phoneNumberBufferUpdatedEvent(*this, _phoneNumberBuffer);
		break;

	case Wenbox::KeyStar:
		_phoneNumberBuffer += "*";
		phoneNumberBufferUpdatedEvent(*this, _phoneNumberBuffer);
		break;

	default:
		LOG_FATAL("unknown key pressed");
	}
}

PhoneCall * WenboxPlugin::getActivePhoneCall() const {
	IPhoneLine * phoneLine = _wengoPhone.getActivePhoneLine();
	PhoneCall * phoneCall = NULL;
	if (phoneLine) {
		phoneCall = phoneLine->getActivePhoneCall();
	}
	return phoneCall;
}

void WenboxPlugin::setState(Wenbox::PhoneCallState state, const std::string & phoneNumber) {
	_wenbox->setState(state, phoneNumber);
}

std::string WenboxPlugin::getWenboxAudioDeviceName() const {
	/*
	 * First looks in the Wenbox audio device list, then
	 * it looks into the audio device list and try to find a matching
	 * string pattern.
	 */

	StringList wenboxAudioDeviceList = _wenbox->getAudioDeviceNameList();
	for (unsigned i = 0; i < wenboxAudioDeviceList.size(); i++) {
		string wenboxAudioDeviceName = wenboxAudioDeviceList[i];

		StringList audioDeviceList = AudioDevice::getOutputMixerDeviceList();
		for (unsigned j = 0; j < audioDeviceList.size(); j++) {
			string audioDeviceName = audioDeviceList[j];

			if (audioDeviceName.find(wenboxAudioDeviceName) != string::npos) {
				//We found the real name of the Wenbox audio device
				LOG_DEBUG("wenbox audio device name=" + audioDeviceName);
				return audioDeviceName;
			}
		}
	}

	//We didn't find the real name of the Wenbox audio device
	LOG_DEBUG("wenbox audio device not found");
	//Empty string
	return String::null;
}

bool WenboxPlugin::switchCurrentAudioDeviceToWenbox() const {
	string defaultPlaybackDevice = AudioDevice::getDefaultPlaybackDevice();
	string intputDeviceName = defaultPlaybackDevice;
	string outputDeviceName = defaultPlaybackDevice;
	string ringerDeviceName = defaultPlaybackDevice;

	//Looks for the Wenbox audio device from the list of devices from Windows
	string wenboxAudioDeviceName(getWenboxAudioDeviceName());
	if (!wenboxAudioDeviceName.empty()) {
		outputDeviceName = wenboxAudioDeviceName;
		intputDeviceName = wenboxAudioDeviceName;
		ringerDeviceName = wenboxAudioDeviceName;
	}

	if (outputDeviceName == defaultPlaybackDevice ||
		intputDeviceName == defaultPlaybackDevice ||
		ringerDeviceName == defaultPlaybackDevice) {

		return false;
	}

	//Changes audio settings
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::AUDIO_OUTPUT_DEVICENAME_KEY, outputDeviceName);
	config.set(Config::AUDIO_INPUT_DEVICENAME_KEY, intputDeviceName);
	config.set(Config::AUDIO_RINGER_DEVICENAME_KEY, ringerDeviceName);

	return true;
}
