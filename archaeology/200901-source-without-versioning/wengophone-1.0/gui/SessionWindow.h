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

#ifndef SESSIONWINDOW_H
#define SESSIONWINDOW_H

#include "AudioCall.h"
#include "contact/Contact.h"
#include "PhonePageWidget.h"
#include "sms/SmsPageWidget.h"
#include "chat/ChatPageWidget.h"
#ifdef ENABLE_VIDEO
#include "visio/VisioPageWidget.h"
#endif

#include <qobject.h>
#include <qsize.h>

class ContactWidget;
class QMainWindow;
class QDialog;
class QTabWidget;
class QWidget;
class QString;

/**
 * Gui widget for a communication session.
 *
 * A communication session regroups audio, video, file transfer and
 * chat (instant messanging).
 * In order to create a SessionWindow one should check the class AudioCall
 * which is the only class to be able to create a SessionWindow.
 *
 * @see SessionWindowForm.ui
 * @author Tanguy Krotoff
 */
class SessionWindow : public QObject {
	Q_OBJECT
public:

	/**
	 * Then AudioCall can construct a SessionWindow since
	 * the constructor is private.
	 */
	friend class AudioCall;

	~SessionWindow();

	/**
	 * Gets the low-level gui widget of this class.
	 *
	 * @return the low-level widget
	 */
	QDialog * getWidget() const {
		return (QDialog *) _sessionWindow;
	}

	Contact * getContact() const {
		return _contact;
	}

	/**
	 * Gets the phone component of the SessionWindow.
	 *
	 * @return the phone component
	 */
	PhonePageWidget & getPhonePageWidget() const {
		return *_phonePageWidget;
	}

#ifdef ENABLE_VIDEO
	VisioPageWidget & getVisioPageWidget() const {
		return *_visioPageWidget;
	}
#endif
	/**
	 * Shows the page to compose a phone number and to call.
	 *
	 * @param sipAddress SIP address to compose
	 * @param contact Contact we want to associate with the SessionWindow
	 */
	void showOutgoingCallPhonePage(const SipAddress & sipAddress, Contact * contact = NULL);

	/**
	 * Shows the page when a call is incoming.
	 *
	 * @param sipAddress SIP uri incoming
	 * @return contact Contact associated with the SessionWindow
	 */
	Contact * showIncomingCallPhonePage(const SipAddress & sipAddress);

	/**
	 * Shows the default page of the SessionWindow.
	 */
	void showEmptyDefaultPage();

	/**
	 * Shows the page on the SMS tab.
	 *
	 * The user wants to send a SMS.
	 */
	void sendSms(const QString & phoneNumber, const QString & message = QString::null, Contact * contact = NULL);

	void startChat(Contact & contact, const QString & messageReceived = QString::null);

	/**
	 * Resets _contact to NULL.
	 * Fix a crash when changing from one account to another.
	 */
	void reset() {
		_contact = NULL;
	}

	/**
	 * Index of the phone page inside the SessionWindow.
	 */
	static const int INDEX_PHONE_PAGE;

	/**
	 * Index of the SMS page inside the SessionWindow.
	 */
	static const int INDEX_SMS_PAGE;

	/**
	 * Index of the chat page inside the SessionWindow.
	 */
	static const int INDEX_CHAT_PAGE;

	/**
	 * Index of the video page inside the SessionWindow.
	 */
	static const int INDEX_VIDEO_PAGE;

	int getCurrentPageIndex() const {
		return _currentPageIndex;
	}

	/**
	 * Show SMS page & give it the focus.
	 */
	void showSmsPage();

public slots:

	void showVisioPage();

	/**
	 * Shows the current (incoming/outgoing) phone call.
	 *
	 * Used by Systray so that we click on the Systray it
	 * show the current (incoming.outgoing) call.
	 *
	 * @see Systray
	 */
	void showCurrentCallPhonePage();

	/**
	 * The SessionWindow has been closed by the user,
	 * the conversation should be stopped aswell.
	 */
	void closeWindow();

private slots:

	/**
	 * The current page of this SessionWindow has been changed.
	 *
	 * @param currentPage the new current page of the SessionWindow
	 */
	void pageChanged(QWidget * currentPage);

	/**
	 * The user clicked on the button "edit profile".
	 */
	void editContact();

	void resize();

private:

	/**
	 * Constructs a SessionWindow.
	 *
	 * @param audioCall belonging to
	 * @param parent parent widget
	 */
	SessionWindow(AudioCall & audioCall, QWidget * parent = 0);

	SessionWindow(const SessionWindow &);
	SessionWindow & operator=(const SessionWindow &);

	/**
	 * Makes the association between a SessionWindow and a Contact.
	 *
	 * @param phoneNumber phone number of the Contact
	 * @param displayName display name of the Contact
	 * @param contact Contact we want to associate with the SessionWindow
	 */
	void setContact(const QString & phoneNumber, const QString & displayName, Contact * contact);

	void flashWindow();

	/**
	 * AudioCall attached to this widget
	 */
	AudioCall & _audioCall;

	/**
	 * Low-level widget of this class.
	 */
	QMainWindow * _sessionWindow;

	/**
	 * Phone widget component of the SessionWindow.
	 */
	PhonePageWidget * _phonePageWidget;

	/**
	 * SMS widget component of the SessionWindow.
	 */
	SmsPageWidget * _smsPageWidget;

	/**
	 * Chat conversation widget component of the SessionWindow.
	 */
	ChatPageWidget * _chatPageWidget;

#ifdef ENABLE_VIDEO
	/**
	 * Visio conversation widget component of the SessionWindow.
	 */
	//static VisioPageWidget * _visioPageWidget;
	VisioPageWidget * _visioPageWidget;
#endif

	/**
	 * Contact corresponding to this SessionWindow.
	 *
	 * NULL if the Contact is not known.
	 */
	Contact * _contact;

	/**
	 * Component that handles different tabs/pages for this class.
	 */
	QTabWidget * _tabWidget;

	int _currentPageIndex;

	QSize _smsPageSize;
	QSize _chatPageSize;
	QSize _videoPageSize;
};

#endif	//SESSIONWINDOW_H
