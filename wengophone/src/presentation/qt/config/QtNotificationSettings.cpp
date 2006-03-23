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

	asSwitchOffAwayAudioNotifications = Object::findChild<QCheckBox *>(_widget,"asSwitchOffAwayAudioNotifications" );

	_acIncomingChat = Object::findChild<QCheckBox *>(_widget,"acIncomingChat" );

	_acIncomingCalls = Object::findChild<QCheckBox *>(_widget,"acIncomingCalls" );

	_acWengophoneOpen = Object::findChild<QCheckBox *>(_widget,"acWengophoneOpen" );

	_acWengophoneConnects = Object::findChild<QCheckBox *>(_widget,"acWengophoneConnects" );

	_acWengophoneNotSuccedConnect = Object::findChild<QCheckBox *>(_widget,"acWengophoneNotSuccedConnect" );

	_acAlternativeIMAcount = Object::findChild<QCheckBox *>(_widget,"acAlternativeIMAcount" );

	_acContactTurningOnline = Object::findChild<QCheckBox *>(_widget,"acContactTurningOnline" );

	_acAuthorization = Object::findChild<QCheckBox *>(_widget,"acAuthorization" );

	_acErrorMessage = Object::findChild<QCheckBox *>(_widget,"acErrorMessage" );

}
