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

#include <presentation/qt/config/QtWengoConfigDialog.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <sound/AudioDeviceManager.h>
#include <sound/VolumeControl.h>

#include <util/Logger.h>

#include <qtutil/PaintEventFilter.h>

#include <QtGui/QtGui>

QtConfigPanel::QtConfigPanel(CWengoPhone & cWengoPhone, QWidget * parent)
	: QObjectThreadSafe(parent),
	_cWengoPhone(cWengoPhone) {

	_configPanelWidget = new QWidget(NULL);

	_ui = new Ui::WengoPhoneWindowConfigPanel();
	_ui->setupUi(_configPanelWidget);

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.valueChangedEvent += boost::bind(&QtConfigPanel::configChangedEventHandler, this, _1, _2);

	//inputSoundSlider
	connect(_ui->inputSoundSlider, SIGNAL(valueChanged(int)), SLOT(inputVolumeSliderValueChanged(int)));

	//outputSoundSlider
	connect(_ui->outputSoundSlider, SIGNAL(valueChanged(int)), SLOT(outputVolumeSliderValueChanged(int)));

	//enableWenboxCheckBox
	connect(_ui->enableWenboxCheckBox, SIGNAL(toggled(bool)), SLOT(enableWenboxCheckBoxToggled(bool)));

	//halfDuplexCheckBox
	connect(_ui->halfDuplexCheckBox, SIGNAL(toggled(bool)), SLOT(halfDuplexCheckBoxToggled(bool)));

	configChangedEventHandler(config, Config::AUDIO_INPUT_DEVICEID_KEY);
	configChangedEventHandler(config, Config::AUDIO_OUTPUT_DEVICEID_KEY);
	configChangedEventHandler(config, Config::WENBOX_ENABLE_KEY);
	configChangedEventHandler(config, Config::AUDIO_HALFDUPLEX_KEY);

	//videoSettingsButton
	connect(_ui->videoSettingsButton, SIGNAL(clicked()), SLOT(videoSettingsClicked()));

	//audioSettingsButton
	connect(_ui->audioSettingsButton, SIGNAL(clicked()), SLOT(audioSettingsClicked()));

	PaintEventFilter * paintFilter = new PaintEventFilter(this, SLOT(paintEvent(QEvent *)));
	_configPanelWidget->installEventFilter(paintFilter);
}

QtConfigPanel::~QtConfigPanel() {
	delete _ui;
}

void QtConfigPanel::inputVolumeSliderValueChanged(int value) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	VolumeControl volumeControl(AudioDevice(config.getAudioInputDeviceId()));
	volumeControl.setLevel(value);
}

void QtConfigPanel::outputVolumeSliderValueChanged(int value) {
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
		AudioDevice audioDevice(config.getAudioInputDeviceId());
		VolumeControl inputVolumeControl(audioDevice);
		_ui->inputSoundSlider->setToolTip(tr("Input: ") + QString::fromStdString(audioDevice.getName()));
		_ui->inputSoundSlider->setValue(inputVolumeControl.getLevel());
	}

	if (key == Config::AUDIO_OUTPUT_DEVICEID_KEY) {
		//outputSoundSlider
		AudioDevice audioDevice(config.getAudioOutputDeviceId());
		VolumeControl outputVolumeControl(audioDevice);
		_ui->outputSoundSlider->setToolTip(tr("Output: ") + QString::fromStdString(audioDevice.getName()));
		_ui->outputSoundSlider->setValue(outputVolumeControl.getLevel());
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

void QtConfigPanel::paintEvent(QEvent * event) {
	QRect rect = _configPanelWidget->rect();

	QLinearGradient lg(QPointF(1, rect.top()), QPointF(1, rect.bottom()));

	lg.setColorAt(0, _configPanelWidget->palette().color(QPalette::Window));
	QColor dest = _configPanelWidget->palette().color(QPalette::Window);

	float red = ((float) dest.red()) / 1.3f;
	float blue = ((float) dest.blue()) / 1.3f;
	float green = ((float) dest.green()) / 1.3f;

	dest = QColor((int) red, (int) green, (int) blue);
	lg.setColorAt(1, dest);

	QPainter painter(_configPanelWidget);
	painter.fillRect(rect, QBrush(lg));
	painter.end();
}
