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

	_snShowWindowOnTopCheckBox = _widget->findChild<QCheckBox *>( "snShowWindowOnTopCheckBox" );

	_snShowBlinkingWindow = _widget->findChild<QCheckBox *>( "snShowBlinkingWindow" );

	_snShowToasterOnIncomingCall = _widget->findChild<QCheckBox *>( "snShowToasterOnIncomingCall" );

	_snShowToasterOnContactOnLine = _widget->findChild<QCheckBox *>( "snShowToasterOnContactOnLine" );

	_snShowWindowpopUpAuthorisations = _widget->findChild<QCheckBox *>( "snShowWindowpopUpAuthorisations" );

	_asShowNoToaster = _widget->findChild<QCheckBox *>( "asShowNoToaster" );

	_asSwitchOffAudioNotifications = _widget->findChild<QCheckBox *>( "asSwitchOffAudioNotifications" );

	_asNotShowAwayToasterAndChatWindow = _widget->findChild<QCheckBox *>( "asNotShowAwayToasterAndChatWindow" );

	asSwitchOffAwayAudioNotifications = _widget->findChild<QCheckBox *>( "asSwitchOffAwayAudioNotifications" );

	_acIncomingChat = _widget->findChild<QCheckBox *>( "acIncomingChat" );

	_acIncomingCalls = _widget->findChild<QCheckBox *>( "acIncomingCalls" );

	_acWengophoneOpen = _widget->findChild<QCheckBox *>( "acWengophoneOpen" );

	_acWengophoneConnects = _widget->findChild<QCheckBox *>( "acWengophoneConnects" );

	_acWengophoneNotSuccedConnect = _widget->findChild<QCheckBox *>( "acWengophoneNotSuccedConnect" );

	_acAlternativeIMAcount = _widget->findChild<QCheckBox *>( "acAlternativeIMAcount" );

	_acContactTurningOnline = _widget->findChild<QCheckBox *>( "acContactTurningOnline" );

	_acAuthorization = _widget->findChild<QCheckBox *>( "acAuthorization" );

	_acErrorMessage = _widget->findChild<QCheckBox *>( "acErrorMessage" );

}
