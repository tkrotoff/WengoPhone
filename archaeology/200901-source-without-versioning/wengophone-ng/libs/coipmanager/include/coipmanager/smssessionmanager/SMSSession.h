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

#ifndef OWSMSSESSION_H
#define OWSMSSESSION_H

#include <coipmanager/session/Session.h>
#include <coipmanager/smssessionmanager/EnumSMSState.h>
#include <coipmanager/smssessionmanager/ISMSSession.h>

class SMSSessionManager;
class ISMSSessionPlugin;

/**
 * SMSSession proxy class.
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 */
class COIPMANAGER_API SMSSession : public Session, public ISMSSession {
	Q_OBJECT
	friend class SMSSessionManager;
public:

	virtual ~SMSSession();

Q_SIGNALS:

	/** Emitted after SMS send to get send result. */
	void smsStateSignal(std::string phoneNumber, EnumSMSState::SMSState state);

public Q_SLOTS:

	// Inherited from Session
	virtual void start();

	virtual void stop();

	virtual void pause();

	virtual void resume();
	////

private Q_SLOTS:

	void smsStateSlot(std::string phoneNumber, EnumSMSState::SMSState state);

private:

	/** This methods is replaced by addPhoneNumber. */
	virtual EnumSessionError::SessionError addContact(const Contact & contact);

	/** Cannot remove Contact in this kind of Session. */
	virtual void removeContact(const std::string & contactId);

	virtual bool canCreateISession(const Account & account,
		const ContactList & contactList) const;

	SMSSession(SMSSessionManager & smsSessionManager,
		CoIpManager & coIpManager);

	/** Running ISMSSessionPlugin. */
	ISMSSessionPlugin * _currentISMSSession;

};

#endif //OWSMSSESSION_H
