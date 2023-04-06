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

#ifndef VIDEO_H
#define VIDEO_H

#include <singleton/Singleton.h>
#include <observer/Subject.h>
#include <observer/Event.h>

#include <qstring.h>

/**
 * Records video settings.
 *
 * Copied over from Video.cpp (written by Philippe Kajmar)
 *
 * @author David Ferlier
 * @author Mathieu Stute
 */
class Video : public Singleton<Video>, public Subject {
	friend class Singleton<Video>;
public:

	/// audio tag
	static const QString VIDEO_TAG;

	/// name tag
	static const QString NAME_TAG;

	static const QString FLIP_TAG;

	static const QString QUALITY_TAG;

	static const QString FRAMERATE_TAG;

	static const QString RESOLUTION_TAG;

	static const QString INPUTDEVICE_TAG;

	static const QString VIDEOENABLE_TAG;

	static const QString CONNECTIONTYPE_TAG;

	void setInputDevice(const QString & inputDeviceName);
	const QString & getInputDeviceName() const;
	int getInputDeviceId() const;

	/// change all attributes
	void changeSettings(const QString & inputDeviceName);

	void storeSettings() {
		_inputDeviceNameStored = getInputDeviceName();
	}

	void loadStoredSettings() {
		changeSettings(_inputDeviceNameStored);
	}

	//Video config getters & setters

	void setQuality(const QString & quality) {
		_quality = quality;
		notify(Event());
	}

	QString getQuality() const {
		if(_quality)
			return _quality;
		else
			return "50";
	}

	void setFrameRate(const QString & frameRate) {
		_frameRate = frameRate;
		notify(Event());
	}

	QString getFrameRate() const {
		if( _frameRate )
			return _frameRate;
		else
			return "15";
	}

	void setCaptureResolution(const QString & resolution) {
		_resolution = resolution;
		notify(Event());
	}

	QString getCaptureResolution() const {
		return _resolution;
	}

	QString getConnectionType() const {
		return _connectionType;
	}

	void setConnectionType(const QString & t) {
		_connectionType = t;
	}

	void setFlip(bool f) {
		_flip= f;
		notify(Event());
	}

	bool getFlip() const {
		return _flip;
	}

	void setEnable(bool e) {
		_enable = e;
	}

	bool getEnable() const {
		return _enable;
	}

	/// set default Settings
	void defaultSettings();

	// test if settings are valid
	bool isValid() const {
		return true;
	}

private:

	Video();

	Video(const Video &);
	Video & operator=(Video &);

	/// input device name
	QString _inputDeviceName;

	QString _inputDeviceNameStored;

	//Webcam resolution
	QString _resolution;

	//Frame rate
	QString _frameRate;

	//Video quality
	QString _quality;

	//Connection type
	QString _connectionType;

	//Flip video
	bool _flip;

	//Enable
	bool _enable;
};

#endif	//VIDEO_H
