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

#ifndef SMSPAGEWIDGET_H
#define SMSPAGEWIDGET_H

#include <qobject.h>

class SessionWindow;
class Contact;
class QComboBox;
class QWidget;
class QTextEdit;
class QLineEdit;
class QStringList;
class QPushButton;

/**
 * SMS widget component of the SessionWindow.
 *
 * SmsPageWidget is a tab/page inside the SessionWindow.
 * Includes a text edit for the SMS message and the signature,
 * a button to send the message and a combobox to select/enter a phone number.
 *
 * @see SmsPageWidgetForm.ui
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class SmsPageWidget : public QObject {
	Q_OBJECT
public:

	/**
	 * Then SessionWindow can construct a SmsPageWidget since
	 * the constructor is private.
	 */
	friend class SessionWindow;

	/**
	 * Gets the low-level gui widget of this class.
	 *
	 * @return the low-level widget
	 */
	QWidget * getWidget() const {
		return _smsPageWidget;
	}

	/**
	 * Sets the Contact associated with the SMS message.
	 *
	 * The Contact is the potential receiver of the SMS message.
	 *
	 * @param phoneNumber phone number of the Contact
	 * @param contact the receiver Contact itself
	 */
	void setContact(const QString & phoneNumber, const Contact & contact);

	/**
	 * Sets the SMS message content.
	 *
	 * @param message the SMS message
	 */
	void setMessage(const QString & message);

	/**
	 * Sets the focus on this gui component.
	 */
	void setFocus();
	
	void setSendButtonEnabled(bool b);

public slots:

	/**
	 * The signal connected to this slot is emitted just
	 * before a managed widget is shown.
	 *
	 * @param widget widget about to be shown
	 * @see QWidgetStack::aboutToShow()
	 * @see SessionWindow
	 */
	void show(QWidget * widget);

private slots:

	/**
	 * Sends the SMS message via HTTP
	 *
	 * @see Sms
	 */
	void sendSms() const;

	/**
	 * The SMS message inside the widget has been changed.
	 */
	void smsTextChanged();

private:

	/**
	 * Constructs the SmsPageWidget.
	 *
	 * @param parent parent widget
	 */
	SmsPageWidget(QWidget * parent);

	SmsPageWidget(const SmsPageWidget &);
	
	SmsPageWidget & operator=(const SmsPageWidget &);

	/**
	 * Low-level widget of this class.
	 */
	QWidget * _smsPageWidget;

	/**
	 * Combo box where the user enters the phone number he wants to send a SMS.
	 */
	QComboBox * _phoneComboBox;

	/**
	 * Widget that stores the SMS message.
	 */
	QTextEdit * _smsText;

	/**
	 * Widget that stores the SMS signature.
	 */
	QLineEdit * _signatureText;

	QPushButton * _sendButton;
	
	/**
	 * Maximum SMS message length (150 characters).
	 */
	static const int MAX_SMS_LENGTH;

	/**
	 * Maximum SMS signature length (15 characters).
	 */
	static const unsigned int MAX_SIGNATURE_LENGTH;
};

#endif	//SMSPAGEWIDGET_H
