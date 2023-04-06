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

#include "WenboxAudioDevice.h"

#include "config/Audio.h"

#include <AudioDevice.h>
#include <StringList.h>

#include <qstringlist.h>
#include <qstring.h>

#include <iostream>
using namespace std;

QStringList WenboxAudioDevice::getWenboxAudioDeviceList() {
	/*
	 * The list should be ordered from the most specific name to the
	 * most generic (e.g USB).
	 */

	QStringList deviceList;

	//winXP us/fr
	//win2k us
	deviceList += "USB Audio Device";

	//win2k fr
	deviceList += "Périphérique audio USB";

	//generic
	deviceList += "USB";

	return deviceList;
}

QString WenboxAudioDevice::getWenboxAudioDeviceName(const StringList & audioDeviceList) {
	/*
	 * First looks in the Wenbox audio device list, then
	 * it looks into the audio device list and try to find a matching
	 * string pattern.
	 */

	QStringList wenboxAudioDeviceList = getWenboxAudioDeviceList();
	for (QStringList::Iterator it = wenboxAudioDeviceList.begin(); it != wenboxAudioDeviceList.end(); ++it) {
		QString wenboxAudioDeviceName = *it;

		cerr << "WenboxAudioDevice wenboxAudioDeviceName: " << wenboxAudioDeviceName << endl;

		for (StringList::const_iterator it2 = audioDeviceList.begin(); it2 != audioDeviceList.end(); ++it2) {
			QString audioDeviceName = *it2;

			cerr << "WenboxAudioDevice audioDeviceName: " << audioDeviceName << endl;

			if (audioDeviceName.find(wenboxAudioDeviceName) != -1) {
				//We found the real name of the Wenbox audio device
				return audioDeviceName;
			}
		}
	}

	//We didn't find the real name of the Wenbox audio device
	//Empty string
	return QString::null;
}

QString WenboxAudioDevice::getWenboxAudioDeviceName() {
	return getWenboxAudioDeviceName(AudioDevice::getOutputMixerDeviceList());
}

bool WenboxAudioDevice::rollbackDefaultAudioDevice() {
	bool ret = false;

	StringList listDevices = AudioDevice::getOutputMixerDeviceList();

	//Take the first audio device that is not the Wenbox
	for (StringList::const_iterator it = listDevices.begin(); it != listDevices.end(); ++it) {
		if (*it != getWenboxAudioDeviceName()) {
			ret = AudioDevice::setDefaultPlaybackDevice(*it);
			ret = AudioDevice::setDefaultRecordDevice(*it);
			break;
		}
	}

	return ret;
}

bool WenboxAudioDevice::isWenboxAudioDeviceUsed() {
	Audio & audio = Audio::getInstance();

	//Check if the Wenbox is the current playback audio device
	if (audio.getRingingDeviceName() == getWenboxAudioDeviceName()) {
		return true;
	}
	return false;
}

bool WenboxAudioDevice::switchCurrentAudioDeviceToWenbox() {
	QString defaultPlaybackDevice(AudioDevice::getDefaultPlaybackDevice());
	QString intputDeviceName = defaultPlaybackDevice;
	QString outputDeviceName = defaultPlaybackDevice;
	QString ringingDeviceName = defaultPlaybackDevice;

	//Look for the Wenbox audio device from the list of devices from Windows
	QString wenboxAudioDeviceName(getWenboxAudioDeviceName());
	if (!wenboxAudioDeviceName.isEmpty()) {
		outputDeviceName = wenboxAudioDeviceName;
		intputDeviceName = wenboxAudioDeviceName;
		ringingDeviceName = wenboxAudioDeviceName;
	}

	//Changes audio settings
	Audio & audio = Audio::getInstance();
	audio.changeSettings(intputDeviceName,
			outputDeviceName,
			ringingDeviceName);

	if (outputDeviceName == AudioDevice::getDefaultPlaybackDevice() ||
		intputDeviceName == AudioDevice::getDefaultPlaybackDevice() ||
		ringingDeviceName == AudioDevice::getDefaultPlaybackDevice()) {

		return false;
	}

	return true;
}
