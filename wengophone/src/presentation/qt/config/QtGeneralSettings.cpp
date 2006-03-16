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

#include "QtGeneralSettings.h"

#include <QDebug>


QtGeneralSettings::QtGeneralSettings( QWidget * parent, Qt::WFlags f ) : QWidget( parent, f ) {
	_widget = WidgetFactory::create( ":/forms/config/GeneralSettings.ui", this );
	QGridLayout * layout = new QGridLayout();
	layout->addWidget( _widget );
	layout->setMargin( 0 );
	setLayout( layout );

	_startWengoCheckBox = _widget->findChild<QCheckBox *>( "startWengoCheckBox" );
	_checkForUpdateCheckBox = _widget->findChild<QCheckBox *>( "checkForUpdateCheckBox" );
	_startFreeCallRadioButton = _widget->findChild<QRadioButton *>( "startFreeCallRadioButton" );
	_startChatOnlyRadioButton = _widget->findChild<QRadioButton *>( "startChatOnlyRadioButton" );
	_callCellPhoneCheckBox = _widget->findChild<QCheckBox *>( "callCellPhoneCheckBox" );
	_showAwayCheckBox = _widget->findChild<QCheckBox *>( "showAwayCheckBox" );
	_showNotAvailableCheckBox = _widget->findChild<QCheckBox *>( "showNotAvailableCheckBox" );
	_awaySpinBox = _widget->findChild<QSpinBox *>( "awaySpinBox" );
	_notAvailableSpinBox = _widget->findChild<QSpinBox *>( "notAvailableSpinBox" );
	_editProfilePushButton = _widget->findChild<QPushButton *>( "editProfilePushButton" );

}


