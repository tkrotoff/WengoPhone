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

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <qobject.h>

class QMainWindow;
class QStatusBar;
class QPushButton;
class QLabel;

/**
 * Gui widget StatusBar of the main window.
 *
 * @author Tanguy Krotoff
 */
class StatusBar : public QObject {
	Q_OBJECT
public:

	/**
	 * Constructs the StatusBar given the main window.
	 *
	 * @param mainWindow the main window where the StatusBar will be added
	 */
	StatusBar(QMainWindow * mainWindow);

	~StatusBar();

	/**
	 * Gets the low-level widget.
	 *
	 * @return the low-level status bar
	 */
	QStatusBar * getWidget() const;

	void setConnectionStatus(bool status);
	bool isConnectionEnabled() const;

	void setUpToDateStatus(bool status);
	bool isUpToDate() const;

	void setSipStatus(bool status);
	bool isSipEnabled() const;

	void setAudioStatus(bool status);
	bool isAudioEnabled() const;

	void setMessage(const QString & message);

private:

	StatusBar(const StatusBar &);
	StatusBar & operator=(const StatusBar &);

	void changeSystrayIcon() const;

	/**
	 * Low-level widget.
	 */
	QStatusBar * _statusBar;

	QLabel * _connectionLabel;

	QLabel * _updateLabel;

	QLabel * _sipLabel;

	QLabel * _audioLabel;

	bool _isConnectionEnabled;

	bool _isUpToDate;

	bool _isSipEnabled;

	bool _isAudioEnabled;
    
	bool _firstRegister;
};

#endif	//STATUSBAR_H
