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

#ifndef PHONEPAGEWIDGET_H
#define PHONEPAGEWIDGET_H

#include <vector>

#include <qstring.h>
#include <qobject.h>
#include <qframe.h>
#include <qlabel.h>

class Contact;
class DialPadWidget;
class SessionWindow;
class QComboBox;
class QWidget;
class QPushButton;
class QLabel;
class QTimer;
class QTime;

/**
 * Phone widget component of the SessionWindow.
 *
 * PhonePageWidget is a tab/page inside the SessionWindow.
 * Includes a dial pad, a button to call somebody, a button to hang up the phone +
 * some informations about the Contact who is calling the user or the user wants to call.
 *
 * @author Tanguy Krotoff
 */
class PhonePageWidget : public QObject {
	Q_OBJECT
public:

	/**
	 * Then SessionWindow can construct a SessionWindow since
	 * the constructor is private.
	 */
	friend class SessionWindow;

	/**
	 * Gets the low-level gui widget of this class.
	 *
	 * @return the low-level widget
	 */
	QWidget * getWidget() const {
		return _phonePageWidget;
	}

	void call();

	QPushButton * getAudioCallButton() const {
		return _audioCallButton;
	}

	QPushButton * getVideoCallButton() const {
		return _videoCallButton;
	}

	/**
	 * Gets the button to hang up the phone.
	 *
	 * @return the button
	 */
	QPushButton * getHangUpButton() const {
		return _hangUpButton;
	}

	/// get the vector of all number buttons
	std::vector<QPushButton *> getNumberButtons() const;

	/// get star button
	QPushButton * getStarButton() const;

	/// get sharp button
	QPushButton * getSharpButton() const;

	QComboBox * getPhoneComboBox() const {
		return _phoneComboBox;
	}

	/**
	 * Sets the phone number the user is currently calling.
	 *
	 * The phone number will be show inside this widget.
	 *
	 * @param phoneNumber phone number called
	 */
	void setPhoneNumberCalled(const QString & phoneNumber) {
		setCallerPhoneNumber(phoneNumber);
	}

	/**
	 * Sets the phone number used to call the user.
	 *
	 * The phone number will be show inside this widget.
	 *
	 * @param phoneNumber phone number calling
	 */
	void setCallerPhoneNumber(const QString & phoneNumber);

	/**
	 * Gets the phone number used to call the user.
	 *
	 * @return phone number calling
	 */
	QString getCallerPhoneNumber() const;

	/**
	 * Sets the phone number the user wants to call.
	 *
	 * The phone number will be inside the combobox of this widget.
	 *
	 * @param phoneNumber phone number to call
	 */
	void setPhoneNumberToCall(const QString & phoneNumber);

	/**
	 * Gets the phone number the user wants to call.
	 *
	 * @return phone number to call
	 */
	QString getPhoneNumberToCall() const;

	/**
	 * Associates a Contact to this widget.
	 *
	 * Contact the user wants to call or the Contact that is calling the user.
	 * All informations needed by this widget will be get from the Contact.
	 *
	 * @param contact Contact associates to this widget
	 */
	void setContact(Contact & contact);

	/**
	 * Gets the name of the Contact the user wants to call or the name
	 * of the Contact who is calling, inside this widget.
	 *
	 * @return name of the Contact to call
	 */
	QString getContactName() const;

	/**
	 * Changes the status text.
	 *
	 * Gives informations to the user.
	 *
	 * @param statusText new status text
	 */
	void setStatusText(const QString & statusText);

	/**
	 * Sets the focus on this gui component.
	 */
	void setFocus();

	/**
	 * Resets the PhonePageWidget: no Contact is associated anymore with the PhonePageWidget.
	 */
	void reset();

	void startCallDurationTimer();

	void stopCallDurationTimer();

	QWidget * getVideoWidget() const {
		return _contactPixmap;
	}

private slots:

	void updateCallDuration();

private:

	/**
	 * Constructs the PhonePageWidget.
	 *
	 * @param parent parent widget
	 */
	PhonePageWidget(SessionWindow * sessionWindow);

	PhonePageWidget(const PhonePageWidget &);
	PhonePageWidget & operator=(const PhonePageWidget &);

	/**
	 * Sets the name of the Contact the user wants to call or the name
	 * of the Contact who is calling, inside this widget.
	 *
	 * @param name name of the Contact to call
	 */
	void setContactName(const QString & name);

	/**
	 * Sets the picture of the Contact the user wants to call or
	 * the picture of the Contact who is calling, inside this widget.
	 *
	 * @param picture picture of the Contact
	 */
	void setContactPicture(const QString & picture);

	/**
	 * Low-level widget of this class.
	 */
	QWidget * _phonePageWidget;

	/**
	 * DialPadwidget.
	 */
	DialPadWidget * _dialPadWidget;

	QPushButton * _audioCallButton;

	QPushButton * _videoCallButton;

	QPushButton * _hangUpButton;

	/**
	 * Combo box where the user enters the phone number he wants to call.
	 */
	QComboBox * _phoneComboBox;

	/**
	 * Label that contains the phone number called.
	 *
	 * @see setPhoneNumberCalled()
	 */
	QLabel * _phoneLabel;

	/**
	 * Contact associates with this widget.
	 *
	 * Contact the user wants to call or the Contact that is calling the user.
	 * Can be NULL if no Contact is associated with this widget,
	 * for example in the case an unknow Contact phones the user.
	 */
	Contact * _contact;

	/**
	 * Contains the name of the Contact the user wants to call
	 * or the name of the Contact that is calling.
	 */
	QLabel * _nameLabel;

	/**
	 * Status label of the SessionWindow, writes messages in this status bar.
	 */
	QLabel * _statusLabel;

	/**
	 * QTimer timeout in milliseconds.
	 */
	static const Q_ULLONG TIMER_TIMEOUT;

	/**
	 * Timer to update the call duration every second.
	 */
	QTimer * _callDurationTimer;

	QTime * _callDuration;

	QFrame * _videoFrame;

	QLabel * _contactPixmap;
};

#endif	//PHONEPAGEWIDGET_H
