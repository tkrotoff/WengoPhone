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

#include <control/CWengoPhone.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <sound/VolumeControl.h>
#include <sound/AudioDevice.h>

#include <util/Logger.h>

#include <QtGui>

static const char * MNG_FORMAT = "MNG";

QtStatusBar::QtStatusBar(CWengoPhone & cWengoPhone, QStatusBar * statusBar)
	: QObjectThreadSafe(statusBar),
	_cWengoPhone(cWengoPhone) {

	_statusBar = statusBar;

	QWidget * statusGroup = new QWidget(_statusBar);
	new QHBoxLayout(statusGroup);
	statusGroup->layout()->setMargin(0);
	statusGroup->layout()->setSpacing(2);

	_wengoConnectionMovie = new QMovie(":/pics/statusbar/status-network-connecting.mng", MNG_FORMAT, _statusBar);
	_internetConnectionMovie = new QMovie(":/pics/statusbar/status-earth-connecting.mng", MNG_FORMAT, _statusBar);

	_cWengoPhone.networkDiscoveryStateChangedEvent +=
		boost::bind(&QtStatusBar::networkDiscoveryStateChangedEventHandler, this, _1, _2);

	//internetConnectionStateLabel
	_internetConnectionStateLabel = new QLabel(statusGroup);
	_internetConnectionStateLabel->setMovie(_internetConnectionMovie);
	_internetConnectionStateLabel->setToolTip(tr("Not Connected"));
	statusGroup->layout()->addWidget(_internetConnectionStateLabel);
	_internetConnectionMovie->start();

	//phoneLineStateLabel
	_phoneLineStateLabel = new QLabel(statusGroup);
	_phoneLineStateLabel->setMovie(_wengoConnectionMovie);
	_phoneLineStateLabel->setToolTip(tr("Not Connected"));
	statusGroup->layout()->addWidget(_phoneLineStateLabel);
	_wengoConnectionMovie->start();

	//soundStateLabel
	_soundStateLabel = new QLabel(statusGroup);
	_soundStateLabel->setPixmap(QPixmap(":/pics/statusbar_audio_error.png"));
	_soundStateLabel->setToolTip(tr("Audio Configuration Error"));
	statusGroup->layout()->addWidget(_soundStateLabel);


	_statusBar->addPermanentWidget(statusGroup);

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.valueChangedEvent += boost::bind(&QtStatusBar::checkSoundConfig, this, _1, _2);
	checkSoundConfigThreadSafe(config, Config::AUDIO_OUTPUT_DEVICENAME_KEY);
}

void QtStatusBar::showMessage(const QString & message) {
	_statusBar->showMessage(message);
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

		AudioDevice::selectAsRecordDevice(config.getAudioInputDeviceName(), AudioDevice::TypeInputMicrophone);
		VolumeControl outputVolumeControl(config.getAudioOutputDeviceName(), VolumeControl::DeviceTypeOutput);
		outputVolumeControl.setMute(false);
		VolumeControl inputVolumeControl(config.getAudioInputDeviceName(), VolumeControl::DeviceTypeInput);
		inputVolumeControl.setMute(false);

		if (!outputVolumeControl.isMuted() &&
			outputVolumeControl.getLevel() > 0 &&
			!inputVolumeControl.isMuted() &&
			inputVolumeControl.getLevel() > 0 &&
			AudioDevice::TypeInputMicrophone == AudioDevice::getSelectedRecordDevice(config.getAudioInputDeviceName())) {

			_soundStateLabel->setPixmap(QPixmap(":/pics/statusbar_audio.png"));
			_phoneLineStateLabel->setToolTip(tr("Audio Configuration OK"));
		}
	}
}

void QtStatusBar::networkDiscoveryStateChangedEventHandler(SipAccount & sender, SipAccount::NetworkDiscoveryState state) {
	typedef PostEvent2<void (SipAccount &, SipAccount::NetworkDiscoveryState), SipAccount &, SipAccount::NetworkDiscoveryState> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtStatusBar::networkDiscoveryStateChangedEventHandlerThreadSafe, this, _1, _2), sender, state);
	postEvent(event);
}

void QtStatusBar::networkDiscoveryStateChangedEventHandlerThreadSafe(SipAccount & sender, SipAccount::NetworkDiscoveryState state) {
	QString tooltip;
	QString pixmap;

	delete _internetConnectionMovie;
	_internetConnectionMovie = NULL;

	switch (state) {
	case SipAccount::NetworkDiscoveryStateOk:
		tooltip = tr("Internet Connection OK");
		pixmap = ":/pics/statusbar/status-earth-connecting.mng";
		break;

	case SipAccount::NetworkDiscoveryStateHTTPError:
		tooltip = tr("Internet Connection Error");
		pixmap = ":/pics/statusbar/status-earth-offline.png";
		break;

	case SipAccount::NetworkDiscoveryStateSIPError:
		tooltip = tr("Internet Connection Error");
		pixmap = ":/pics/statusbar/status-earth-offline.png";
		break;

	case SipAccount::NetworkDiscoveryStateProxyNeedsAuthentication:
		tooltip = tr("Internet Connection Error");
		pixmap = ":/pics/statusbar/status-earth-offline.png";
		break;

	case SipAccount::NetworkDiscoveryStateError:
		tooltip = tr("Internet Connection Error");
		pixmap = ":/pics/statusbar/status-earth-offline.png";
		break;

	default:
		LOG_FATAL("unknown state=" + String::fromNumber(state));
	};

	_internetConnectionStateLabel->setPixmap(pixmap);
	_internetConnectionStateLabel->setToolTip(tooltip);
}
