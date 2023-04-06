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

#ifndef OWCALLSESSIONMANAGER_H
#define OWCALLSESSIONMANAGER_H

#include "ICallSessionManagerPlugin.h"

#include <coipmanager/ICoIpSessionManager.h>

#include <sipwrapper/EnumPhoneCallState.h>

class Account;
class CallSession;
class ICallSession;
class ICallSessionPlugin;
class ICallSessionManagerPlugin;

/**
 * Call Session Manager.
 *
 * Used to manage classes related to Call.
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_API CallSessionManager : public ICoIpSessionManager {
	Q_OBJECT
	friend class CallSession;
public:

	CallSessionManager(CoIpManager & coIpManager);

	virtual ~CallSessionManager();

	/**
	 * Creates a CallSession.
	 *
	 * Caller is responsible for deleting the Session.
	 */
	CallSession * createCallSession();

	virtual ICallSessionManagerPlugin * getICoIpManagerPlugin(const Account & account, const ContactList & contactList) const;

Q_SIGNALS:

	/**
	 * Emitted when a new CallSession is created from outside.
	 *
	 * @param callSession pointer to the newly created CallSession,
	 *        this CallSession must be deleted by the receiver of the Event
	 */
	void newCallSessionCreatedSignal(CallSession * callSession);

private Q_SLOTS:

	/**
	 * @see ICallSessionManager::newICallSessionCreatedSignal()
	 */
	void newICallSessionPluginCreatedSlot(ICallSessionPlugin * iCallSessionPlugin);

	/**
	 * @see CallSession::phoneCallStateChangedSignal()
	 */
	void phoneCallStateChangedSlot(EnumPhoneCallState::PhoneCallState state);

private:

	virtual void initialize();

	virtual void uninitialize();

};

#endif //OWCALLSESSIONMANAGER_H
