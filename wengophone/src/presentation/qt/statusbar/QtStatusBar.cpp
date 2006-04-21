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

QtStatusBar::QtStatusBar(CWengoPhone & cWengoPhone, QStatusBar * statusBar)
	: QObjectThreadSafe(),
	_cWengoPhone(cWengoPhone) {

	_statusBar = statusBar;

	_cWengoPhone.networkDiscoveryStateChangedEvent +=
		boost::bind(&QtStatusBar::networkDiscoveryStateChangedEventHandler, this, _1, _2);

	//FIXME Ugly code to be removed
	//inside a QStyle class?
	/*QPalette statusBarPalette = _statusBar->palette();
	statusBarPalette.setColor(QPalette::Window, QColor(60, 60, 60));
	_statusBar->setPalette(statusBarPalette);
	_statusBar->setAutoFillBackground(true);*/
	//

	_internetConnectionStateLabel = new QLabel(_statusBar);
	_internetConnectionStateLabel->setPixmap(QPixmap(":/pics/statusbar_connect_error.png"));
	_internetConnectionStateLabel->setToolTip(tr("Not connected"));
	_statusBar->addPermanentWidget(_internetConnectionStateLabel);

	_phoneLineStateLabel = new QLabel(_statusBar);
	_phoneLineStateLabel->setPixmap(QPixmap(":/pics/statusbar_sip_error.png"));
	_phoneLineStateLabel->setToolTip(tr("Not connected"));
	_statusBar->addPermanentWidget(_phoneLineStateLabel);

	_soundStateLabel = new QLabel(_statusBar);
	_soundStateLabel->setPixmap(QPixmap(":/pics/statusbar_sound_error.png"));
	_phoneLineStateLabel->setToolTip(tr("Sound configuration error"));
	_statusBar->addPermanentWidget(_soundStateLabel);

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.valueChangedEvent += boost::bind(&QtStatusBar::checkSoundConfig, this);
	checkSoundConfigThreadSafe();
}

void QtStatusBar::showMessage(const QString & message) {
	_statusBar->showMessage(message);
}

void QtStatusBar::checkSoundConfig() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtStatusBar::checkSoundConfigThreadSafe, this));
	postEvent(event);
}

void QtStatusBar::checkSoundConfigThreadSafe() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

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

		_soundStateLabel->setPixmap(QPixmap(":/pics/statusbar_sound.png"));
		_phoneLineStateLabel->setToolTip(tr("Sound configuration ok"));
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

	switch (state) {
	case SipAccount::NetworkDiscoveryStateOk:
		tooltip = tr("Internet connection OK");
		pixmap = ":/pics/statusbar_connect.png";
		break;

	case SipAccount::NetworkDiscoveryStateHTTPError:
		tooltip = tr("Internet connection error");
		pixmap = ":/pics/statusbar_connect_error.png";
		break;

	case SipAccount::NetworkDiscoveryStateSIPError:
		tooltip = tr("Internet connection error");
		pixmap = ":/pics/statusbar_connect_error.png";
		break;

	case SipAccount::NetworkDiscoveryStateProxyNeedsAuthentication:
		tooltip = tr("Internet connection error");
		pixmap = ":/pics/statusbar_connect_error.png";
		break;

	case SipAccount::NetworkDiscoveryStateError:
		tooltip = tr("Internet connection error");
		pixmap = ":/pics/statusbar_connect_error.png";
		break;

	default:
		LOG_FATAL("unknown state=" + String::fromNumber(state));
	};

	_internetConnectionStateLabel->setPixmap(pixmap);
	_internetConnectionStateLabel->setToolTip(tooltip);
}
