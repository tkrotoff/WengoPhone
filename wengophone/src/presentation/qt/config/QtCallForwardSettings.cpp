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

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include "QtCallForwardSettings.h"

QtCallForwardSettings::QtCallForwardSettings( QWidget * parent, Qt::WFlags f ) : QWidget ( parent, f ) {

	_widget = WidgetFactory::create( ":/forms/config/CallForwardSettings.ui", this );
	QGridLayout * layout = new QGridLayout();
	layout->addWidget( _widget );
	layout->setMargin( 0 );
	setLayout( layout );
	setupChilds();
	readConfigData();
}

void QtCallForwardSettings::setupChilds() {

	_forwardCallCheckBox = Object::findChild<QCheckBox *>(_widget,"forwardCallCheckBox" );

	_forwardAllCallCheckBox = Object::findChild<QCheckBox *>( _widget,"forwardAllCallCheckBox" );

	_phoneNumber1Edit = Object::findChild<QLineEdit *>( _widget,"phoneNumber1Edit" );

	_phoneNumber2Edit = Object::findChild<QLineEdit *>( _widget,"phoneNumber2Edit" );

	_phoneNumber3Edit = Object::findChild<QLineEdit *>( _widget,"phoneNumber3Edit" );

	_forwardCallMobilCheckBox = Object::findChild<QCheckBox *>(_widget, "forwardCallMobilCheckBox" );
/*
	connect ( _forwardCallCheckBox, SIGNAL( stateChanged( int ) ), this,
			  SLOT( forwardCallStateChanged( int ) ) );

	connect ( _forwardAllCallCheckBox, SIGNAL( stateChanged( int ) ), this,
			  SLOT( forwardAllCallToStateChanged( int ) ) );

			  */

}

void QtCallForwardSettings::forwardCallStateChanged( int state ) {

	if ( state == Qt::Checked ) {
			_forwardAllCallCheckBox->setCheckState( Qt::Unchecked );
		}
}

void QtCallForwardSettings::forwardAllCallToStateChanged( int state ) {
	if ( state == Qt::Checked ) {
			_forwardCallCheckBox->setCheckState( Qt::Unchecked );
		}
}

void QtCallForwardSettings::saveData(){

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if ( _forwardCallCheckBox->checkState() == Qt::Checked )
	{
		config.set(config.CALL_FORWARD_ALL_UNDELIVREDTOVM_KEY, true );
		qDebug() << "_forwardCallCheckBox is checked, save it !";
	}
	else
		config.set(config.CALL_FORWARD_ALL_UNDELIVREDTOVM_KEY, false );

	if ( _forwardAllCallCheckBox->checkState() == Qt::Checked )
		config.set(config.CALL_FORWARD_ALL_UNDELIVREDTO_KEY,true);
	else
		config.set(config.CALL_FORWARD_ALL_UNDELIVREDTO_KEY,false);

	if ( _forwardCallMobilCheckBox->checkState() == Qt::Checked )
		config.set(config.CALL_FORWARD_TOMOBILE_KEY,true);
	else
		config.set(config.CALL_FORWARD_TOMOBILE_KEY,false);

	config.set(config.CALL_FORWARD_PHONENUMBER1_KEY,_phoneNumber1Edit->text().toStdString());
	config.set(config.CALL_FORWARD_PHONENUMBER2_KEY,_phoneNumber2Edit->text().toStdString());
	config.set(config.CALL_FORWARD_PHONENUMBER3_KEY,_phoneNumber3Edit->text().toStdString());

}

void QtCallForwardSettings::readConfigData(){

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if ( config.getCallForwardAllUndelivredToVm() )
		_forwardCallCheckBox->setCheckState(Qt::Checked);
	else
		_forwardCallCheckBox->setCheckState(Qt::Unchecked);

	if ( config.getCallForwardAllUndelivredTo() )
		_forwardAllCallCheckBox->setCheckState(Qt::Checked);
	else
		_forwardAllCallCheckBox->setCheckState(Qt::Unchecked);

	if ( config.getCallForwardToMobile() )
		_forwardCallMobilCheckBox->setCheckState(Qt::Checked);
	else
		_forwardCallMobilCheckBox->setCheckState(Qt::Unchecked);

	_phoneNumber1Edit->setText( QString::fromStdString(config.getCallForwardPhoneNumber1()) );
	_phoneNumber2Edit->setText( QString::fromStdString(config.getCallForwardPhoneNumber2()) );
	_phoneNumber3Edit->setText( QString::fromStdString(config.getCallForwardPhoneNumber3()) );

}
