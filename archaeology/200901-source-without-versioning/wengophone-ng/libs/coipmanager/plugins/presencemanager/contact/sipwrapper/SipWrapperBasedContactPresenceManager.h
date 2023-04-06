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

#ifndef OWSIPWRAPPERBASEDCONTACTPRESENCEMANAGER_H
#define OWSIPWRAPPERBASEDCONTACTPRESENCEMANAGER_H

#include <coipmanager/presencemanager/contact/IContactPresenceManagerPlugin.h>

#include <coipmanager_base/EnumPresenceState.h>

class SipWrapper;

/**
 * SipWrapper implementation of Contact Presence Management.
 *
 * @author Philippe Bernery
 */
class SipWrapperBasedContactPresenceManager : public IContactPresenceManagerPlugin {
	Q_OBJECT
public:

	SipWrapperBasedContactPresenceManager(CoIpManager & coIpManager);

	virtual ~SipWrapperBasedContactPresenceManager();

	void setSipWrapper(SipWrapper *sipWrapper);

	virtual void blockContact(const IMContact & imContact);

	virtual void unblockContact(const IMContact & imContact);

	virtual void authorizeContact(const IMContact & imContact,
		bool authorized, const std::string & message);

	virtual void imContactAdded(const IMContact & imContact);

	virtual void imContactRemoved(const IMContact & imContact);

protected:

	SipWrapper *_sipWrapper;

private Q_SLOTS:

	void accountConnectionStateSlot(std::string accountId,
		EnumConnectionState::ConnectionState state);

	void presenceStateChangedSlot(EnumPresenceState::PresenceState state,
		const std::string & alias, const std::string & from);

};

#endif //OWSIPWRAPPERBASEDCONTACTPRESENCEMANAGER_H
