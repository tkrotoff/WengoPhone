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

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <sound/AudioDevice.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>
#include <qtutil/StringListConvert.h>

QtAudioSettings::QtAudioSettings(QWidget * parent, Qt::WFlags f) : QWidget (parent, f) {
	_widget = WidgetFactory::create(":/forms/config/AudioSettings.ui", this);
	QGridLayout * layout = new QGridLayout();
	layout->addWidget(_widget);
	layout->setMargin(0);
	setLayout(layout);
	setupChilds();
}

void QtAudioSettings::setupChilds() {
	//inputDeviceList
	_inputDeviceComboBox = Object::findChild<QComboBox *>(_widget, "inputDeviceComboBox");
	StringList inputDeviceList = AudioDevice::getInputMixerDeviceList();
	_inputDeviceComboBox->addItems(StringListConvert::toQStringList(inputDeviceList));

	//outputDeviceList
	_outputDeviceComboBox = Object::findChild<QComboBox *>(_widget, "outputDeviceComboBox");
	StringList outputDeviceList = AudioDevice::getOutputMixerDeviceList();
	_outputDeviceComboBox->addItems(StringListConvert::toQStringList(outputDeviceList));

	//ringingDeviceList = outputDeviceList
	_ringingDeviceComboBox = Object::findChild<QComboBox *>(_widget, "ringingDeviceComboBox");
	_ringingDeviceComboBox->addItems(StringListConvert::toQStringList(outputDeviceList));

	_makeTestCallPushButton = Object::findChild<QPushButton *>(_widget, "makeTestCallPushButton");

	_personalAudioConfig = Object::findChild<QGroupBox *>(_widget, "personalAudioConfig");

	readConfigData();
}

void QtAudioSettings::saveData() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set(Config::AUDIO_INPUT_DEVICENAME_KEY, _inputDeviceComboBox->currentText().toStdString());
	config.set(Config::AUDIO_OUTPUT_DEVICENAME_KEY, _outputDeviceComboBox->currentText().toStdString());
	config.set(Config::AUDIO_RINGER_DEVICENAME_KEY, _ringingDeviceComboBox->currentText().toStdString());

	config.set(Config::AUDIO_PERSONAL_CONFIGURATION_KEY, _personalAudioConfig->isChecked());
}

void QtAudioSettings::readConfigData() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_inputDeviceComboBox->setCurrentIndex(_inputDeviceComboBox->findText(QString::fromStdString(config.getAudioInputDeviceName())));
	_outputDeviceComboBox->setCurrentIndex(_outputDeviceComboBox->findText(QString::fromStdString(config.getAudioOutputDeviceName())));
	_ringingDeviceComboBox->setCurrentIndex(_ringingDeviceComboBox->findText(QString::fromStdString(config.getAudioRingerDeviceName())));

	_personalAudioConfig->setChecked(config.getAudioPersonalConfiguration());
}
