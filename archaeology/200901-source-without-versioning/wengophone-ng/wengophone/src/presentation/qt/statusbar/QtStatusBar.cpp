/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/network/NetworkObserver.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/phoneline/PhoneLineState.h>
#include <model/profile/UserProfile.h>

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/QtToolBar.h>

#include <sound/VolumeControl.h>
#include <sound/AudioDevice.h>
#include <qtutil/LanguageChangeEventFilter.h>
#include <qtutil/SafeConnect.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <QtGui/QtGui>

static const char * MNG_FORMAT = "MNG";

QtStatusBar::QtStatusBar(CWengoPhone & cWengoPhone, QStatusBar * statusBar)
	: QWidget(statusBar),
	_cWengoPhone(cWengoPhone)
	{

	_statusBar = statusBar;
	_statusBar->addPermanentWidget(this);

	_ui = new Ui::StatusBarWidget();
	_ui->setupUi(this);

	updateEncryptionState(false);

	SAFE_CONNECT(this, SIGNAL(updateInternetIcon(bool)), SLOT(updateInternetIconSlot(bool)));
	SAFE_CONNECT(this, SIGNAL(updateSoundIcon()), SLOT(updateSoundIconSlot()));
	LANGUAGE_CHANGE(this);

	//internetConnectionLabel
	NetworkObserver::getInstance().connectionIsDownEvent += boost::bind(&QtStatusBar::connectionIsDownEventHandler, this);
	NetworkObserver::getInstance().connectionIsUpEvent += boost::bind(&QtStatusBar::connectionIsUpEventHandler, this);

	_internetConnectionMovie = new QMovie(":/pics/statusbar/internet_status_connecting.mng", MNG_FORMAT, this);
	_ui->internetConnectionLabel->setMovie(_internetConnectionMovie);
	_internetConnectionMovie->start();
	if (NetworkObserver::getInstance().isConnected()) {
		connectionIsUpEventHandler();
	}
	////

	//sipConnectionLabel
	_sipConnectionMovie = new QMovie(":/pics/statusbar/network_status_connecting.mng", MNG_FORMAT, this);
	_ui->sipConnectionLabel->setMovie(_sipConnectionMovie);
	_sipConnectionMovie->start();
	////

	//soundButton
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.valueChangedEvent += boost::bind(&QtStatusBar::checkSoundConfig, this, _1);
	SAFE_CONNECT(_ui->soundButton, SIGNAL(clicked()), SIGNAL(toggleConfigPanel()));
	////

	//status bar re-initialization
	SAFE_CONNECT((QtWengoPhone *) _cWengoPhone.getPresentation(), SIGNAL(userProfileDeleted()), SLOT(userProfileDeleted()));

	init();
}

QtStatusBar::~QtStatusBar() {
	OWSAFE_DELETE(_ui);
}

void QtStatusBar::showMessage(const QString & message, int timeout) {
	_statusBar->showMessage(message, timeout);
}

void QtStatusBar::checkSoundConfig(const std::string & key) {
	if (key == Config::AUDIO_OUTPUT_DEVICEID_KEY ||
		key == Config::AUDIO_INPUT_DEVICEID_KEY ||
		key == Config::AUDIO_RINGER_DEVICEID_KEY) {
		updateSoundIcon();
	}
}

void QtStatusBar::updateSoundIconSlot() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//FIXME This is responsible for a lot of crashes
	//AudioDevice::selectAsRecordDevice(config.getAudioInputDeviceName(), AudioDevice::TypeInputMicrophone);
	VolumeControl outputVolumeControl(config.getAudioOutputDeviceId());
	//outputVolumeControl.setMute(false);
	VolumeControl inputVolumeControl(config.getAudioInputDeviceId());
	//inputVolumeControl.setMute(false);

	QStringList messages;
	if (outputVolumeControl.isMuted()) {
		messages << tr("- Output volume is muted.");
	} else if (outputVolumeControl.getLevel() == 0) {
		messages << tr("- Output volume is 0.");
	}
	if (inputVolumeControl.isMuted()) {
		messages << tr("- Input volume is muted.");
	} else if (inputVolumeControl.getLevel() == 0) {
		messages << tr("- Input volume is 0.");
	}

	QString toolTip;
	QString pixmap;
	if (messages.empty()) {
		toolTip = tr("Audio Configuration OK");
		pixmap = ":/pics/statusbar/audio_status_ok.png";
	} else {
		toolTip = tr("Audio Configuration Errors:\n");
		toolTip += messages.join("\n");
		pixmap = ":/pics/statusbar/audio_status_error.png";
	}
	_ui->soundButton->setIcon(QPixmap(pixmap));
	_ui->soundButton->setToolTip(toolTip);
}

void QtStatusBar::connectionIsDownEventHandler() {
	updateInternetIcon(false);
}

void QtStatusBar::connectionIsUpEventHandler() {
	updateInternetIcon(true);
}

void QtStatusBar::updateInternetIconSlot(bool connected) {
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

void QtStatusBar::userProfileDeleted() {
	_ui->sipConnectionLabel->setPixmap(QPixmap(":/pics/statusbar/network_status_error.png"));
	_ui->sipConnectionLabel->setToolTip(tr("Not connected"));
}

void QtStatusBar::updatePhoneLineState(EnumPhoneLineState::PhoneLineState state) {

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
			_sipConnectionMovie = new QMovie(":/pics/statusbar/network_status_connecting.mng", MNG_FORMAT, this);
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

void QtStatusBar::init() {
	updateInternetIcon(NetworkObserver::getInstance().isConnected());
	updateSoundIcon();

	// Init hack: if the event has been sent before being binded
	EnumPhoneLineState::PhoneLineState state;
	CUserProfile *cUserProfile = _cWengoPhone.getCUserProfileHandler().getCUserProfile();
	if (cUserProfile) {
		IPhoneLine * phoneLine = cUserProfile->getUserProfile().getActivePhoneLine();
		if (phoneLine) {
			//FIXME: next line could crash if active phone line is destroyed during update
			state = phoneLine->getState().getCode();
			updatePhoneLineState(state);
		}
	}
	////
}

void QtStatusBar::languageChanged() {
	_ui->retranslateUi(this);
	init();
}

void QtStatusBar::updateEncryptionState(bool enable) {
	_ui->encrytionLabel->setVisible(enable);
}
