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

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/phonecall/PhoneCall.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/profile/UserProfile.h>

#include <sound/AudioDevice.h>

#include <util/Logger.h>

using namespace std;

WenboxPlugin::WenboxPlugin(UserProfile & userProfile)
	: _userProfile(userProfile) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.valueChangedEvent +=
		boost::bind(&WenboxPlugin::wenboxConfigChangedEventHandler, this, _1, _2);

	_wenbox = new Wenbox();
	openWenbox();
}

WenboxPlugin::~WenboxPlugin() {
	delete _wenbox;
}

void WenboxPlugin::openWenbox() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	if (_wenbox->open()) {
		if (config.getWenboxEnable()) {
			//Disable Half-duplex mode
			config.set(Config::AUDIO_HALFDUPLEX_KEY, false);
			//Enable AEC
			config.set(Config::AUDIO_AEC_KEY, true);

			switchCurrentAudioDeviceToWenbox();
			_wenbox->setDefaultMode(Wenbox::ModeUSB);
			_wenbox->switchMode(Wenbox::ModeUSB);
			_wenbox->keyPressedEvent += boost::bind(&WenboxPlugin::keyPressedEventHandler, this, _1, _2);
		} else {
			_wenbox->close();
		}
	} else {
		config.set(Config::WENBOX_ENABLE_KEY, false);
	}
}

void WenboxPlugin::closeWenbox() {
	if (_wenbox->isOpen()) {
		switchCurrentAudioDeviceToSoundCard();
		_wenbox->close();
	}
}

void WenboxPlugin::wenboxConfigChangedEventHandler(Settings & sender, const std::string & key) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	if (key == Config::WENBOX_ENABLE_KEY) {
		if (config.getWenboxEnable()) {
			openWenbox();
		} else {
			closeWenbox();
		}
	}
}

void WenboxPlugin::keyPressedEventHandler(IWenbox & sender, IWenbox::Key key) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	PhoneCall * phoneCall = getActivePhoneCall();

	switch (key) {
	case IWenbox::KeyPickUp:
		if (phoneCall) {
			phoneCall->accept(config.getVideoEnable());
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
		LOG_FATAL("unknown key pressed=" + String::fromNumber(key));
	}
}

PhoneCall * WenboxPlugin::getActivePhoneCall() const {
	IPhoneLine * phoneLine = _userProfile.getActivePhoneLine();
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
	LOG_DEBUG("Wenbox audio device not found");
	//Empty string
	return String::null;
}

void WenboxPlugin::switchCurrentAudioDeviceToWenbox() {
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

	//Windows default playback is the Wenbox
	if (outputDeviceName == defaultPlaybackDevice ||
		intputDeviceName == defaultPlaybackDevice ||
		ringerDeviceName == defaultPlaybackDevice) {

		return;
	}

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	//Changes audio settings
	config.set(Config::AUDIO_OUTPUT_DEVICENAME_KEY, outputDeviceName);
	config.set(Config::AUDIO_INPUT_DEVICENAME_KEY, intputDeviceName);
	config.set(Config::AUDIO_RINGER_DEVICENAME_KEY, ringerDeviceName);
}

void WenboxPlugin::switchCurrentAudioDeviceToSoundCard() {
	//Back to the previous audio settings
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	std::string tmp = boost::any_cast<std::string>(config.getDefaultValue(Config::AUDIO_OUTPUT_DEVICENAME_KEY));
	config.set(Config::AUDIO_OUTPUT_DEVICENAME_KEY, tmp);

	tmp = boost::any_cast<std::string>(config.getDefaultValue(Config::AUDIO_INPUT_DEVICENAME_KEY));
	config.set(Config::AUDIO_INPUT_DEVICENAME_KEY, tmp);

	tmp = boost::any_cast<std::string>(config.getDefaultValue(Config::AUDIO_RINGER_DEVICENAME_KEY));
	config.set(Config::AUDIO_RINGER_DEVICENAME_KEY, tmp);
}
