/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#ifndef PHAPIIMCONNECT_H
#define PHAPIIMCONNECT_H

#include "PhApiFactory.h"

#include <imwrapper/IMConnect.h>

#include <util/Trackable.h>

class IMAccount;

/**
 * SIP Instant Messaging connection.
 *
 * @ingroup model
 * @author Mathieu Stute
 */
class PhApiIMConnect : public IMConnect, public Trackable {

	friend class PhApiFactory;

public:

	void connect();

	void disconnect();

private:

	PhApiIMConnect(IMAccount & account, PhApiWrapper & phApiWrapper);

	~PhApiIMConnect();

	void connectedEventHandler(PhApiWrapper & sender);

	void disconnectedEventHandler(PhApiWrapper & sender, bool connectionError, const std::string & reason);

	void connectionProgressEventHandler(PhApiWrapper & sender, int currentStep, int totalSteps, const std::string & infoMessage);

	PhApiWrapper & _phApiWrapper;
};

#endif	//PHAPIIMCONNECT_H
