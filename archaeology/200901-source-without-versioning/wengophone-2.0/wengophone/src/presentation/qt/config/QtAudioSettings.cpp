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

#include <sound/AudioDeviceManager.h>

#include <util/SafeDelete.h>

#include <qtutil/StringListConvert.h>
#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

static const char * AUDIO_TEST_CALL = "333";

QtAudioSettings::QtAudioSettings(CWengoPhone & cWengoPhone, QWidget * parent)
	: QObject(parent),
	_cWengoPhone(cWengoPhone) {

	_audioSettingsWidget = new QWidget(NULL);

	_ui = new Ui::AudioSettings();
	_ui->setupUi(_audioSettingsWidget);

	SAFE_CONNECT(_ui->makeTestCallButton, SIGNAL(pressed()), SLOT(makeTestCallClicked()));

	readConfig();
}

QtAudioSettings::~QtAudioSettings() {
	OWSAFE_DELETE(_ui);
}

QString QtAudioSettings::getName() const {
	return tr("Audio");
}

void QtAudioSettings::saveConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//Retrieving the input data from the concatenated string
	std::string concatString = _ui->inputDeviceComboBox->itemData(_ui->inputDeviceComboBox->currentIndex()).toString().toStdString();
	std::list<AudioDevice> inputDeviceList = AudioDeviceManager::getInputDeviceList();
	for (std::list<AudioDevice>::const_iterator it = inputDeviceList.begin();
		it != inputDeviceList.end();
		++it) {
		if ((*it).getData().toString() == concatString) {
			config.set(Config::AUDIO_INPUT_DEVICEID_KEY, (*it).getData());
			break;
		}
	}
	////

	//Retrieving the output data from the concatenated string
	concatString = _ui->outputDeviceComboBox->itemData(_ui->outputDeviceComboBox->currentIndex()).toString().toStdString();
	std::list<AudioDevice> outputDeviceList = AudioDeviceManager::getOutputDeviceList();
	for (std::list<AudioDevice>::const_iterator it = outputDeviceList.begin();
		it != outputDeviceList.end();
		++it) {
		if ((*it).getData().toString() == concatString) {
			config.set(Config::AUDIO_OUTPUT_DEVICEID_KEY, (*it).getData());
			break;
		}
	}
	////

	//Retrieving the ringer data from the concatenated string
	concatString = _ui->ringingDeviceComboBox->itemData(_ui->ringingDeviceComboBox->currentIndex()).toString().toStdString();
	for (std::list<AudioDevice>::const_iterator it = outputDeviceList.begin();
		it != outputDeviceList.end();
		++it) {
		if ((*it).getData().toString() == concatString) {
			config.set(Config::AUDIO_RINGER_DEVICEID_KEY, (*it).getData());
			break;
		}
	}
	////
}

void QtAudioSettings::readConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//inputDeviceList
	_ui->inputDeviceComboBox->clear();

	std::list<AudioDevice> inputDeviceList = AudioDeviceManager::getInputDeviceList();
	for (std::list<AudioDevice>::const_iterator it = inputDeviceList.begin();
		it != inputDeviceList.end();
		++it) {
		_ui->inputDeviceComboBox->addItem(QString::fromUtf8((*it).getName().c_str()),
			QString::fromStdString((*it).getData().toString()));
	}

	QString currentInputDeviceId = QString::fromUtf8(config.getAudioInputDeviceId().toString().c_str());
	_ui->inputDeviceComboBox->setCurrentIndex(_ui->inputDeviceComboBox->findData(currentInputDeviceId));
	////

	//outputDeviceList
	_ui->outputDeviceComboBox->clear();

	std::list<AudioDevice> outputDeviceList = AudioDeviceManager::getOutputDeviceList();
	for (std::list<AudioDevice>::const_iterator it = outputDeviceList.begin();
		it != outputDeviceList.end();
		++it) {
		_ui->outputDeviceComboBox->addItem(QString::fromUtf8((*it).getName().c_str()),
			QString::fromStdString((*it).getData().toString()));
	}

	QString currentOutputDeviceId = QString::fromUtf8(config.getAudioOutputDeviceId().toString().c_str());
	_ui->outputDeviceComboBox->setCurrentIndex(_ui->outputDeviceComboBox->findData(currentOutputDeviceId));
	////

	//ringingDeviceList = outputDeviceList
	_ui->ringingDeviceComboBox->clear();

	for (std::list<AudioDevice>::const_iterator it = outputDeviceList.begin();
		it != outputDeviceList.end();
		++it) {
		_ui->ringingDeviceComboBox->addItem(QString::fromUtf8((*it).getName().c_str()),
			QString::fromStdString((*it).getData().toString()));
	}

	QString currentRingerDeviceId = QString::fromUtf8(config.getAudioRingerDeviceId().toString().c_str());
	_ui->ringingDeviceComboBox->setCurrentIndex(_ui->ringingDeviceComboBox->findData(currentRingerDeviceId));
	////
}

void QtAudioSettings::makeTestCallClicked() {
	CUserProfile * cUserProfile = _cWengoPhone.getCUserProfileHandler().getCUserProfile();
	if ((cUserProfile) && (cUserProfile->getUserProfile().getActivePhoneLine())) {
		cUserProfile->getUserProfile().getActivePhoneLine()->makeCall(AUDIO_TEST_CALL);
	}
}
