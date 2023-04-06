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

#ifndef CALLLOG_H
#define CALLLOG_H

#include <qdatetime.h>
#include <qstring.h>
#include <qpixmap.h>

/**
 * One record (a log) of one call.
 *
 * A call can be of several types: incoming, outgoing, missed.
 * It can be an audio call or a SMS.
 *
 * @see CallHistory
 * @author Tanguy Krotoff
 */
class CallLog {
public:

	/**
	 * Type of call.
	 */
	enum Type {
		CallIncoming,
		CallOutgoing,
		CallMissed,
		SmsOutgoing,
		SmsIncoming
	};

	/**
	 * Initializes the time of the call to the 
	 *
	 */
	CallLog();

	CallLog(const CallLog & callLog);

	~CallLog();

	/**
	 * Sets the kind of call.
	 *
	 * @param type type of call
	 */
	void setType(Type type) {
		_type = type;
	}

	/**
	 * Sets the kind of call.
	 *
	 * Used by the XML reader that reads strings only.
	 *
	 * @param type type of call
	 */
	void setType(const QString & type);

	/**
	 * Gets the kind of call.
	 *
	 * Used by the XML writer that writes strings only.
	 *
	 * @return type of call as a string
	 */
	QString getTypeString() const;

	/**
	 * Translated version of getTypeString().
	 *
	 * @see getTypeString()
	 * @see CallHistoryWidget
	 */
	QString getTypeStringTranslated() const;

	/**
	 * Gets the icon to put inside the CallHistoryWidget
	 *
	 * @return the icon
	 */
	QPixmap getTypeIcon() const;

	QDateTime getDateTime() const {
		return _callDateTime;
	}

	/**
	 * Sets the date time of the call.
	 *
	 * Used by the XML reader that reads strings only.
	 *
	 * @param dateTime date time of the call
	 */
	void setDateTime(const QDateTime & dateTime) {
		_callDateTime = dateTime;
	}

	/**
	 * Sets the name of the contact, can be a phone number if the contact has no name.
	 *
	 * @param contactName name or phone number of the contact
	 */
	void setContactName(const QString & contactName) {
		_contactName = contactName;
	}

	/**
	 * Gets contact name, can be a phone number if there is no name.
	 *
	 * @return contact name
	 */
	QString getContactName() const {
		return _contactName;
	}

	void setPhoneNumber(const QString & phoneNumber) {
		_phoneNumber = phoneNumber;
	}

	QString getPhoneNumber() const {
		return _phoneNumber;
	}

	void setSms(const QString & sms) {
		//_sms = QString::fromUtf8(sms);
		_sms = sms;
	}

	QString getSms() const {
		//return QString::fromUtf8(_sms);
		return _sms;
	}

	/**
	 * Gets the call duration.
	 *
	 * @return call duration
	 */
	QTime getDuration() const;

	/**
	 * Gets the call duration in seconds as an integer in seconds.
	 *
	 * @return call duration as an integer in seconds
	 */
	unsigned long getDurationForXml() const {
		return _duration;
	}

	/**
	 * Sets the call duration
	 *
	 * Used by the XML reader that reads strings only.
	 *
	 * @param duration call duration in seconds
	 */
	void setDuration(unsigned long duration);

	/**
	 * Starts the timer in order to compute the call duration.
	 */
	void startTimer();

	/**
	 * Stops the timer in order to compute the call duration.
	 */
	void stopTimer();

private:

	CallLog & operator=(const CallLog &);

	/**
	 * Type of call: incoming, outgoing or missed.
	 */
	Type _type;

	/**
	 * Time in seconds since 1970 when the call started.
	 */
	unsigned int _timer;

	/**
	 * Call duration in seconds.
	 */
	unsigned long _duration;

	/**
	 * Date and time of the call.
	 *
	 * Initialized inside the constructor so that it 
	 * corresponds to the current date time.
	 */
	QDateTime _callDateTime;

	/**
	 * Name or phone number of the contact associated with this CallLog.
	 *
	 * Corresponds to a phone number if the contact is not known (=no name).
	 */
	QString _contactName;

	/**
	 * Phone number associated with this CallLog.
	 */
	QString _phoneNumber;

	QString _sms;
};

#endif	//CALLLOG_H
