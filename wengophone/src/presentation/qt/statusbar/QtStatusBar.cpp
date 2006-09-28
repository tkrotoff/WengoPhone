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

#include "ui_StatusBarWidget.h"

#include <presentation/qt/QtToolBar.h>

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
#include <util/SafeDelete.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

static const char * MNG_FORMAT = "MNG";

QtStatusBar::QtStatusBar(CWengoPhone & cWengoPhone, QStatusBar * statusBar, QtToolBar * qtToolBar)
	: QObjectThreadSafe(statusBar),
	_cWengoPhone(cWengoPhone), _qtToolBar(qtToolBar) {

	_statusBar = statusBar;

	_statusBarWidget = new QWidget(NULL);

	_statusBar->addPermanentWidget(_statusBarWidget);

	_ui = new Ui::StatusBarWidget();
	_ui->setupUi(_statusBarWidget);

	//_cWengoPhone.networkDiscoveryStateChangedEvent += boost::bind(&QtStatusBar::networkDiscoveryStateChangedEventHandler, this, _1, _2);

	NetworkObserver::getInstance().connectionIsDownEvent += boost::bind(&QtStatusBar::connectionIsDownEventHandler, this);
	NetworkObserver::getInstance().connectionIsUpEvent += boost::bind(&QtStatusBar::connectionIsUpEventHandler, this);

	//internetConnectionLabel
	_internetConnectionMovie = new QMovie(":/pics/statusbar/internet_status_connecting.mng", MNG_FORMAT, _statusBarWidget);
	_ui->internetConnectionLabel->setMovie(_internetConnectionMovie);
	_internetConnectionMovie->start();
	if (NetworkObserver::getInstance().isConnected()) {
		connectionIsUpEventHandler();
	}

	//sipConnectionLabel
	_sipConnectionMovie = new QMovie(":/pics/statusbar/network_status_connecting.mng", MNG_FORMAT, _statusBarWidget);
	_ui->sipConnectionLabel->setMovie(_sipConnectionMovie);
	_sipConnectionMovie->start();

	//soundButton
	SAFE_CONNECT_RECEIVER(_ui->soundButton, SIGNAL(clicked()), _qtToolBar, SLOT(expandVolumePanel()));

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.valueChangedEvent += boost::bind(&QtStatusBar::checkSoundConfig, this, _1, _2);
	checkSoundConfigThreadSafe(Config::AUDIO_OUTPUT_DEVICEID_KEY);

	init();
}

QtStatusBar::~QtStatusBar() {
	//TODO: unregister events, delete created objects
	OWSAFE_DELETE(_ui);
}

void QtStatusBar::showMessage(const QString & message, int timeout) {
	_statusBar->showMessage(message, timeout);
}

void QtStatusBar::checkSoundConfig(Settings & sender, const std::string & key) {
	typedef PostEvent1<void (const std::string &), std::string> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtStatusBar::checkSoundConfigThreadSafe, this, _1), key);
	postEvent(event);
}

void QtStatusBar::checkSoundConfigThreadSafe(const std::string & key) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if (key == Config::AUDIO_OUTPUT_DEVICEID_KEY ||
		key == Config::AUDIO_INPUT_DEVICEID_KEY ||
		key == Config::AUDIO_RINGER_DEVICEID_KEY) {

		//FIXME This is responsible for a lot of crashes
		//AudioDevice::selectAsRecordDevice(config.getAudioInputDeviceName(), AudioDevice::TypeInputMicrophone);
		VolumeControl outputVolumeControl(config.getAudioOutputDeviceId());
		//outputVolumeControl.setMute(false);
		VolumeControl inputVolumeControl(config.getAudioInputDeviceId());
		//inputVolumeControl.setMute(false);

		if (!outputVolumeControl.isMuted() &&
			/*outputVolumeControl.getLevel() > 0 &&*/
			!inputVolumeControl.isMuted()/* &&
			inputVolumeControl.getLevel() > 0*/) {
			_ui->soundButton->setIcon(QPixmap(":/pics/statusbar/audio_status_ok.png"));
			_ui->soundButton->setToolTip(tr("Audio Configuration OK"));
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
	OWSAFE_DELETE(_internetConnectionMovie);

	if (connected) {
		tooltip = tr("Internet Connection OK");
		pixmap = ":/pics/statusbar/internet_status_connecting.mng";
	} else {
		tooltip = tr("Internet Connection Error");
		pixmap = ":/pics/statusbar/internet_status_error.png";
	}

	_ui->internetConnectionLabel->setPixmap(pixmap);
	_ui->internetConnectionLabel->setToolTip(tooltip);
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
		OWSAFE_DELETE(_sipConnectionMovie);

		switch (state) {
		case EnumPhoneLineState::PhoneLineStateUnknown:
			tooltip = tr("Not connected");
			pixmap = ":/pics/statusbar/network_status_error.png";
			break;

		case EnumPhoneLineState::PhoneLineStateServerError:
			tooltip = tr("An error occured");
			pixmap = ":/pics/statusbar/network_status_error.png";
			break;

		case EnumPhoneLineState::PhoneLineStateTimeout:
			tooltip = tr("An error occured");
			pixmap = ":/pics/statusbar/network_status_error.png";
			break;

		case EnumPhoneLineState::PhoneLineStateOk:
			tooltip = tr("Register done");
			pixmap = ":/pics/statusbar/network_status_ok.png";
			break;

		case EnumPhoneLineState::PhoneLineStateClosed:
			tooltip = tr("Unregister done");
			pixmap = ":/pics/statusbar/network_status_error.png";
			break;

		case EnumPhoneLineState::PhoneLineStateProgress:
			_sipConnectionMovie = new QMovie(":/pics/statusbar/network_status_connecting.mng", MNG_FORMAT, _statusBarWidget);
			_ui->sipConnectionLabel->setMovie(_sipConnectionMovie);
			_ui->sipConnectionLabel->setToolTip(tr("Connecting"));
			_sipConnectionMovie->start();
			return;

		default:
			LOG_FATAL("unknown state=" + EnumPhoneLineState::toString(state));
		};

		_ui->sipConnectionLabel->setPixmap(pixmap);
		_ui->sipConnectionLabel->setToolTip(tooltip);
	}
}

void QtStatusBar::init() {
	updateInternetConnectionState();
	updatePhoneLineState();
}
