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

#include "QtGeneralSettings.h"

#include "ui_GeneralSettings.h"

#include <presentation/qt/QtToolBar.h>
#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/profile/QtProfileDetails.h>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <model/config/Config.h>
#include <model/config/ConfigManager.h>

#include <cutil/global.h>
#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

QtGeneralSettings::QtGeneralSettings(CWengoPhone & cWengoPhone, QWidget * parent)
	: QObject(parent) {

	_generalSettingsWidget = new QWidget(NULL);

	_ui = new Ui::GeneralSettings();
	_ui->setupUi(_generalSettingsWidget);

#ifdef OS_LINUX
	_ui->autoStartGroupBox->hide();
#endif

	if (cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		QtWengoPhone *qtWengoPhone = dynamic_cast<QtWengoPhone *>(cWengoPhone.getPresentation());
		QtToolBar *toolbar = &qtWengoPhone->getQtToolBar();
		SAFE_CONNECT_RECEIVER(_ui->editProfileButton, SIGNAL(clicked()), 
			toolbar, SLOT(editMyProfile()));
	} else {
		_ui->editProfileButton->setDisabled(true);
	}

	readConfig();
}

QtGeneralSettings::~QtGeneralSettings() {
	delete _ui;
}

QString QtGeneralSettings::getName() const {
	return tr("General");
}

void QtGeneralSettings::readConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_ui->startWengoPhoneCheckBox->setChecked(config.getGeneralAutoStart());
	_ui->startFreeCallRadioButton->setChecked(config.getGeneralClickStartFreeCall());
	_ui->callCellPhoneCheckBox->setChecked(config.getGeneralClickCallCellPhone());
	_ui->startChatOnlyRadioButton->setChecked(config.getGeneralClickStartChat());

	int awayTimer = config.getGeneralAwayTimer();
	if (awayTimer != Config::NO_AWAY_TIMER) {
		_ui->showAwayCheckBox->setChecked(true);
		_ui->awaySpinBox->setValue(awayTimer);
	} else {
		_ui->showAwayCheckBox->setChecked(false);
	}
}

void QtGeneralSettings::saveConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set(Config::GENERAL_AUTOSTART_KEY, _ui->startWengoPhoneCheckBox->isChecked());
	config.set(Config::GENERAL_CLICK_START_FREECALL_KEY, _ui->startFreeCallRadioButton->isChecked());
	config.set(Config::GENERAL_CLICK_CALL_CELLPHONE_KEY, _ui->callCellPhoneCheckBox->isChecked());
	config.set(Config::GENERAL_CLICK_START_CHAT_KEY, _ui->startChatOnlyRadioButton->isChecked());

	int awayTimer = _ui->awaySpinBox->value();
	if (_ui->showAwayCheckBox->isChecked()) {
		config.set(Config::GENERAL_AWAY_TIMER_KEY, awayTimer);
	} else {
		config.set(Config::GENERAL_AWAY_TIMER_KEY, Config::NO_AWAY_TIMER);
	}
}
