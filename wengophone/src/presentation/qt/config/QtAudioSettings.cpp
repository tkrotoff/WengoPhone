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

#include "QtAudioSettings.h"

#include "ui_AudioSettings.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/profile/UserProfile.h>
#include <model/phoneline/IPhoneLine.h>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <sound/AudioDevice.h>

#include <qtutil/StringListConvert.h>

#include <QtGui>

static const char * AUDIO_TEST_CALL = "333";

QtAudioSettings::QtAudioSettings(CWengoPhone & cWengoPhone, QWidget * parent)
	: QObject(parent), _cWengoPhone(cWengoPhone) {

	_audioSettingsWidget = new QWidget(NULL);

	_ui = new Ui::AudioSettings();
	_ui->setupUi(_audioSettingsWidget);

	connect(_ui->makeTestCallButton, SIGNAL(pressed()), SLOT(makeTestCallClicked()));

	readConfig();
}

QtAudioSettings::~QtAudioSettings() {
	delete _ui;
}

QString QtAudioSettings::getName() const {
	return tr("Audio");
}

void QtAudioSettings::saveConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set(Config::AUDIO_INPUT_DEVICENAME_KEY, _ui->inputDeviceComboBox->currentText().toStdString());
	config.set(Config::AUDIO_OUTPUT_DEVICENAME_KEY, _ui->outputDeviceComboBox->currentText().toStdString());
	config.set(Config::AUDIO_RINGER_DEVICENAME_KEY, _ui->ringingDeviceComboBox->currentText().toStdString());
}

void QtAudioSettings::readConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//inputDeviceList
	StringList inputDeviceList;
	inputDeviceList += AudioDevice::getDefaultRecordDevice();
	inputDeviceList += AudioDevice::getInputMixerDeviceList();
	inputDeviceList.removeDuplicatedStrings();

	_ui->inputDeviceComboBox->clear();
	_ui->inputDeviceComboBox->addItems(StringListConvert::toQStringList(inputDeviceList));
	_ui->inputDeviceComboBox->setCurrentIndex(_ui->inputDeviceComboBox->findText(QString::fromUtf8(config.getAudioInputDeviceName().c_str())));

	//outputDeviceList
	StringList outputDeviceList;
	outputDeviceList += AudioDevice::getDefaultPlaybackDevice();
	outputDeviceList += AudioDevice::getOutputMixerDeviceList();
	outputDeviceList.removeDuplicatedStrings();

	_ui->outputDeviceComboBox->clear();
	_ui->outputDeviceComboBox->addItems(StringListConvert::toQStringList(outputDeviceList));
	_ui->outputDeviceComboBox->setCurrentIndex(_ui->outputDeviceComboBox->findText(QString::fromUtf8(config.getAudioOutputDeviceName().c_str())));

	//ringingDeviceList = outputDeviceList
	_ui->ringingDeviceComboBox->clear();
	_ui->ringingDeviceComboBox->addItems(StringListConvert::toQStringList(outputDeviceList));
	_ui->ringingDeviceComboBox->setCurrentIndex(_ui->ringingDeviceComboBox->findText(QString::fromUtf8(config.getAudioRingerDeviceName().c_str())));
}

void QtAudioSettings::makeTestCallClicked() {
	CUserProfile * cUserProfile = _cWengoPhone.getCUserProfileHandler().getCUserProfile();
	if (cUserProfile) {
		cUserProfile->getUserProfile().getActivePhoneLine()->makeCall(AUDIO_TEST_CALL, false);
	}
}
