/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007 Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef OWISMSSESSION_H
#define OWISMSSESSION_H

#include <coipmanager/coipmanagerdll.h>

#include <util/Interface.h>
#include <util/StringList.h>

/**
 * SMSSession interface class.
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 */
class COIPMANAGER_API ISMSSession : Interface {
public:

	/**
	 * Gets the SMS message.
	 */
	std::string getMessage() const;

	/**
	 * Sets the message to send.
	 */
	void setMessage(const std::string & message);

	/**
	 * Adds phone number in the phone number list.
	 */
	void addPhoneNumber(const std::string & phoneNumber);

	/**
	 * Sets the phone number list.
	 */
	void setPhoneNumberList(const StringList & phoneNumberList);

	/**
	 * Gets the list of phone number.
	 */
	StringList getPhoneNumberList() const;

protected:

	/** The SMS Message. */
	std::string _smsMessage;

	/** Lists of phone number to use. */
	StringList _phoneNumberList;

};

#endif //OWISMSSESSION_H
