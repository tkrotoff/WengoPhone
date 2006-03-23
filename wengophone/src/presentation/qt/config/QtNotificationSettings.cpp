/*
* WengoPhone, a voice over Internet phone
* Copyright ( C ) 2004 - 2005 Wengo
*
* This program is free software;
you can redistribute it and / or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation;
either version 2 of the License, or
* ( at your option ) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY;
without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the

* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program;

if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 - 1307 USA
*/


#include <QtGui>
#include <QGridLayout>
#include <qtutil/Object.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>


#include "QtNotificationSettings.h"

/**
 * Load and display the notification settings configuration page
 *
 * Creates widgets dynamically (from a XML file .ui).
 *
 * @author Kavoos Bojnourdi
 */

QtNotificationSettings::QtNotificationSettings( QWidget * parent, Qt::WFlags f )
		: QWidget( parent, f ) {
	QStringList strlist;

	_widget = WidgetFactory::create( ":/forms/config/NotificationSettings.ui", this );
	QGridLayout * layout = new QGridLayout();
	layout->addWidget( _widget );
	layout->setMargin( 0 );
	setLayout( layout );
	setupChilds();
	readConfigData();
}

void QtNotificationSettings::setupChilds() {

	_snShowWindowOnTopCheckBox = Object::findChild<QCheckBox *>(_widget,"snShowWindowOnTopCheckBox" );

	_snShowBlinkingWindow = Object::findChild<QCheckBox *>(_widget,"snShowBlinkingWindow" );

	_snShowToasterOnIncomingCall = Object::findChild<QCheckBox *>(_widget,"snShowToasterOnIncomingCall" );

	_snShowToasterOnContactOnLine = Object::findChild<QCheckBox *>(_widget,"snShowToasterOnContactOnLine" );

	_snShowWindowpopUpAuthorisations = Object::findChild<QCheckBox *>(_widget,"snShowWindowpopUpAuthorisations" );

	_asShowNoToaster = Object::findChild<QCheckBox *>(_widget,"asShowNoToaster" );

	_asSwitchOffAudioNotifications = Object::findChild<QCheckBox *>(_widget,"asSwitchOffAudioNotifications" );

	_asNotShowAwayToasterAndChatWindow = Object::findChild<QCheckBox *>(_widget,"asNotShowAwayToasterAndChatWindow" );

	_asSwitchOffAwayAudio = Object::findChild<QCheckBox *>(_widget,"asSwitchOffAwayAudioNotifications" );

	_acIncomingChat = Object::findChild<QCheckBox *>(_widget,"acIncomingChat" );

	_acIncomingCalls = Object::findChild<QCheckBox *>(_widget,"acIncomingCalls" );

	_acWengophoneOpens = Object::findChild<QCheckBox *>(_widget,"acWengophoneOpen" );

	_acWengophoneConnects = Object::findChild<QCheckBox *>(_widget,"acWengophoneConnects" );

	_acWengophoneNotSuccedConnect = Object::findChild<QCheckBox *>(_widget,"acWengophoneNotSuccedConnect" );

	_acAlternativeIMAcount = Object::findChild<QCheckBox *>(_widget,"acAlternativeIMAcount" );

	_acContactTurningOnline = Object::findChild<QCheckBox *>(_widget,"acContactTurningOnline" );

	_acAuthorization = Object::findChild<QCheckBox *>(_widget,"acAuthorization" );

	_acErrorMessage = Object::findChild<QCheckBox *>(_widget,"acErrorMessage" );

}

void QtNotificationSettings::readConfigData(){
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	setChecked( _snShowWindowOnTopCheckBox,config.getNotificationShowWindowOnTop() );
	setChecked( _snShowBlinkingWindow, config.getNotificationShowBlinkingWindow());
	setChecked( _snShowToasterOnIncomingCall, config.getNotificationShowToasterOnIncomingCall());
	setChecked( _snShowToasterOnContactOnLine, config.getNotificationShowToasterOnContactOnline());
	setChecked( _snShowWindowpopUpAuthorisations, config.getNotificationShowWindowPopupAuthorization());
	setChecked( _asShowNoToaster, config.getNotificationShowNoToaster() );
	setChecked( _asSwitchOffAudioNotifications, config.getNotificationSwithOffAudioNotification() );
	setChecked( _asNotShowAwayToasterAndChatWindow, config.getNotificationNotShowAwayToasterAndChat() );
	setChecked( _asSwitchOffAwayAudio, config.getNotificationSwitchOffAwayAudio() );
	setChecked( _acIncomingChat, config.getNotificationIncomingChat() );
	setChecked( _acIncomingCalls, config.getNotificationIncomingCalls() );
	setChecked( _acWengophoneOpens, config.getNotificationWengophoneOpens() );
	setChecked( _acWengophoneConnects, config.getNotificationWengophoneConnects() );
	setChecked( _acWengophoneNotSuccedConnect, config.getNotificationWengophoneNotSuccedConnect() );
	setChecked( _acAlternativeIMAcount, config.getNotificationAlternativeIMAccount() );
	setChecked( _acContactTurningOnline, config.getNotificationContactTurningOnline() );
	setChecked( _acAuthorization, config.getNotificationAuthorization() );
	setChecked( _acErrorMessage, config.getNotificationErrorMessage() );

}

void QtNotificationSettings::saveData(){
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set( config.NOTIFICATION_SHOW_WINDOWONTOP_KEY, isChecked(_snShowWindowOnTopCheckBox) );
	config.set( config.NOTIFICATION_SHOW_BLINKINGWINDOW_KEY, isChecked(_snShowBlinkingWindow) );
	config.set( config.NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY, isChecked(_snShowToasterOnIncomingCall) );
	config.set( config.NOTIFICATION_SHOW_TOASTER_ON_CONTACT_ONLINE_KEY, isChecked(_snShowToasterOnContactOnLine) );
	config.set( config.NOTIFICATION_SHOW_WINDOWPOPUP_AUTHORIZATION_KEY, isChecked(_snShowWindowpopUpAuthorisations) );
	config.set( config.NOTIFICATION_SHOW_NO_TOASTER_KEY, isChecked(_asShowNoToaster) );
	config.set( config.NOTIFICATION_SWITCH_OFF_AUDIO_NOFITICATION_KEY, isChecked(_asSwitchOffAudioNotifications) );
	config.set( config.NOTIFICATION_NOT_SHOW_AWAY_TOASTER_AND_CHAT_KEY, isChecked(_asNotShowAwayToasterAndChatWindow) );
	config.set( config.NOTIFICATION_SWITCH_OFF_AWAY_AUDIO_KEY, isChecked(_asSwitchOffAwayAudio) );
	config.set( config.NOTIFICATION_INCOMING_CHAT_KEY, isChecked(_acIncomingChat) );
	config.set( config.NOTIFICATION_INCOMING_CALLS_KEY, isChecked(_acIncomingCalls) );
	config.set( config.NOTIFICATION_WENGOPHONE_OPENS_KEY, isChecked(_acWengophoneOpens) );
	config.set( config.NOTIFICATION_WENGOPHONE_CONNECTS_KEY, isChecked(_acWengophoneConnects) );
	config.set( config.NOTIFICATION_WENGOPHONE_NOT_SUCCED_CONNECT_KEY, isChecked(_acWengophoneNotSuccedConnect) );
	config.set( config.NOTIFICATION_ALTERNATIVE_IMACCOUNT_KEY, isChecked(_acAlternativeIMAcount) );
	config.set( config.NOTIFICATION_CONTACT_TURNING_ONLINE_KEY, isChecked(_acContactTurningOnline) );
	config.set( config.NOTIFICATION_AUTHORIZATION_KEY, isChecked(_acAuthorization) );
	config.set( config.NOTIFICATION_ERROR_MESSAGE_KEY, isChecked(_acErrorMessage) );

}

void QtNotificationSettings::setChecked(QCheckBox * chk, bool value){

	if ( value )
		chk->setCheckState(Qt::Checked);
	else
		chk->setCheckState(Qt::Unchecked);
}

bool QtNotificationSettings::isChecked(QCheckBox * chk){
	if ( chk->checkState() == Qt::Checked )
		return true;
	return false;
}
