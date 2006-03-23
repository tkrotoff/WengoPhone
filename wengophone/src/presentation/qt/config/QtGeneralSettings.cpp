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
#include <qtutil/Object.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>


#include "QtGeneralSettings.h"



QtGeneralSettings::QtGeneralSettings( QWidget * parent, Qt::WFlags f ) : QWidget( parent, f ) {
	_widget = WidgetFactory::create( ":/forms/config/GeneralSettings.ui", this );
	QGridLayout * layout = new QGridLayout();
	layout->addWidget( _widget );
	layout->setMargin( 0 );
	setLayout( layout );

	_startWengoCheckBox = Object::findChild<QCheckBox *>(_widget,"startWengoCheckBox" );
	_checkForUpdateCheckBox = Object::findChild<QCheckBox *>(_widget,"checkForUpdateCheckBox" );
	_startFreeCallRadioButton = Object::findChild<QRadioButton *>(_widget,"startFreeCallRadioButton" );
	_startChatOnlyRadioButton = Object::findChild<QRadioButton *>(_widget,"startChatOnlyRadioButton" );
	_callCellPhoneCheckBox = Object::findChild<QCheckBox *>(_widget,"callCellPhoneCheckBox" );
	_showAwayCheckBox = Object::findChild<QCheckBox *>(_widget,"showAwayCheckBox" );
	_showNotAvailableCheckBox = Object::findChild<QCheckBox *>(_widget,"showNotAvailableCheckBox" );
	_awaySpinBox = Object::findChild<QSpinBox *>(_widget,"awaySpinBox" );
	_notAvailableSpinBox = Object::findChild<QSpinBox *>(_widget,"notAvailableSpinBox" );
	_editProfilePushButton = Object::findChild<QPushButton *>(_widget,"editProfilePushButton" );

	readConfigData();
}

void QtGeneralSettings::readConfigData(){

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if ( config.getGeneralSettingsAutoStartWengo() )
		_startWengoCheckBox->setCheckState(Qt::Checked);
	else
		_startWengoCheckBox->setCheckState(Qt::Unchecked);

	if ( config.getGeneralSettingsCheckUpdate() )
		_checkForUpdateCheckBox->setCheckState(Qt::Checked);
	else
		_checkForUpdateCheckBox->setCheckState(Qt::Unchecked);

	if ( config.getGeneralSettingsStartFreeCall() )
		_startFreeCallRadioButton->setChecked ( true );
	else
		_startFreeCallRadioButton->setChecked ( false );

	if ( config.getGeneralSettingsStartChatOnly() )
		_startChatOnlyRadioButton->setChecked ( true );
	else
		_startChatOnlyRadioButton->setChecked ( false );

	if ( config.getGeneralSettingsCallCellPhone() )
		_callCellPhoneCheckBox->setCheckState(Qt::Checked);
	else
		_callCellPhoneCheckBox->setCheckState(Qt::Unchecked);

	if ( config.getGeneralSettingsShowAway() )
		_showAwayCheckBox->setCheckState(Qt::Checked);
	else
		_showAwayCheckBox->setCheckState(Qt::Unchecked);

	if ( config.getGeneralSettingsShowNotAvailable() )
		_showNotAvailableCheckBox->setCheckState(Qt::Checked);
	else
		_showNotAvailableCheckBox->setCheckState(Qt::Unchecked);

	_notAvailableSpinBox->setValue(config.getGeneralSettingsNotAvailableTimer());

	_awaySpinBox->setValue(config.getGeneralSettingsGetAwayTimer());

}

void QtGeneralSettings::saveData(){

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if ( _startWengoCheckBox->checkState() == Qt::Checked )
		config.set(config.GENERAL_SETTINGS_AUTOSTART_WENGO_KEY, true);
	else
		config.set(config.GENERAL_SETTINGS_AUTOSTART_WENGO_KEY, false);

	if ( _startFreeCallRadioButton->isChecked() )
		config.set(config.GENERAL_SETTINGS_START_FREECALL_KEY,true);
	else
		config.set(config.GENERAL_SETTINGS_START_FREECALL_KEY,false);

	if ( _checkForUpdateCheckBox->checkState() == Qt::Checked )
		config.set(config.GENERAL_SETTINGS_CHECK_UPDATE_KEY,true);
	else
		config.set(config.GENERAL_SETTINGS_CHECK_UPDATE_KEY,false);

	if ( _startChatOnlyRadioButton->isChecked() )
		config.set(config.GENERAL_SETTINGS_START_CHATONLY_KEY,true);
	else
		config.set(config.GENERAL_SETTINGS_START_CHATONLY_KEY,false);

	if ( _callCellPhoneCheckBox->checkState() == Qt::Checked )
		config.set(config.GENERAL_SETTINGS_CALL_CELLPHONE_KEY,true);
	else
		config.set(config.GENERAL_SETTINGS_CALL_CELLPHONE_KEY,false);

	if ( _showAwayCheckBox->checkState() == Qt::Checked )
		config.set(config.GENERAL_SETTINGS_SHOW_AWAY_KEY,true);
	else
		config.set(config.GENERAL_SETTINGS_SHOW_AWAY_KEY,false);

	if ( _showNotAvailableCheckBox->checkState() == Qt::Checked )
		config.set(config.GENERAL_SETTINGS_SHOW_NOTAVAILABLE_KEY,true);
	else
		config.set(config.GENERAL_SETTINGS_SHOW_NOTAVAILABLE_KEY,false);

	config.set(config.GENERAL_SETTINGS_AWAY_TIMER_KEY,_awaySpinBox->value());
	config.set(config.GENERAL_SETTINGS_NOTAVAILABLE_TIMER_KEY,_awaySpinBox->value());

}