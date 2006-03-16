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

#include <WidgetFactory.h>

#include "QtCallForwardSettings.h"

QtCallForwardSettings::QtCallForwardSettings( QWidget * parent, Qt::WFlags f ) : QWidget ( parent, f ) {

	_widget = WidgetFactory::create( ":/forms/config/CallForwardSettings.ui", this );
	QGridLayout * layout = new QGridLayout();
	layout->addWidget( _widget );
	layout->setMargin( 0 );
	setLayout( layout );
	setupChilds();
}

void QtCallForwardSettings::setupChilds() {

	_forwardCallCheckBox = _widget->findChild<QCheckBox *>( "forwardCallCheckBox" );

	_forwardAllCallCheckBox = _widget->findChild<QCheckBox *>( "forwardAllCallCheckBox" );

	_phoneNumber1Edit = _widget->findChild<QLineEdit *>( "phoneNumber1Edit" );

	_phoneNumber2Edit = _widget->findChild<QLineEdit *>( "phoneNumber2Edit" );

	_phoneNumber3Edit = _widget->findChild<QLineEdit *>( "phoneNumber3Edit" );

	_forwardCallMobilCheckBox = _widget->findChild<QCheckBox *>( "forwardCallMobilCheckBox" );

	connect ( _forwardCallCheckBox, SIGNAL( stateChanged( int ) ), this,
			  SLOT( forwardCallStateChanged( int ) ) );

	connect ( _forwardAllCallCheckBox, SIGNAL( stateChanged( int ) ), this,
			  SLOT( forwardAllCallToStateChanged( int ) ) );

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
