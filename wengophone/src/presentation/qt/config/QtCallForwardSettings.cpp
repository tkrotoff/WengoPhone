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

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>
#include <control/webservices/callforward/CWsCallForward.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/Logger.h>

#include <QtGui>

QtCallForwardSettings::QtCallForwardSettings(CWengoPhone & cWengoPhone, QWidget * parent)
	: QObject(parent),
	_cWengoPhone(cWengoPhone) {

	_callForwardSettingsWidget = new QWidget(NULL);

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
	bool mustCallTheWs = false;
	std::string mode = "";
	std::string number1 = _ui->phoneNumber1Edit->text().toStdString();
	std::string number2 = _ui->phoneNumber2Edit->text().toStdString();
	std::string number3 = _ui->phoneNumber3Edit->text().toStdString();

	if(_ui->forwardToVoiceMailRadioButton->isChecked()) {
		mode = "voicemail";
	} else if( _ui->disableRadioButton->isChecked() ) {
		mode = "disable";
	} else if( _ui->forwardToNumberRadioButton->isChecked() ) {
		mode = "number";
	}

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//if the mode has changed
	if (config.getCallForwardMode() != mode) {
		config.set(Config::CALL_FORWARD_MODE_KEY, mode);
		mustCallTheWs = true;
	} else {
		//if numbers have changed
		if ((mode == "number") &&
			(config.getCallForwardPhoneNumber1() != number1 ) ||
			(config.getCallForwardPhoneNumber2() != number2 ) ||
			(config.getCallForwardPhoneNumber3() != number3)) {
			config.set(Config::CALL_FORWARD_PHONENUMBER1_KEY, number1);
			config.set(Config::CALL_FORWARD_PHONENUMBER2_KEY, number2);
			config.set(Config::CALL_FORWARD_PHONENUMBER3_KEY, number3);
			mustCallTheWs = true;
		}
	}

	if (mustCallTheWs) {	
		if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
			if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCWsCallForward()) {
				if (mode == "voicemail") {
					_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCWsCallForward()->forwardToVoiceMail();
				} else if (mode == "disable") {
					_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCWsCallForward()->disableCallForward();
				} else if (mode == "number") {
					_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCWsCallForward()->forwardToNumber(number1, number2, number3);
				}
			}
		}
	}

	//unused for now
	config.set(Config::CALL_FORWARD_TOMOBILE_KEY, _ui->forwardCallMobilCheckBox->isChecked());
}

void QtCallForwardSettings::readConfig() {

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if( config.getCallForwardMode() == "voicemail") {
		_ui->forwardToVoiceMailRadioButton->setChecked(true);

	}  else if( config.getCallForwardMode() == "number" ) {
		_ui->forwardToNumberRadioButton->setChecked(true);

	} else if( config.getCallForwardMode() == "disable" ) {
		_ui->disableRadioButton->setChecked(true);

	} else if( config.getCallForwardMode() == "unauthorized") {
		_ui->groupBoxSettings->setEnabled(false);
		_ui->groupBoxCellPhone->setEnabled(false);
	}

	_ui->forwardToVoiceMailRadioButton->setEnabled(config.hasVoiceMail());
	_ui->phoneNumber1Edit->setText(QString::fromStdString(config.getCallForwardPhoneNumber1()));
	_ui->phoneNumber2Edit->setText(QString::fromStdString(config.getCallForwardPhoneNumber2()));
	_ui->phoneNumber3Edit->setText(QString::fromStdString(config.getCallForwardPhoneNumber3()));
	_ui->forwardCallMobilCheckBox->setChecked(config.getCallForwardToMobile());
}
