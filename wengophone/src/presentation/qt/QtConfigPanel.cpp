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

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <sound/SoundMixer.h>

#include <util/Logger.h>

#include <qtutil/Object.h>
#include <qtutil/WidgetFactory.h>

#include <QtGui>

QtConfigPanel::QtConfigPanel(QWidget * parent)
	: QObject(parent) {

	_configPanelWidget = WidgetFactory::create(":/forms/WengoPhoneWindowConfigPanel.ui", parent);

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	_soundMixer = new SoundMixer(config.getAudioInputDeviceName(), config.getAudioOutputDeviceName());

	//inputSoundSlider
	QSlider * inputSoundSlider = Object::findChild<QSlider *>(_configPanelWidget, "inputSoundSlider");
	inputSoundSlider->setValue(_soundMixer->getInputVolume());
	connect(inputSoundSlider, SIGNAL(valueChanged(int)), SLOT(inputSoundSliderValueChanged(int)));

	//outputSoundSlider
	QSlider * outputSoundSlider = Object::findChild<QSlider *>(_configPanelWidget, "outputSoundSlider");
	outputSoundSlider->setValue(_soundMixer->getOutputVolume());
	connect(outputSoundSlider, SIGNAL(valueChanged(int)), SLOT(outputSoundSliderValueChanged(int)));

	//enableVideoCheckBox
	QCheckBox * enableVideoCheckBox = Object::findChild<QCheckBox *>(_configPanelWidget, "enableVideoCheckBox");
	connect(enableVideoCheckBox, SIGNAL(toggled(bool)), SLOT(enableVideoCheckBoxToggled(bool)));

	//enableWenboxCheckBox
	QCheckBox * enableWenboxCheckBox = Object::findChild<QCheckBox *>(_configPanelWidget, "enableWenboxCheckBox");
	connect(enableWenboxCheckBox, SIGNAL(toggled(bool)), SLOT(enableWenboxCheckBoxToggled(bool)));

	//videoSettingsLabel
	QLabel * videoSettingsLabel = Object::findChild<QLabel *>(_configPanelWidget, "videoSettingsLabel");

	//audioSettingsLabel
	QLabel * audioSettingsLabel = Object::findChild<QLabel *>(_configPanelWidget, "audioSettingsLabel");
}

void QtConfigPanel::inputSoundSliderValueChanged(int value) {
	_soundMixer->setInputVolume(value);
}

void QtConfigPanel::outputSoundSliderValueChanged(int value) {
	_soundMixer->setOutputVolume(value);
}

void QtConfigPanel::enableVideoCheckBoxToggled(bool checked) {
}

void QtConfigPanel::enableWenboxCheckBoxToggled(bool checked) {
}
