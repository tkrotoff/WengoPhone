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

#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <qobject.h>
#include <qtimer.h>

class CheckUpdate;
class QDialog;
class QWidget;

/**
 * WengoPhone about window.
 *
 * Shows the copyright + the current build id + the last build id available.
 *
 * @author Tanguy Krotoff
 */
class AboutWindow : public QObject {
	Q_OBJECT
public:

	AboutWindow(QWidget * parent);

	~AboutWindow();

	/**
	 * Gets the low-level gui widget of this class.
	 *
	 * @return low-level widget
	 */
	QDialog * getWidget() {
		return _aboutWindow;
	}

private slots:

	void updateAboutText();

	/**
	 * Checks for updates.
	 */
  void checkUpdate();
    
    void displayTranslatorsCredits() const;
private:

	AboutWindow(const AboutWindow &);
	AboutWindow & operator=(const AboutWindow &);

	/**
	 * Low-level widget of this class.
	 */
	QDialog * _aboutWindow;

	/**
	 * QTimer timeout in milliseconds.
	 */
	static const int TIMER_TIMEOUT;

	QTimer * _timer;

	CheckUpdate * _checkUpdate;
};

#endif	//ABOUTWINDOW_H
