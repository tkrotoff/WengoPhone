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

#include <sound/AudioDevice.h>

#include <qtutil/StringListConvert.h>

#include <QtGui>

QtAudioSettings::QtAudioSettings(QWidget * parent)
	: QObject(parent) {

	_audioSettingsWidget = new QWidget(parent);

	_ui = new Ui::AudioSettings();
	_ui->setupUi(_audioSettingsWidget);

	//inputDeviceList
	StringList inputDeviceList = AudioDevice::getInputMixerDeviceList();
	_ui->inputDeviceComboBox->addItems(StringListConvert::toQStringList(inputDeviceList));

	//outputDeviceList
	StringList outputDeviceList = AudioDevice::getOutputMixerDeviceList();
	_ui->outputDeviceComboBox->addItems(StringListConvert::toQStringList(outputDeviceList));

	//ringingDeviceList = outputDeviceList
	_ui->ringingDeviceComboBox->addItems(StringListConvert::toQStringList(outputDeviceList));

	readConfig();
}

QtAudioSettings::~QtAudioSettings() {
	delete _ui;
}

void QtAudioSettings::saveConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set(Config::AUDIO_INPUT_DEVICENAME_KEY, _ui->inputDeviceComboBox->currentText().toStdString());
	config.set(Config::AUDIO_OUTPUT_DEVICENAME_KEY, _ui->outputDeviceComboBox->currentText().toStdString());
	config.set(Config::AUDIO_RINGER_DEVICENAME_KEY, _ui->ringingDeviceComboBox->currentText().toStdString());

	config.set(Config::AUDIO_PERSONAL_CONFIGURATION_KEY, _ui->personalAudioConfig->isChecked());
}

void QtAudioSettings::readConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_ui->inputDeviceComboBox->setCurrentIndex(_ui->inputDeviceComboBox->findText(QString::fromStdString(config.getAudioInputDeviceName())));
	_ui->outputDeviceComboBox->setCurrentIndex(_ui->outputDeviceComboBox->findText(QString::fromStdString(config.getAudioOutputDeviceName())));
	_ui->ringingDeviceComboBox->setCurrentIndex(_ui->ringingDeviceComboBox->findText(QString::fromStdString(config.getAudioRingerDeviceName())));

	_ui->personalAudioConfig->setChecked(config.getAudioPersonalConfiguration());
}
