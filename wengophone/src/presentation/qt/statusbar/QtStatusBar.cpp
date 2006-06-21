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

#include "QtStatusBar.h"
#include "QtStatusBarStyle.h"

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/network/NetworkObserver.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/phoneline/PhoneLineState.h>
#include <model/profile/UserProfile.h>

#include <sound/VolumeControl.h>
#include <sound/AudioDevice.h>

#include <util/Logger.h>

#include <QtGui>

static const char * MNG_FORMAT = "MNG";

QtStatusBar::QtStatusBar(CWengoPhone & cWengoPhone, QStatusBar * statusBar)
	: QObjectThreadSafe(statusBar),
	_cWengoPhone(cWengoPhone) {

	_statusBar = statusBar;
	QtStatusBarStyle * qtStatusBarstyle = new QtStatusBarStyle();
	_statusBar->setStyle(qtStatusBarstyle);

	QWidget * statusGroup = new QWidget(_statusBar);
	new QHBoxLayout(statusGroup);
	statusGroup->layout()->setMargin(0);
	statusGroup->layout()->setSpacing(3);

	//_cWengoPhone.networkDiscoveryStateChangedEvent +=
	//	boost::bind(&QtStatusBar::networkDiscoveryStateChangedEventHandler, this, _1, _2);

	NetworkObserver::getInstance().connectionIsDownEvent +=
		boost::bind(&QtStatusBar::connectionIsDownEventHandler, this);

	NetworkObserver::getInstance().connectionIsUpEvent +=
		boost::bind(&QtStatusBar::connectionIsUpEventHandler, this);

	//internetConnectionStateLabel
	_internetConnectionMovie = new QMovie(":/pics/statusbar/status-earth-connecting.mng", MNG_FORMAT, _statusBar);
	_internetConnectionStateLabel = new QLabel(statusGroup);
	_internetConnectionStateLabel->setMovie(_internetConnectionMovie);
	_internetConnectionStateLabel->setToolTip(tr("Not Connected"));
	statusGroup->layout()->addWidget(_internetConnectionStateLabel);
	_internetConnectionMovie->start();
	if (NetworkObserver::getInstance().isConnected()) {
		connectionIsUpEventHandler();
	}


	//phoneLineStateLabel
	_sipConnectionMovie = new QMovie(":/pics/statusbar/status-network-connecting.mng", MNG_FORMAT, _statusBar);
	_phoneLineStateLabel = new QLabel(statusGroup);
	_phoneLineStateLabel->setMovie(_sipConnectionMovie);
	_phoneLineStateLabel->setToolTip(tr("Not Connected"));
	statusGroup->layout()->addWidget(_phoneLineStateLabel);
	_sipConnectionMovie->start();

	//soundStateLabel
	_soundStateLabel = new QLabel(statusGroup);
	_soundStateLabel->setPixmap(QPixmap(":/pics/statusbar/status-audio-offline.png"));
	_soundStateLabel->setToolTip(tr("Audio Configuration Error"));
	statusGroup->layout()->addWidget(_soundStateLabel);

	_statusBar->addPermanentWidget(statusGroup);

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.valueChangedEvent += boost::bind(&QtStatusBar::checkSoundConfig, this, _1, _2);
	checkSoundConfigThreadSafe(config, Config::AUDIO_OUTPUT_DEVICENAME_KEY);

	init();
}

QtStatusBar::~QtStatusBar() {
	//TODO: unregister events, delete created objects
}

void QtStatusBar::showMessage(const QString & message, int timeout) {
	_statusBar->showMessage(message, timeout);
}

void QtStatusBar::checkSoundConfig(Settings & sender, const std::string & key) {
	typedef PostEvent2<void (Settings & sender, const std::string &), Settings &, std::string> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtStatusBar::checkSoundConfigThreadSafe, this, _1, _2), sender, key);
	postEvent(event);
}

void QtStatusBar::checkSoundConfigThreadSafe(Settings & sender, const std::string & key) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if (key == Config::AUDIO_OUTPUT_DEVICENAME_KEY ||
		key == Config::AUDIO_INPUT_DEVICENAME_KEY ||
		key == Config::AUDIO_RINGER_DEVICENAME_KEY) {

		//FIXME This is responsible for a lot of crashes
		//AudioDevice::selectAsRecordDevice(config.getAudioInputDeviceName(), AudioDevice::TypeInputMicrophone);
		try {
			VolumeControl outputVolumeControl(config.getAudioOutputDeviceName(), VolumeControl::DeviceTypeOutput);
			//outputVolumeControl.setMute(false);
			VolumeControl inputVolumeControl(config.getAudioInputDeviceName(), VolumeControl::DeviceTypeInput);
			//inputVolumeControl.setMute(false);

			if (!outputVolumeControl.isMuted() &&
				/*outputVolumeControl.getLevel() > 0 &&*/
				!inputVolumeControl.isMuted()/* &&
				inputVolumeControl.getLevel() > 0*/) {

				_soundStateLabel->setPixmap(QPixmap(":/pics/statusbar/status-audio-online.png"));
				_soundStateLabel->setToolTip(tr("Audio Configuration OK"));
			}
		} catch (Exception & e) {
		}
	}
}

void QtStatusBar::connectionIsDownEventHandler() {
	typedef PostEvent1<void (bool), bool> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtStatusBar::connectionStateEventHandlerThreadSafe, this, _1), false);
	postEvent(event);
}

void QtStatusBar::connectionIsUpEventHandler() {
	typedef PostEvent1<void (bool), bool> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtStatusBar::connectionStateEventHandlerThreadSafe, this, _1), true);
	postEvent(event);
}

void QtStatusBar::phoneLineStateChanged(EnumPhoneLineState::PhoneLineState state) {
	updatePhoneLineState();
}

void QtStatusBar::connectionStateEventHandlerThreadSafe(bool connected) {
	QString tooltip;
	QString pixmap;

	//Stops animated pixmap
	delete _internetConnectionMovie;
	_internetConnectionMovie = NULL;

	if (connected) {
		tooltip = tr("Internet Connection OK");
		pixmap = ":/pics/statusbar/status-earth-connecting.mng";
	} else {
		tooltip = tr("Internet Connection Error");
		pixmap = ":/pics/statusbar/status-earth-offline.png";
	}

	_internetConnectionStateLabel->setPixmap(pixmap);
	_internetConnectionStateLabel->setToolTip(tooltip);
}

void QtStatusBar::updateInternetConnectionState() {
	connectionStateEventHandlerThreadSafe(NetworkObserver::getInstance().isConnected());
}

void QtStatusBar::updatePhoneLineState() {
	EnumPhoneLineState::PhoneLineState state;

	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getActivePhoneLine()) {
		//FIXME: next line could crash if active phone line is destroyed during update
		state = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getActivePhoneLine()->getState().getCode();

		QString tooltip;
		QString pixmap;

		//Stops animated pixmap
		delete _sipConnectionMovie;
		_sipConnectionMovie = NULL;

		switch (state) {
		case EnumPhoneLineState::PhoneLineStateUnknown:
			tooltip = tr("Not connected");
			pixmap = ":/pics/statusbar/status-network-offline.png";
			break;

		case EnumPhoneLineState::PhoneLineStateServerError:
			tooltip = tr("An error occured");
			pixmap = ":/pics/statusbar/status-network-offline.png";
			break;

		case EnumPhoneLineState::PhoneLineStateTimeout:
			tooltip = tr("An error occured");
			pixmap = ":/pics/statusbar/status-network-offline.png";
			break;

		case EnumPhoneLineState::PhoneLineStateOk:
			tooltip = tr("Register done");
			pixmap = ":/pics/statusbar/status-network-online-static.png";
			break;

		case EnumPhoneLineState::PhoneLineStateClosed:
			tooltip = tr("Unregister done");
			pixmap = ":/pics/statusbar/status-network-offline.png";
			break;

		case EnumPhoneLineState::PhoneLineStateProgress:
			tooltip = tr("Not connected");
			_sipConnectionMovie = new QMovie(":/pics/statusbar/status-network-connecting.mng", MNG_FORMAT, _statusBar);
			_phoneLineStateLabel->setMovie(_sipConnectionMovie);
			_phoneLineStateLabel->setToolTip(tr("Connecting"));
			_sipConnectionMovie->start();
			return;

		default:
			LOG_FATAL("unknown state=" + EnumPhoneLineState::toString(state));
		};

		_phoneLineStateLabel->setPixmap(pixmap);
		_phoneLineStateLabel->setToolTip(tooltip);
	}
}

void QtStatusBar::init() {
	updateInternetConnectionState();
	updatePhoneLineState();
}
