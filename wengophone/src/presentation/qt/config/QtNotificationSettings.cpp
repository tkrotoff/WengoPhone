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

#include "QtNotificationSettings.h"

#include "ui_NotificationSettings.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <sound/Sound.h>

#include <util/Logger.h>

#include <QtGui/QtGui>

QtNotificationSettings::QtNotificationSettings(QWidget * parent)
	: QObject(parent) {

	_notificationSettingsWidget = new QWidget(NULL);

	_ui = new Ui::NotificationSettings();
	_ui->setupUi(_notificationSettingsWidget);

	connect(_ui->soundComboBox, SIGNAL(activated(const QString &)), SLOT(soundComboBoxActivated(const QString &)));
	connect(_ui->browseButton, SIGNAL(clicked()), SLOT(browseSounds()));
	connect(_ui->playButton, SIGNAL(clicked()), SLOT(playSound()));
	connect(_ui->clearSoundButton, SIGNAL(clicked()), SLOT(clearSound()));

	readConfig();
}

QtNotificationSettings::~QtNotificationSettings() {
	delete _ui;
}

QString QtNotificationSettings::getName() const {
	return tr("Notifications & Sounds");
}

void QtNotificationSettings::readConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_ui->showToasterOnIncomingCallCheckBox->setChecked(config.getNotificationShowToasterOnIncomingCall());
	_ui->showToasterOnIncomingChatCheckBox->setChecked(config.getNotificationShowToasterOnIncomingChat());
	_ui->showToasterOnContactOnlineCheckBox->setChecked(config.getNotificationShowToasterOnContactOnline());

	_ui->doNotDisturbNoWindowCheckBox->setChecked(config.getNotificationDoNotDisturbNoWindow());
	_ui->doNotDisturbNoAudioCheckBox->setChecked(config.getNotificationDoNotDisturbNoAudio());
	_ui->awayNoWindowCheckBox->setChecked(config.getNotificationAwayNoWindow());
	_ui->awayNoAudioCheckBox->setChecked(config.getNotificationAwayNoAudio());

	_incomingCallSoundFile = config.getAudioIncomingCallFile();
	_callClosedSoundFile = config.getAudioCallClosedFile();
	_incomingChatSoundFile = config.getAudioIncomingChatFile();
	_imAccountConnectedSoundFile = config.getAudioIMAccountConnectedFile();
	_imAccountDisconnectedSoundFile = config.getAudioIMAccountDisconnectedFile();
	_contactOnlineSoundFile = config.getAudioContactOnlineFile();

	soundComboBoxActivated(_ui->soundComboBox->currentText());
}

void QtNotificationSettings::saveConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set(Config::NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY, _ui->showToasterOnIncomingCallCheckBox->isChecked());
	config.set(Config::NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CHAT_KEY, _ui->showToasterOnIncomingChatCheckBox->isChecked());
	config.set(Config::NOTIFICATION_SHOW_TOASTER_ON_CONTACT_ONLINE_KEY, _ui->showToasterOnContactOnlineCheckBox->isChecked());

	config.set(Config::NOTIFICATION_DONOTDISTURB_NO_WINDOW_KEY, _ui->doNotDisturbNoWindowCheckBox->isChecked());
	config.set(Config::NOTIFICATION_DONOTDISTURB_NO_AUDIO_KEY, _ui->doNotDisturbNoAudioCheckBox->isChecked());
	config.set(Config::NOTIFICATION_AWAY_NO_WINDOW_KEY, _ui->awayNoWindowCheckBox->isChecked());
	config.set(Config::NOTIFICATION_AWAY_NO_AUDIO_KEY, _ui->awayNoAudioCheckBox->isChecked());

	config.set(Config::AUDIO_INCOMINGCALL_FILE_KEY, _incomingCallSoundFile);
	config.set(Config::AUDIO_CALLCLOSED_FILE_KEY, _callClosedSoundFile);
	config.set(Config::AUDIO_INCOMINGCHAT_FILE_KEY, _incomingChatSoundFile);
	config.set(Config::AUDIO_IMACCOUNTCONNECTED_FILE_KEY, _imAccountConnectedSoundFile);
	config.set(Config::AUDIO_IMACCOUNTDISCONNECTED_FILE_KEY, _imAccountDisconnectedSoundFile);
	config.set(Config::AUDIO_CONTACTONLINE_FILE_KEY, _contactOnlineSoundFile);
}

void QtNotificationSettings::soundComboBoxActivated(const QString & text) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	std::string soundFile;
	if (text == tr("Incoming call (ringtone)")) {
		soundFile = _incomingCallSoundFile;
	}
	else if (text == tr("Call closed (hang up tone)")) {
		soundFile = _callClosedSoundFile;
	}
	else if (text == tr("Incoming chat")) {
		soundFile = _incomingChatSoundFile;
	}
	else if (text == tr("IM account connected")) {
		soundFile = _imAccountConnectedSoundFile;
	}
	else if (text == tr("IM account disconnected")) {
		soundFile = _imAccountDisconnectedSoundFile;
	}
	else if (text == tr("Contact online")) {
		soundFile = _contactOnlineSoundFile;
	}
	else {
		LOG_FATAL("unknown text=" + text.toStdString());
	}

	_ui->soundLineEdit->setText(QString::fromStdString(soundFile));
}

void QtNotificationSettings::browseSounds() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	QString soundFile = QFileDialog::getOpenFileName(_notificationSettingsWidget,
				tr("Choose a sound file"),
				QString::fromStdString(config.getResourcesDir()) + "sounds",
				tr("Sounds") + "(*.wav)");

	if (!soundFile.isNull()) {
		_ui->soundLineEdit->setText(soundFile);
		setSounds(soundFile);
	}
}

void QtNotificationSettings::playSound() {
	std::string soundFile = _ui->soundLineEdit->text().toStdString();
	Sound::play(soundFile);
}

void QtNotificationSettings::clearSound() {
	setSounds(QString::null);
}

void QtNotificationSettings::setSounds(const QString & soundFile) {
	_ui->soundLineEdit->setText(soundFile);

	QString text = _ui->soundComboBox->currentText();

	if (text == tr("Incoming call (ringtone)")) {
		_incomingCallSoundFile = soundFile.toStdString();
	}
	else if (text == tr("Call closed (hang up tone)")) {
		_callClosedSoundFile = soundFile.toStdString();
	}
	else if (text == tr("Incoming chat")) {
		_incomingChatSoundFile = soundFile.toStdString();
	}
	else if (text == tr("IM account connected")) {
		_imAccountConnectedSoundFile = soundFile.toStdString();
	}
	else if (text == tr("IM account disconnected")) {
		_imAccountDisconnectedSoundFile = soundFile.toStdString();
	}
	else if (text == tr("Contact online")) {
		_contactOnlineSoundFile = soundFile.toStdString();
	}
	else {
		LOG_FATAL("unknown text=" + text.toStdString());
	}

	soundComboBoxActivated(text);
}
