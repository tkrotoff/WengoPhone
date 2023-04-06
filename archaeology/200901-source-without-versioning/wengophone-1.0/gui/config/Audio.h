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

#ifndef AUDIO_H
#define AUDIO_H

#include <singleton/Singleton.h>
#include <observer/Subject.h>

#include <qstring.h>

/**
 * Records audio settings.
 *
 * @author Philippe Kajmar
 */
class Audio : public Singleton<Audio>, public Subject {
	friend class Singleton<Audio>;
public:

	/// audio tag
	static const QString AUDIO_TAG;

	/// input device tag
	static const QString INPUT_TAG;

	/// output device tag
	static const QString OUTPUT_TAG;

	/// ringing device tag
	static const QString RINGING_TAG;

	/// name tag
	static const QString NAME_TAG;

	void setInputDevice(const QString & inputDeviceName);
	const QString & getInputDeviceName() const;
	int getInputDeviceId() const;

	void setOutputDevice(const QString & outputDeviceName);
	const QString & getOutputDeviceName() const;
	int getOutputDeviceId() const;

	void setRingingDevice(const QString & ringingDeviceName);
	const QString & getRingingDeviceName() const;

	void updateDevicesId();

	/// change all attributes
	void changeSettings(const QString & inputDeviceName,
			const QString & outputDeviceName,
			const QString & ringingDeviceName);

	void storeSettings() {
		_inputDeviceNameStored = getInputDeviceName();
		_outputDeviceNameStored = getOutputDeviceName();
		_ringingDeviceNameStored = getRingingDeviceName();
	}

	const QString & getInputDeviceNameStored() const {
		return _inputDeviceNameStored;
	}

	const QString & getOutputDeviceNameStored() const {
		return _outputDeviceNameStored;
	}

	const QString & getRingingDeviceNameStored() const {
		return _ringingDeviceNameStored;
	}

	void loadStoredSettings() {
		changeSettings(_inputDeviceNameStored,
				_outputDeviceNameStored,
				_ringingDeviceNameStored);
	}

	/// set default Settings
	void defaultSettings();

private:

	Audio();

	Audio(const Audio &);
	Audio & operator=(Audio &);

	/// input device name
	QString _inputDeviceName;

	/// output device name
	QString _outputDeviceName;

	/// ringing device name
	QString _ringingDeviceName;

	QString _inputDeviceNameStored;
	QString _outputDeviceNameStored;
	QString _ringingDeviceNameStored;
};

#endif	//AUDIO_H
