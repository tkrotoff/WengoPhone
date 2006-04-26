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

#include "QtConfigPanel.h"

#include "ui_WengoPhoneWindowConfigPanel.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <sound/VolumeControl.h>
#include <sound/AudioDevice.h>

#include <util/Logger.h>

#include <QtGui>

QtConfigPanel::QtConfigPanel(QWidget * parent)
	: QObjectThreadSafe(parent) {

	_configPanelWidget = new QWidget(parent);

	_ui = new Ui::WengoPhoneWindowConfigPanel();
	_ui->setupUi(_configPanelWidget);

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.valueChangedEvent += boost::bind(&QtConfigPanel::configChangedEventHandler, this, _1, _2);

	//inputSoundSlider
	connect(_ui->inputSoundSlider, SIGNAL(valueChanged(int)), SLOT(inputSoundSliderValueChanged(int)));

	//outputSoundSlider
	connect(_ui->outputSoundSlider, SIGNAL(valueChanged(int)), SLOT(outputSoundSliderValueChanged(int)));

	//enableVideoCheckBox
	connect(_ui->enableVideoCheckBox, SIGNAL(toggled(bool)), SLOT(enableVideoCheckBoxToggled(bool)));

	//enableWenboxCheckBox
	connect(_ui->enableWenboxCheckBox, SIGNAL(toggled(bool)), SLOT(enableWenboxCheckBoxToggled(bool)));
}

QtConfigPanel::~QtConfigPanel() {
	delete _ui;
}

void QtConfigPanel::inputSoundSliderValueChanged(int value) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	VolumeControl volumeControl(config.getAudioInputDeviceName(), VolumeControl::DeviceTypeInput);
	volumeControl.setLevel(value);
}

void QtConfigPanel::outputSoundSliderValueChanged(int value) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	VolumeControl volumeControl(config.getAudioOutputDeviceName(), VolumeControl::DeviceTypeOutput);
	volumeControl.setLevel(value);
}

void QtConfigPanel::enableVideoCheckBoxToggled(bool checked) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::VIDEO_ENABLE_KEY, checked);
}

void QtConfigPanel::enableWenboxCheckBoxToggled(bool checked) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::WENBOX_ENABLE_KEY, checked);
}

void QtConfigPanel::configChangedEventHandler(Settings & sender, const std::string & key) {
	typedef PostEvent2<void (Settings & sender, const std::string &), Settings &, std::string> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtConfigPanel::configChangedEventHandlerThreadSafe, this, _1, _2), sender, key);
	postEvent(event);
}

void QtConfigPanel::configChangedEventHandlerThreadSafe(Settings & sender, const std::string & key) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if (key == Config::AUDIO_OUTPUT_DEVICENAME_KEY ||
		key == Config::AUDIO_INPUT_DEVICENAME_KEY) {

		//inputSoundSlider
		VolumeControl inputVolumeControl(config.getAudioInputDeviceName(), VolumeControl::DeviceTypeInput);
		_ui->inputSoundSlider->setValue(inputVolumeControl.getLevel());

		//outputSoundSlider
		VolumeControl outputVolumeControl(config.getAudioOutputDeviceName(), VolumeControl::DeviceTypeOutput);
		_ui->outputSoundSlider->setValue(outputVolumeControl.getLevel());
	}

	if (key == Config::VIDEO_ENABLE_KEY) {
		//enableVideoCheckBox
		_ui->enableVideoCheckBox->setChecked(config.getVideoEnable());
	}

	if (key == Config::WENBOX_ENABLE_KEY) {
		//enableWenboxCheckBox
		_ui->enableWenboxCheckBox->setChecked(config.getWenboxEnable());
	}
}
