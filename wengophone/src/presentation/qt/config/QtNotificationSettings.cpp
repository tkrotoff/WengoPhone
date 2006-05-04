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

#include <QtGui>

QtNotificationSettings::QtNotificationSettings(QWidget * parent)
	: QtISettings(parent) {

	_notificationSettingsWidget = new QWidget(parent);

	_ui = new Ui::NotificationSettings();
	_ui->setupUi(_notificationSettingsWidget);

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
	_ui->showToasterOnContactOnlineCheckBox->setChecked(config.getNotificationShowToasterOnContactOnline());

	_ui->doNotDisturbNoWindowCheckBox->setChecked(config.getNotificationDoNotDisturbNoWindow());
	_ui->doNotDisturbNoAudioCheckBox->setChecked(config.getNotificationDoNotDisturbNoAudio());
	_ui->awayNoWindowCheckBox->setChecked(config.getNotificationAwayNoWindow());
	_ui->awayNoAudioCheckBox->setChecked(config.getNotificationAwayNoAudio());
}

void QtNotificationSettings::saveConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set(Config::NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY, _ui->showToasterOnIncomingCallCheckBox->isChecked());
	config.set(Config::NOTIFICATION_SHOW_TOASTER_ON_CONTACT_ONLINE_KEY, _ui->showToasterOnContactOnlineCheckBox->isChecked());

	config.set(Config::NOTIFICATION_DONOTDISTURB_NO_WINDOW_KEY, _ui->doNotDisturbNoWindowCheckBox->isChecked());
	config.set(Config::NOTIFICATION_DONOTDISTURB_NO_AUDIO_KEY, _ui->doNotDisturbNoAudioCheckBox->isChecked());
	config.set(Config::NOTIFICATION_AWAY_NO_WINDOW_KEY, _ui->awayNoWindowCheckBox->isChecked());
	config.set(Config::NOTIFICATION_AWAY_NO_AUDIO_KEY, _ui->awayNoAudioCheckBox->isChecked());
}
