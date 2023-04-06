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

#ifndef SYSTRAYPOPUPWINDOW_H
#define SYSTRAYPOPUPWINDOW_H

#include <qobject.h>
#include <qcursor.h>

class Contact;
class QLabel;
class QDialog;
class QString;
class QTimer;

/**
 * Wengophone Systray popup window.
 *
 * Shows a popup window close to the systray like MSN or Skype or Outlook
 * when an email is received.
 *
 * @author Tanguy Krotoff
 */
class SystrayPopupWindow : public QObject {
	Q_OBJECT
public:

	SystrayPopupWindow();

	~SystrayPopupWindow();

	void setTitle(const QString & title);

	void setContact(const QString & phoneNumber, Contact * contact);

private slots:

	void audioCallButtonClicked();
	void videoCallButtonClicked();
	void rejectCallButtonClicked();

	void showPopupProgressing();

private:

	QDialog * _systrayPopupWindow;

	QLabel * _phoneNumberLabel;

	/**
	 * QTimer timeout in milliseconds.
	 */
	static const int TIMER_TIMEOUT;

	/**
	 * Timer for the slow presentation of the popup.
	 */
	QTimer * _timer;

	double _timeoutOpacity;
};

#endif	//SYSTRAYPOPUPWINDOW_H
