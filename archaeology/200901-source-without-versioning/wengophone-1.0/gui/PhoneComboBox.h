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

#ifndef PHONECOMBOBOX_H
#define PHONECOMBOBOX_H

#include "callhistory/CallHistory.h"

#include <qobject.h>

class CallLog;
class QWidget;
class QComboBox;

/**
 * Special QComboBox that shows the last 5 phone numbers called by the user.
 *
 * @author Laurent Lumbroso
 */
class PhoneComboBox : public QObject {
	Q_OBJECT
public:

	PhoneComboBox(QWidget * parent);

	~PhoneComboBox();

	/**
	 * Gets the low-level gui widget.
	 *
	 * @return the low-level gui widget
	 */
	QComboBox * getWidget() const {
		return _phoneComboBox;
	}

private slots:

	/**
	 * Inserts the last 5 (MAX_PHONE_NUMBERS) phone numbers called
	 * inside the combobox.
	 */
	void insertCallHistory();

	/**
	 * The phone number from the combobox has been validated
	 * and should be called.
	 */
	void callButtonClicked();

private:

	PhoneComboBox(const PhoneComboBox &);
	PhoneComboBox & operator = (const PhoneComboBox &);

	/**
	 * Gets a string that represents a phone call.
	 *
	 * @param callLog call to transform into a string
	 * @return the string representating the CallLog
	 */
	static QString extractStringFromCallLog(const CallLog & callLog);

	/**
	 * Parses a string from the combobox and get a phone number.
	 *
	 * @param str string to parse
	 * @return the phone number corresponding to the string parsed
	 */
	static QString extractPhoneNumberFromString(const QString & str);

	/** Low-level gui widget of this class. */
	QComboBox * _phoneComboBox;

	/** Number of phone numbers inserted inside the combobox. */
	static const unsigned int MAX_PHONE_NUMBERS;
};

#endif	//PHONECOMBOBOX_H
