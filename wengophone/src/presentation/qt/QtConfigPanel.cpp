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
#include "QtSliderManager.h"

#include "ui_WengoPhoneWindowConfigPanel.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <presentation/qt/config/QtWengoConfigDialog.h>

#include <sound/AudioDeviceManager.h>
#include <sound/VolumeControl.h>

#include <util/Logger.h>
#include <qtutil/MouseEventFilter.h>

#include <QtGui>

QtConfigPanel::QtConfigPanel(CWengoPhone & cWengoPhone, QWidget * parent)
	: QObjectThreadSafe(parent), _cWengoPhone(cWengoPhone) {

	_configPanelWidget = new QWidget(NULL);

	_ui = new Ui::WengoPhoneWindowConfigPanel();
	_ui->setupUi(_configPanelWidget);

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.valueChangedEvent += boost::bind(&QtConfigPanel::configChangedEventHandler, this, _1, _2);

	//inputSoundSlider
	_inputSoundSlider = new QtSliderManager(this);
	_inputSoundSlider->addLabel(_ui->mic_vol0, ":pics/audio/progress_1.png", ":pics/audio/progress_1_off.png");
	_inputSoundSlider->addLabel(_ui->mic_vol1, ":pics/audio/progress_2.png", ":pics/audio/progress_2_off.png");
	_inputSoundSlider->addLabel(_ui->mic_vol2, ":pics/audio/progress_3.png", ":pics/audio/progress_3_off.png");
	_inputSoundSlider->addLabel(_ui->mic_vol3, ":pics/audio/progress_4.png", ":pics/audio/progress_4_off.png");
	_inputSoundSlider->addLabel(_ui->mic_vol4, ":pics/audio/progress_5.png", ":pics/audio/progress_5_off.png");
	_inputSoundSlider->addLabel(_ui->mic_vol5, ":pics/audio/progress_6.png", ":pics/audio/progress_6_off.png");
	_inputSoundSlider->addLabel(_ui->mic_vol6, ":pics/audio/progress_7.png", ":pics/audio/progress_7_off.png");
	connect(_inputSoundSlider, SIGNAL(volumeChanged(int)), SLOT(inputSoundSliderValueChanged(int)));

	//outputSoundSlider
	_outputSoundSlider = new QtSliderManager(this);
	_outputSoundSlider->addLabel(_ui->hp_vol0, ":pics/audio/progress_1.png", ":pics/audio/progress_1_off.png");
	_outputSoundSlider->addLabel(_ui->hp_vol1, ":pics/audio/progress_2.png", ":pics/audio/progress_2_off.png");
	_outputSoundSlider->addLabel(_ui->hp_vol2, ":pics/audio/progress_3.png", ":pics/audio/progress_3_off.png");
	_outputSoundSlider->addLabel(_ui->hp_vol3, ":pics/audio/progress_4.png", ":pics/audio/progress_4_off.png");
	_outputSoundSlider->addLabel(_ui->hp_vol4, ":pics/audio/progress_5.png", ":pics/audio/progress_5_off.png");
	_outputSoundSlider->addLabel(_ui->hp_vol5, ":pics/audio/progress_6.png", ":pics/audio/progress_6_off.png");
	_outputSoundSlider->addLabel(_ui->hp_vol6, ":pics/audio/progress_7.png", ":pics/audio/progress_7_off.png");
	connect(_outputSoundSlider, SIGNAL(volumeChanged(int)), SLOT(outputSoundSliderValueChanged(int)));

	//enableWenboxCheckBox
	connect(_ui->enableWenboxCheckBox, SIGNAL(toggled(bool)), SLOT(enableWenboxCheckBoxToggled(bool)));

	//halfDuplexCheckBox
	connect(_ui->halfDuplexCheckBox, SIGNAL(toggled(bool)), SLOT(halfDuplexCheckBoxToggled(bool)));

	configChangedEventHandler(config, Config::AUDIO_INPUT_DEVICEID_KEY);
	configChangedEventHandler(config, Config::AUDIO_OUTPUT_DEVICEID_KEY);
	configChangedEventHandler(config, Config::WENBOX_ENABLE_KEY);
	configChangedEventHandler(config, Config::AUDIO_HALFDUPLEX_KEY);

	MousePressEventFilter * mouseFilter = new MousePressEventFilter(
		this, SLOT(videoSettingsClicked()), Qt::LeftButton);
	_ui->videoSettingsLabel->installEventFilter(mouseFilter);

	MousePressEventFilter * mouseFilter2 = new MousePressEventFilter(
		this, SLOT(audioSettingsClicked()), Qt::LeftButton);
	_ui->audioSettingsLabel->installEventFilter(mouseFilter2);
}

QtConfigPanel::~QtConfigPanel() {
	delete _ui;
}

void QtConfigPanel::inputSoundSliderValueChanged(int value) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	VolumeControl volumeControl(AudioDevice(config.getAudioInputDeviceId()));
	volumeControl.setLevel(value);
}

void QtConfigPanel::outputSoundSliderValueChanged(int value) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	VolumeControl volumeControl(AudioDevice(config.getAudioOutputDeviceId()));
	volumeControl.setLevel(value);
}

void QtConfigPanel::enableWenboxCheckBoxToggled(bool checked) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::WENBOX_ENABLE_KEY, checked);
}


void QtConfigPanel::halfDuplexCheckBoxToggled(bool checked) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::AUDIO_HALFDUPLEX_KEY, checked);
}

void QtConfigPanel::configChangedEventHandler(Settings & sender, const std::string & key) {
	typedef PostEvent2<void (Settings & sender, const std::string &), Settings &, std::string> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtConfigPanel::configChangedEventHandlerThreadSafe, this, _1, _2), sender, key);
	postEvent(event);
}

void QtConfigPanel::configChangedEventHandlerThreadSafe(Settings & sender, const std::string & key) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if (key == Config::AUDIO_INPUT_DEVICEID_KEY) {
		//inputSoundSlider
		VolumeControl inputVolumeControl(AudioDevice(config.getAudioInputDeviceId()));
		_inputSoundSlider->setVolume(inputVolumeControl.getLevel());
	}

	if (key == Config::AUDIO_OUTPUT_DEVICEID_KEY) {
		//outputSoundSlider
		VolumeControl outputVolumeControl(AudioDevice(config.getAudioOutputDeviceId()));
		_outputSoundSlider->setVolume(outputVolumeControl.getLevel());
	}

	if (key == Config::WENBOX_ENABLE_KEY) {
		//enableWenboxCheckBox
		_ui->enableWenboxCheckBox->setChecked(config.getWenboxEnable());
	}

	if (key == Config::AUDIO_HALFDUPLEX_KEY) {
		//halfDuplexCheckBox
		_ui->halfDuplexCheckBox->setChecked(config.getAudioHalfDuplex());
	}
}

void QtConfigPanel::videoSettingsClicked() {
	QtWengoConfigDialog dialog(_cWengoPhone, _configPanelWidget);
	dialog.showVideoPage();
	dialog.show();
}

void QtConfigPanel::audioSettingsClicked() {
	QtWengoConfigDialog dialog(_cWengoPhone, _configPanelWidget);
	dialog.showAudioPage();
	dialog.show();
}

void QtConfigPanel::slotTranslationChanged() {
	_ui->retranslateUi(_configPanelWidget);
}
