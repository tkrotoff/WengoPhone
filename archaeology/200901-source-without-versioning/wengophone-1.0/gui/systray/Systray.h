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

#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <qobject.h>
#include <qpixmap.h>

class Contact;
class MainWindow;
class SessionWindow;
class TrayIcon;
class QWidget;
class QPopupMenu;
class QTimer;

/**
 * Wengophone Systray.
 *
 * Uses the class TrayIcon from the software Psi: a multiplatform Jabber client in Qt.
 *
 * @author Tanguy Krotoff
 */
class Systray : public QObject {
	Q_OBJECT
public:

	/**
	 * Constructs the Systray and connects it to functionnalities
	 * from the MainWindow.
	 *
	 * @param mainWindow MainWindow of WengoPhone
	 */
	Systray(MainWindow & mainWindow);

	~Systray();

public slots:

	/**
	 * Changes the Systray icon to the default one.
	 */
	void setToDefault();

	/**
	 * Changes the Systray icon to the one that represents a configuration problem.
	 */
	void setToConfigError();

	/**
	 * Changes the Systray icon and shows that a call is incoming.
	 */
	void setCallIncoming(const QString & phoneNumber, Contact * contact);

	/**
	 * Changes the Systray icon and shows that a call is outgoing.
	 */
	void setCallOutgoing();

	/**
	 * Changes the Systray icon and shows that a call was missed.
	 */
	void setCallMissed();

	/**
	 * Hide the Systray icon.
	 */
	void hide();

	/**
	 * The user has clicked on quit from the Systray menu.
	 */
	void quit();

private slots:

	/**
	 * Animates the Systray icon.
	 */
	void animatedIcon();

	/**
	 * Changes the Systray icon to the default one if it was
	 * previously a call missed.
	 */
	void setToDefaultIfCallMissed(const QPoint &, int);

	void updateMenu();

private:

	Systray(const Systray &);
	Systray & operator=(const Systray &);

	/**
	 * The Systray itself.
	 *
	 * This class is from the software Psi: a multiplatform Jabber client in Qt.
	 */
	TrayIcon * _trayIcon;

	/**
	 * QTimer timeout in milliseconds.
	 */
	static const int TIMER_TIMEOUT;

	/**
	 * Timer for the icon animation.
	 */
	QTimer * _timer;

	/**
	 * Current icon showed by the Systray icon.
	 */
	QPixmap _currentIcon;

	/**
	 * Defines the state of the Systray.
	 */
	enum State { Default = 0, CallIncoming = 1, CallOutgoing = 2, CallMissed = 3 };

	/**
	 * State of the Systray icon.
	 */
	State _state;

	MainWindow & _mainWindow;

	QPopupMenu * _menu;
};

#endif	//SYSTRAY_H
