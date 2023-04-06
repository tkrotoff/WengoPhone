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

#include "Audio.h"

#include "connect/Connect.h"

#include <AudioDevice.h>
#include <observer/Event.h>

#include <phapi.h>

#include <qstring.h>

const QString Audio::AUDIO_TAG = "audio";
const QString Audio::INPUT_TAG = "input";
const QString Audio::OUTPUT_TAG = "output";
const QString Audio::RINGING_TAG = "ringing";
const QString Audio::NAME_TAG = "name";

Audio::Audio() {
	StringList l = AudioDevice::getInputMixerDeviceList();
	if( l.size() >= 1 ) {
		setInputDevice(l[0]);
	}
	l = AudioDevice::getOutputMixerDeviceList();
	if( l.size() >= 1 ) {
		setOutputDevice(l[0]);
		setRingingDevice(l[0]);
	}
}

/**
* @param inputDeviceName
* @brief set input device name
*/
void Audio::setInputDevice(const QString & inputDeviceName) {
	if (AudioDevice::getInputMixerDeviceList().contains(inputDeviceName) > 0) {
		_inputDeviceName = inputDeviceName;
		notify(Event());
	}
}

/**
* @return input device name
*/
const QString & Audio::getInputDeviceName() const {
	return _inputDeviceName;
}

int Audio::getInputDeviceId() const {
	return AudioDevice::getWaveInDeviceId(_inputDeviceName);
}

/**
* @param outputDeviceName
* @brief set output device name
*/
void Audio::setOutputDevice(const QString & outputDeviceName) {
	if (AudioDevice::getOutputMixerDeviceList().contains(outputDeviceName) > 0) {
		_outputDeviceName = outputDeviceName;
		notify(Event());
	}
}

/**
* @return output device identifier
*/
const QString & Audio::getOutputDeviceName() const {
	return _outputDeviceName;
}

int Audio::getOutputDeviceId() const {
	return AudioDevice::getWaveOutDeviceId(_outputDeviceName);
}

/**
* @param inputDeviceName
* @brief set input device name
*/
void Audio::setRingingDevice(const QString & ringingDeviceName) {
	if (AudioDevice::getOutputMixerDeviceList().contains(ringingDeviceName) > 0) {
		_ringingDeviceName = ringingDeviceName;
		notify(Event());
	}
}

/**
* @return ringing device name
*/
const QString & Audio::getRingingDeviceName() const {
	return _ringingDeviceName;
}

void Audio::updateDevicesId() {
	static const char * INPUT_DEVICE_TAG = "IN=";
	static const char * OUTPUT_DEVICE_TAG  = "OUT=";

	phChangeAudioDevices(QString("pa:") + INPUT_DEVICE_TAG
			+ QString("").setNum(getInputDeviceId())
			+ " "
			+ OUTPUT_DEVICE_TAG
			+ QString("").setNum(getOutputDeviceId()));
}

/**
 * @param inputDeviceName
 * @param outputDeviceName
 * @param ringingDeviceName
 * @brief change all attributes
 */
void Audio::changeSettings(const QString & inputDeviceName,
				const QString & outputDeviceName,
				const QString & ringingDeviceName) {
	_inputDeviceName = inputDeviceName;
	_outputDeviceName = outputDeviceName;
	_ringingDeviceName = ringingDeviceName;
	notify(Event());

	updateDevicesId();
}

/**
 * @brief set default settings
 */
void Audio::defaultSettings() {
	changeSettings(AudioDevice::getDefaultPlaybackDevice(),
		AudioDevice::getDefaultPlaybackDevice(),
		AudioDevice::getDefaultPlaybackDevice());
}
