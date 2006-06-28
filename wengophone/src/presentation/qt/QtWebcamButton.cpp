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

#include "QtWebcamButton.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/StringList.h>
#include <util/Logger.h>

#include <QtGui>

QtWebcamButton::QtWebcamButton(QPushButton * webcamButton)
	: QObjectThreadSafe(webcamButton) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_webcamButton = webcamButton;
	_enableVideo = config.getVideoEnable();

	config.valueChangedEvent += boost::bind(&QtWebcamButton::configChangedEventHandler, this, _1, _2);

	connect(_webcamButton, SIGNAL(clicked()), SLOT(enableVideo()));

	configChangedEventHandler(config, Config::VIDEO_ENABLE_KEY);
}

QtWebcamButton::~QtWebcamButton() {
	//TODO: unregister events, delete created objects
}

void QtWebcamButton::enableVideo() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::VIDEO_ENABLE_KEY, !_enableVideo);
}

void QtWebcamButton::changeWebcamButtonState() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_enableVideo = config.getVideoEnable();
	if (_enableVideo) {
		_webcamButton->setIcon(QIcon(":pics/iconbar/webcam.png"));
	} else {
		_webcamButton->setIcon(QIcon(":pics/iconbar/webcam-off.png"));
	}

	_webcamButton->setEnabled(!config.getVideoWebcamDevice().empty());
}

void QtWebcamButton::configChangedEventHandler(Settings & sender, const std::string & key) {
	typedef PostEvent2<void (Settings & sender, const std::string &), Settings &, std::string> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWebcamButton::configChangedEventHandlerThreadSafe, this, _1, _2), sender, key);
	postEvent(event);
}

void QtWebcamButton::configChangedEventHandlerThreadSafe(Settings & sender, const std::string & key) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if (key == Config::VIDEO_ENABLE_KEY || key == Config::VIDEO_WEBCAM_DEVICE_KEY) {
		changeWebcamButtonState();
	}
}
