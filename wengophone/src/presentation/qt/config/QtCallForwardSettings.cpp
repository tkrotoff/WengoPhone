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

#include "QtCallForwardSettings.h"

#include "ui_CallForwardSettings.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <QtGui>

QtCallForwardSettings::QtCallForwardSettings(QWidget * parent)
	: QtISettings(parent) {

	_callForwardSettingsWidget = new QWidget(parent);

	_ui = new Ui::CallForwardSettings();
	_ui->setupUi(_callForwardSettingsWidget);

	readConfig();
}

QtCallForwardSettings::~QtCallForwardSettings() {
	delete _ui;
}

QString QtCallForwardSettings::getName() const {
	return tr("Call Forward");
}

void QtCallForwardSettings::saveConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set(Config::CALL_FORWARD_ALL_UNDELIVREDTOVM_KEY, _ui->forwardToVoiceMailRadioButton->isChecked());
	config.set(Config::CALL_FORWARD_ALL_UNDELIVREDTO_KEY, _ui->forwardToNumberRadioButton->isChecked());
	config.set(Config::CALL_FORWARD_PHONENUMBER1_KEY, _ui->phoneNumber1Edit->text().toStdString());
	config.set(Config::CALL_FORWARD_PHONENUMBER2_KEY, _ui->phoneNumber2Edit->text().toStdString());
	config.set(Config::CALL_FORWARD_PHONENUMBER3_KEY, _ui->phoneNumber3Edit->text().toStdString());
	config.set(Config::CALL_FORWARD_TOMOBILE_KEY, _ui->forwardCallMobilCheckBox->isChecked());
}

void QtCallForwardSettings::readConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_ui->forwardToVoiceMailRadioButton->setChecked(config.getCallForwardAllUndelivredToVm());
	_ui->forwardToNumberRadioButton->setChecked(config.getCallForwardAllUndelivredTo());
	_ui->phoneNumber1Edit->setText(QString::fromStdString(config.getCallForwardPhoneNumber1()));
	_ui->phoneNumber2Edit->setText(QString::fromStdString(config.getCallForwardPhoneNumber2()));
	_ui->phoneNumber3Edit->setText(QString::fromStdString(config.getCallForwardPhoneNumber3()));
	_ui->forwardCallMobilCheckBox->setChecked(config.getCallForwardToMobile());

}
