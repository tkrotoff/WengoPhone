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

#ifndef OWPHAPIFORWENGOCONNECTINGSTATE_H
#define OWPHAPIFORWENGOCONNECTINGSTATE_H

#include "../SipWrapperBasedConnectingState.h"

#include <ssorequest/SSORequest.h>

#include <QtCore/QObject>

/**
 * Wengo account connection state.
 *
 * - connect does nothing.
 * - disconnect does nothing.
 * - execute connects the Account.
 *
 * @author Philippe Bernery
 */
class PhApiForWengoConnectingState : public SipWrapperBasedConnectingState {
	Q_OBJECT
public:

	PhApiForWengoConnectingState(IConnectPlugin & context, SipWrapper & sipWrapper);

	virtual void execute();

protected Q_SLOTS:

	virtual void connectionProcess();

private:

	/** Do SSO request. */
	SSORequest::SSORequestError doSsoRequest();

	/** Resets attempt counters. */
	virtual void resetAttemptCounter();

	/** Number of connection attempt. */
	unsigned _udpConnectionAttempt, _http80ConnectionAttempt,
		_http443ConnectionAttempt, _https443ConnectionAttempt;

	SipWrapper * _sipWrapper;

};

#endif //OWPHAPIFORWENGOCONNECTINGSTATE_H
