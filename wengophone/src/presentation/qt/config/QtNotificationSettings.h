#ifndef NOTIFICATIONSETTINGS_H
#define NOTIFICATIONSETTINGS_H

#include <QtGui>
#include <qtutil/WidgetFactory.h>

/**
 * Load and display the notification settings configuration page
 *
 * Creates widgets dynamically (from a XML file .ui).
 *
 * @author Kavoos Bojnourdi
 */
class QtNotificationSettings : public QWidget {

		Q_OBJECT

	public:
		QtNotificationSettings( QWidget * parent = 0, Qt::WFlags f = 0 );

		void saveData();

	protected:
		QWidget * _widget;

		void setupChilds();

		void readConfigData();

		void setChecked(QCheckBox * chk, bool value);

		bool isChecked(QCheckBox * chk);

		// Standard notifications
		QCheckBox * _snShowWindowOnTopCheckBox;

		QCheckBox * _snShowBlinkingWindow;

		QCheckBox * _snShowToasterOnIncomingCall;

		QCheckBox * _snShowToasterOnContactOnLine;

		QCheckBox * _snShowWindowpopUpAuthorisations;

		QCheckBox * _asShowNoToaster;

		QCheckBox * _asSwitchOffAudioNotifications;

		QCheckBox * _asNotShowAwayToasterAndChatWindow;

		QCheckBox * _asSwitchOffAwayAudio;

		QCheckBox * _acIncomingChat;

		QCheckBox * _acIncomingCalls;

		QCheckBox * _acWengophoneOpens;

		QCheckBox * _acWengophoneConnects;

		QCheckBox * _acWengophoneNotSuccedConnect;

		QCheckBox * _acAlternativeIMAcount;

		QCheckBox * _acContactTurningOnline;

		QCheckBox * _acAuthorization;

		QCheckBox * _acErrorMessage;

	};

#endif
