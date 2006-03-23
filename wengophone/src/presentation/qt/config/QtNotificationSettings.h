#ifndef NOTIFICATIONSETTINGS_H
#define NOTIFICATIONSETTINGS_H

#include <QtGui>
#include <qtutil/WidgetFactory.h>


class QtNotificationSettings : public QWidget {

		Q_OBJECT

	public:
		QtNotificationSettings( QWidget * parent = 0, Qt::WFlags f = 0 );

	protected:
		QWidget * _widget;

		void setupChilds();

		// Standard notifications
		QCheckBox * _snShowWindowOnTopCheckBox;

		QCheckBox * _snShowBlinkingWindow;

		QCheckBox * _snShowToasterOnIncomingCall;

		QCheckBox * _snShowToasterOnContactOnLine;

		QCheckBox * _snShowWindowpopUpAuthorisations;

		QCheckBox * _asShowNoToaster;

		QCheckBox * _asSwitchOffAudioNotifications;

		QCheckBox * _asNotShowAwayToasterAndChatWindow;

		QCheckBox * asSwitchOffAwayAudioNotifications;

		QCheckBox * _acIncomingChat;

		QCheckBox * _acIncomingCalls;

		QCheckBox * _acWengophoneOpen;

		QCheckBox * _acWengophoneConnects;

		QCheckBox * _acWengophoneNotSuccedConnect;

		QCheckBox * _acAlternativeIMAcount;

		QCheckBox * _acContactTurningOnline;

		QCheckBox * _acAuthorization;

		QCheckBox * _acErrorMessage;

	};

#endif
