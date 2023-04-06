/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "AudioWidget.h"

#include "MyWidgetFactory.h"
#include "ConfigWindow.h"
#include "Audio.h"
#include "Softphone.h"
#include "StatusBar.h"
#include "MainWindow.h"

#include <AudioDevice.h>
#include <SoundMixer.h>
#include <StringList.h>

#include <qwidget.h>
#include <qslider.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <qcheckbox.h>

#include <cassert>
#include <iostream>
using namespace std;

AudioWidget::AudioWidget(ConfigWindow & configWindow, MainWindow & mainWindow)
: QObject(configWindow.getWidget()), _configWindow(configWindow), _audio(Audio::getInstance()), _mainWindow(mainWindow) {

	QWidget * parent = _configWindow.getWidget();

	_audio.addObserver(*this);

	_audioWidget = MyWidgetFactory::create("AudioWidgetForm.ui", this, parent);

	_inputComboBox = (QComboBox *) _audioWidget->child("inputComboBox");
	_inputComboBox->setDuplicatesEnabled(false);
	_outputComboBox = (QComboBox *) _audioWidget->child("outputComboBox");
	_outputComboBox->setDuplicatesEnabled(false);
	_ringingComboBox = (QComboBox *) _audioWidget->child("ringingComboBox");
	_ringingComboBox->setDuplicatesEnabled(false);

	_muteMicPlayBackCheckBox = (QCheckBox *) _mainWindow.getWidget()->child("mutePlayBackCheckBox", "QCheckBox");
#ifndef WIN32
	connect(_muteMicPlayBackCheckBox, SIGNAL(toggled ( bool )),
			this, SLOT(setMicPlayBack( bool )));
	//hide the wenbox checkbox
	QCheckBox * wenboxCheckBox = (QCheckBox *) _mainWindow.getWidget()->child("WenboxCheckBox", "QCheckBox");
	wenboxCheckBox->hide();
#else
	_muteMicPlayBackCheckBox->hide();
#endif	
	_audioConfigRadioButton = (QRadioButton *) _audioWidget->child("audioConfigRadioButton");

	_defaultButton = (QPushButton *) _audioWidget->child("defaultButton");
	connect(_defaultButton, SIGNAL(clicked()),
		this, SLOT(defaultButtonClicked()));

	//Update widget
	initComboBoxes();

	updateDevices();

	_soundMixer = NULL;
	updateSoundSliders();
}

AudioWidget::~AudioWidget() {
	delete _soundMixer;
}

void AudioWidget::defaultButtonClicked() {
	if (_audioConfigRadioButton->isChecked()) {
		_audio.defaultSettings();
	}
}

void AudioWidget::update(const Subject &, const Event &) {
	updateDevices();

	//Change the audio configuration without saving in a XML file
	//_configWindow.apply();
	updateSoundSliders();
}

void AudioWidget::save() {
	if ((_inputComboBox->currentText() != _audio.getInputDeviceName()
		|| _outputComboBox->currentText() != _audio.getOutputDeviceName()
		|| _ringingComboBox->currentText() != _audio.getRingingDeviceName())
		&& _audioConfigRadioButton->isChecked()) {

			_audio.changeSettings(_inputComboBox->currentText(),
				_outputComboBox->currentText(),
				_ringingComboBox->currentText());

			//Saves the audio settings in order to restore them later on
			_audio.storeSettings();

		apply();
	}
}

void AudioWidget::apply() {
}

void AudioWidget::initComboBoxes() {
	fillComboBox(_inputComboBox, AudioDevice::getInputMixerDeviceList());
	fillComboBox(_outputComboBox, AudioDevice::getOutputMixerDeviceList());
	fillComboBox(_ringingComboBox, AudioDevice::getOutputMixerDeviceList());
}

void AudioWidget::fillComboBox(QComboBox * comboBox, const StringList & listAudioDevices) {
	QStringList tmp;
	for (StringList::const_iterator it = listAudioDevices.begin();
		it != listAudioDevices.end(); ++it) {

		tmp += *it;
	}

	comboBox->clear();
	comboBox->insertStringList(tmp);
}

void AudioWidget::updateDevices() {
	if (!_audio.getInputDeviceName().isNull()) {
		_inputComboBox->setCurrentText(_audio.getInputDeviceName());
	} else {
		_inputComboBox->setCurrentItem(0);
	}
	if (!_audio.getOutputDeviceName().isNull()) {
		_outputComboBox->setCurrentText(_audio.getOutputDeviceName());
	} else {
		_outputComboBox->setCurrentItem(0);
	}
	if (!_audio.getRingingDeviceName().isNull()) {
		_ringingComboBox->setCurrentText(_audio.getRingingDeviceName());
	} else {
		_ringingComboBox->setCurrentItem(0);
	}
}

void AudioWidget::updateSoundSliders() {
	//Volume sliders
	QSlider * inputSoundSlider = _mainWindow.getInputSoundSlider();
	QSlider * outputSoundSlider = _mainWindow.getOutputSoundSlider();
	disconnect(inputSoundSlider, 0, 0, 0);
	disconnect(outputSoundSlider, 0, 0, 0);

	//Deletes the previous soundMixer
	delete _soundMixer;
	try {
		_soundMixer = new SoundMixer(_audio.getInputDeviceName(), _audio.getOutputDeviceName());
	} catch (const NoSoundCardException & e) {
		QMessageBox::information(Softphone::getInstance().getActiveWindow(),
					tr("Sound card error"),
					e.what());
		cerr << "AudioWidget: Sound card error: " << e.what() << endl;
		_mainWindow.getStatusBar()->setAudioStatus(false);
		outputSoundSlider->setEnabled(false);
		inputSoundSlider->setEnabled(false);
		return;
	} catch (const SoundMixerException & e) {
		QMessageBox::information(Softphone::getInstance().getActiveWindow(),
				tr("Sound mixer error"),
				tr("An error occured in the sound mixer system: ") + e.what());
		//_mainWindow.getStatusBar()->setAudioStatus(false);
		cerr << "AudioWidget: An error occured in the sound mixer system: " << e.what() << endl;
		outputSoundSlider->setEnabled(false);
		inputSoundSlider->setEnabled(false);
		return;
	}

	outputSoundSlider->setValue(_soundMixer->getOutputVolume());
	connect(outputSoundSlider, SIGNAL(valueChanged(int)),
		this, SLOT(setOutputVolume(int)));

	inputSoundSlider->setValue(_soundMixer->getInputVolume());
	connect(inputSoundSlider, SIGNAL(valueChanged(int)),
		this, SLOT(setInputVolume(int)));

#ifndef WIN32
	if( _soundMixer->isPlaybackMuted() ) {
		_muteMicPlayBackCheckBox->setChecked(true);
	}
#endif
}

void AudioWidget::setInputVolume(int volume) {
	_soundMixer->setInputVolume(volume);
}

void AudioWidget::setOutputVolume(int volume) {
	_soundMixer->setOutputVolume(volume);
}

void AudioWidget::setMicPlayBack(bool mute) {
#ifndef WIN32
	cout << "Mute playback: " << mute << endl;
	_soundMixer->setMicPlayBack(mute);
#endif
}

