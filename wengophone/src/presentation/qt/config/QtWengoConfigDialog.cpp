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

#include <QtGui>
#include <qtutil/Object.h>
#include <qtutil/WidgetFactory.h>

#include "QtWengoConfigDialog.h"



QtWengoConfigDialog::QtWengoConfigDialog( QWidget * parent, Qt::WFlags f ) : QDialog ( parent, f ) {
	_widget = WidgetFactory::create( ":/forms/config/WengoConfigDialog.ui", this );

	QGridLayout * layout = new QGridLayout();
	layout->addWidget( _widget );
	layout->setMargin( 0 );
	setLayout( layout );

	_notificationWidget = new QtNotificationSettings();
	_generalSettingsWidget = new QtGeneralSettings();
	_accountSettingsWidget = new QtAccountSettings();
	_privacySettingsWidget = new QtPrivacySettings();
	_audioSettingsWidget = new QtAudioSettings();
	_videoSettingsWidget = new QtVideoSettings();
	_advancedSettingsWidget = new QtAdvancedSettings();
	_callForwardWidget = new QtCallForwardSettings();

	_stackedWidget = Object::findChild<QStackedWidget *>( _widget, "stackedWidget" );
	_treeWidget = Object::findChild<QTreeWidget *>( _widget, "treeWidget" );

	// Hide the header of the tree view
	_treeWidget->header() ->hide();

	_stackedWidget->addWidget( _notificationWidget );
	_stackedWidget->addWidget( _generalSettingsWidget );
	_stackedWidget->addWidget( _accountSettingsWidget );
	_stackedWidget->addWidget( _privacySettingsWidget );
	_stackedWidget->addWidget( _audioSettingsWidget );
	_stackedWidget->addWidget( _videoSettingsWidget );
	_stackedWidget->addWidget( _callForwardWidget );
	_stackedWidget->addWidget( _advancedSettingsWidget );

	_stackedWidget->setCurrentWidget( _generalSettingsWidget );

	QPushButton * pb;

	pb = Object::findChild<QPushButton *>( _widget, "okButton" );
	connect ( pb, SIGNAL( clicked() ), this, SLOT( accept() ) );
	pb = Object::findChild<QPushButton *>( _widget, "cancelButton" );
	connect ( pb, SIGNAL( clicked() ), this, SLOT( reject() ) );

	connect ( _treeWidget, SIGNAL( itemSelectionChanged() ), this, SLOT( itemActivated ( ) ) );
}

void QtWengoConfigDialog::itemActivated ( ) {
	QString itemText;
	const QList<QTreeWidgetItem *> itemList = _treeWidget->selectedItems();
	itemText = itemList[ 0 ] ->text( 0 );

	if ( itemText == "General" )
		_stackedWidget->setCurrentWidget( _generalSettingsWidget );

	if ( itemText == "Notifications & Sounds" )
		_stackedWidget->setCurrentWidget( _notificationWidget );

	if ( itemText == "Accounts" )
		_stackedWidget->setCurrentWidget( _accountSettingsWidget );

	if ( itemText == "Privacy" )
		_stackedWidget->setCurrentWidget( _privacySettingsWidget );

	if ( itemText == "Audio" )
		_stackedWidget->setCurrentWidget( _audioSettingsWidget );

	if ( itemText == "Video" )
		_stackedWidget->setCurrentWidget( _videoSettingsWidget );

	if ( itemText == "Advanced" )
		_stackedWidget->setCurrentWidget( _advancedSettingsWidget );

	if ( itemText == "Call Forward" )
		_stackedWidget->setCurrentWidget( _callForwardWidget );
}

void QtWengoConfigDialog::accept(){

	_generalSettingsWidget->saveData();
	_notificationWidget->saveData();
	_privacySettingsWidget->saveData();
	_audioSettingsWidget->saveData();
	_callForwardWidget->saveData();
	_videoSettingsWidget->saveData();

	QDialog::accept();
}