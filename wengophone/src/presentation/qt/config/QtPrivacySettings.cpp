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

#include "QtPrivacySettings.h"

QtPrivacySettings::QtPrivacySettings ( QWidget * parent, Qt::WFlags f ) : QWidget ( parent, f ) {
	_widget = WidgetFactory::create( ":/forms/config/PrivacySettings.ui", this );
	QGridLayout * layout = new QGridLayout();
	layout->addWidget( _widget );
	layout->setMargin( 0 );
	setLayout( layout );
	setupChilds();
}

void QtPrivacySettings::setupChilds() {

	_allowCallFromAnyoneRadioButton = Object::findChild<QRadioButton *>(_widget,"allowCallFromAnyoneRadioButton" );

	_allowCallOnlyFromContactListRadioButton = Object::findChild<QRadioButton *>(_widget,"allowCallOnlyFromContactListRadioButton" );

	_allowChatsFromAnyoneRadioButton = Object::findChild<QRadioButton *>(_widget,"allowChatsFromAnyoneRadioButton" );

	_allowChatFromOnlyContactListRadioButton = Object::findChild<QRadioButton *>(_widget,"allowChatFromOnlyContactListRadioButton" );

	_alwaysSignAsInvisible = Object::findChild<QCheckBox *>(_widget,"alwaysSignAsInvisible" );

	_manageBlockedUserPushButton = Object::findChild<QPushButton *>(_widget,"manageBlockedUserPushButton" );
}
