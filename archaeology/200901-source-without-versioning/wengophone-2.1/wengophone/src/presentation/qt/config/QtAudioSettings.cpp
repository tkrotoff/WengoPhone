/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include <presentation/qt/QtWengoPhone.h>

#include <sound/AudioDeviceManager.h>
#include <qtutil/StringListConvert.h>
#include <qtutil/SafeConnect.h>
#include <util/SafeDelete.h>

#include <QtGui/QtGui>

static const char * AUDIO_TEST_CALL = "333";

QtAudioSettings::QtAudioSettings(CWengoPhone & cWengoPhone, QWidget * parent)
	: QWidget(parent),
	_cWengoPhone(cWengoPhone) {

	_ui = new Ui::AudioSettings();
	_ui->setupUi(this);

	SAFE_CONNECT(_ui->makeTestCallButton, SIGNAL(pressed()), SLOT(makeTestCallClicked()));
	SAFE_CONNECT(_ui->AdvButton, SIGNAL(pressed()), SLOT(AdvPressed()));
	SAFE_CONNECT(_ui->UpButton, SIGNAL(pressed()), SLOT(UpPressed()));
	SAFE_CONNECT(_ui->DownButton, SIGNAL(pressed()), SLOT(DownPressed()));
	SAFE_CONNECT(_ui->ResetButton, SIGNAL(pressed()), SLOT(ResetPressed()));
	_ui->AdvGroupBox->setVisible(false);

	readConfig();
}

QtAudioSettings::~QtAudioSettings() {
	OWSAFE_DELETE(_ui);
}

QString QtAudioSettings::getName() const {
	return tr("Audio");
}

QString QtAudioSettings::getTitle() const {
	return tr("Audio Settings");
}

QString QtAudioSettings::getIconName() const {
	return "audio";
}

void QtAudioSettings::saveConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//Retrieving the input data from the concatenated string
	std::string concatString = _ui->inputDeviceComboBox->itemData(_ui->inputDeviceComboBox->currentIndex()).toString().toStdString();
	std::list<AudioDevice> inputDeviceList = AudioDeviceManager::getInstance().getInputDeviceList();
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
	std::list<AudioDevice> outputDeviceList = AudioDeviceManager::getInstance().getOutputDeviceList();
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
	
	// advanced
	StringList saveCodec;
	int nbCodec = _ui->audioCodecListWidget->count();
	for (int i = 0; i < nbCodec; ++i) {
		std::string audioCodec = _ui->audioCodecListWidget->item(i)->text().toStdString();
		// HACK: PHAPI_SPEEXWB_REPLACES_G726WB_HACK
		if (audioCodec == "SPEEX/16000") {
			audioCodec = "G726-64wb/16000";
		}
		////
		saveCodec += audioCodec;
	}
	config.set(Config::AUDIO_CODEC_LIST_KEY, saveCodec);
	////
}

void QtAudioSettings::readConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//inputDeviceList
	_ui->inputDeviceComboBox->clear();

#ifdef OS_LINUX
	bool savedSettingsFound = false;
	AudioDevice tmpDev(config.getAudioInputDeviceId());
#endif

	std::list<AudioDevice> inputDeviceList = AudioDeviceManager::getInstance().getInputDeviceList();
	for (std::list<AudioDevice>::const_iterator it = inputDeviceList.begin();
		it != inputDeviceList.end();
		++it) {
		_ui->inputDeviceComboBox->addItem(
			QString::fromUtf8((*it).getName().c_str()),
			QString::fromStdString((*it).getData().toString())
		);
#ifdef OS_LINUX
		if ((*it).getData() == tmpDev.getData()) {
			savedSettingsFound = true;
		}
#endif
	}

#ifdef OS_LINUX
	if (!savedSettingsFound) {
		_ui->outputDeviceComboBox->addItem(
			QString::fromUtf8(tmpDev.getName().c_str()),
			QString::fromStdString(tmpDev.getData().toString())
		);
	}
#endif
	QString currentInputDeviceId = 
		QString::fromUtf8(config.getAudioInputDeviceId().toString().c_str());
	_ui->inputDeviceComboBox->setCurrentIndex(
		_ui->inputDeviceComboBox->findData(currentInputDeviceId)
	);
	////

	//outputDeviceList
	_ui->outputDeviceComboBox->clear();

#ifdef OS_LINUX
	savedSettingsFound = false;
	tmpDev = AudioDevice(config.getAudioOutputDeviceId());
#endif
	std::list<AudioDevice> outputDeviceList = AudioDeviceManager::getInstance().getOutputDeviceList();
	for (std::list<AudioDevice>::const_iterator it = outputDeviceList.begin();
		it != outputDeviceList.end();
		++it) {
		_ui->outputDeviceComboBox->addItem(
			QString::fromUtf8((*it).getName().c_str()),
			QString::fromStdString((*it).getData().toString())
		);
#ifdef OS_LINUX
		if ((*it).getData() == tmpDev.getData()) {
			savedSettingsFound = true;
		}
#endif
	}

#ifdef OS_LINUX
	if (!savedSettingsFound) {
		_ui->outputDeviceComboBox->addItem(
			QString::fromUtf8(tmpDev.getName().c_str()),
			QString::fromStdString(tmpDev.getData().toString())
		);
	}
#endif
	QString currentOutputDeviceId = 
		QString::fromUtf8(config.getAudioOutputDeviceId().toString().c_str());
	_ui->outputDeviceComboBox->setCurrentIndex(
		_ui->outputDeviceComboBox->findData(currentOutputDeviceId)
	);
	////

	//ringingDeviceList = outputDeviceList
	_ui->ringingDeviceComboBox->clear();
#ifdef OS_LINUX
	savedSettingsFound = false;
	tmpDev = AudioDevice(config.getAudioOutputDeviceId());
#endif
	for (std::list<AudioDevice>::const_iterator it = outputDeviceList.begin();
		it != outputDeviceList.end();
		++it) {
		_ui->ringingDeviceComboBox->addItem(
			QString::fromUtf8((*it).getName().c_str()),
			QString::fromStdString((*it).getData().toString())
		);
#ifdef OS_LINUX
		if ((*it).getData() == tmpDev.getData()) {
			savedSettingsFound = true;
		}
#endif
	}

#ifdef OS_LINUX
	if (!savedSettingsFound) {
		_ui->ringingDeviceComboBox->addItem(
			QString::fromUtf8(tmpDev.getName().c_str()),
			QString::fromStdString(tmpDev.getData().toString())
		);
	}
#endif
	QString currentRingerDeviceId = 
		QString::fromUtf8(config.getAudioRingerDeviceId().toString().c_str());
	_ui->ringingDeviceComboBox->setCurrentIndex(
		_ui->ringingDeviceComboBox->findData(currentRingerDeviceId)
	);
	////
	
	// advanced
	fillAudioCodecList();
}

void QtAudioSettings::makeTestCallClicked() {
	CUserProfile * cUserProfile = _cWengoPhone.getCUserProfileHandler().getCUserProfile();
	if ((cUserProfile) && (cUserProfile->getUserProfile().getActivePhoneLine())) {
		cUserProfile->getUserProfile().getActivePhoneLine()->makeCall(AUDIO_TEST_CALL);
		QtWengoPhone* qWengoPhone = static_cast<QtWengoPhone*>(_cWengoPhone.getPresentation());
		qWengoPhone->getWidget()->raise();
	}
}

void QtAudioSettings::AdvPressed() {
	if (_ui->AdvGroupBox->isVisible()) {
		_ui->AdvGroupBox->setVisible(false);
		_ui->AdvButton->setText(tr("Advanced") + " >>");
	} else {
		_ui->AdvGroupBox->setVisible(true);
		_ui->AdvButton->setText(tr("Advanced") + " <<");
	}
}

void QtAudioSettings::UpPressed() {

	//extracts current Item
	int currentrow = _ui->audioCodecListWidget->currentRow();

	//if an item is selected
	if (currentrow < 0) {
		return;
	}

	QListWidgetItem * mover = _ui->audioCodecListWidget->takeItem(currentrow);

	//inserts it to its new place
	 _ui->audioCodecListWidget->insertItem(currentrow - 1,mover);

	//sets focus on mover
	_ui->audioCodecListWidget->setCurrentItem(mover);
}

void QtAudioSettings::DownPressed() {

	//extracts current Item
	int currentrow = _ui->audioCodecListWidget->currentRow();

	//if an item is selected
	if (currentrow < 0) {
		return;
	}

	QListWidgetItem * mover = _ui->audioCodecListWidget->takeItem(currentrow);

	//inserts it to its new place
	 _ui->audioCodecListWidget->insertItem(currentrow + 1,mover);

	//sets focus on mover
	_ui->audioCodecListWidget->setCurrentItem(mover);
}

void QtAudioSettings::ResetPressed() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//resets saved audio codec list
	config.resetToDefaultValue(Config::AUDIO_CODEC_LIST_KEY);

	fillAudioCodecList();
}

void QtAudioSettings::fillAudioCodecList() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//resets displayed audio codec list
	StringList audioCodecList = config.getAudioCodecList();
	_ui->audioCodecListWidget->clear();
	for (StringList::const_iterator it = audioCodecList.begin(); it != audioCodecList.end(); ++it) {
		QString audioCodec = QString(it->c_str());
		// HACK: PHAPI_SPEEXWB_REPLACES_G726WB_HACK
		if (audioCodec == "G726-64wb/16000") {
			audioCodec = "SPEEX/16000";
		}
		////
		_ui->audioCodecListWidget->addItem(audioCodec);
	}
	////
}
