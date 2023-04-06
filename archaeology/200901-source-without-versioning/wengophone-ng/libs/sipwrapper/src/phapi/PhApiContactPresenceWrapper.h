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

#ifndef OWPHAPICONTACTPRESENCEWRAPPER_H
#define OWPHAPICONTACTPRESENCEWRAPPER_H

#include <phapiwrapperdll.h>

#include <sipwrapper/SipContactPresenceWrapper.h>

/**
 * PhApi implementation of SipWrapper.
 *
 * @author Philippe Bernery
 */
class PHAPIWRAPPER_API PhApiContactPresenceWrapper : public SipContactPresenceWrapper {
public:

	PhApiContactPresenceWrapper();

	~PhApiContactPresenceWrapper();

	virtual void blockContact(int lineId, const std::string & contactId);

	virtual void unblockContact(int lineId, const std::string & contactId);

	virtual void authorizeContact(int lineId, const std::string & contactId,
		bool auhorized, const std::string message);

	virtual void subscribeToPresenceOf(int lineId, const std::string & contactId);

	virtual void unsubscribeToPresenceOf(int lineId, const std::string & contactId);

};

#endif //OWPHAPICONTACTPRESENCEWRAPPER_H
