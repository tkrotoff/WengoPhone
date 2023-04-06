/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWTSMSSESSION_H
#define OWTSMSSESSION_H

#include <coipmanager_threaded/session/TSession.h>

#include <coipmanager/smssessionmanager/SMSSession.h>

class TCoIpManager;

/**
 * Threaded SMSSession.
 *
 * @see SMSSession
 * @ingroup TCoIpManager
 * @author Philippe Bernery
 */
class COIPMANAGER_THREADED_API TSMSSession : public TSession {
	Q_OBJECT
	friend class TSMSSessionManager;
public:

	~TSMSSession();

	/**
	 * @see SMSSession::setMessage
	 */
	void setMessage(const std::string & message);

	/**
	 * @see SMSSession::addPhoneNumber
	 */
	void addPhoneNumber(const std::string & phoneNumber);

	/**
	 * @see SMSSession::setPhoneNumberList
	 */
	void setPhoneNumberList(const StringList & phoneNumberList);

	/**
	 * @see SMSSession::getPhoneNumberList
	 */
	StringList getPhoneNumberList() const;

Q_SIGNALS:

	void smsStateSignal(std::string phoneNumber, EnumSMSState::SMSState state);

private:

	TSMSSession(TCoIpManager & tCoIpManager, SMSSession * session);

	SMSSession * getSMSSession() const;

};

#endif //OWTSMSSESSION_H
