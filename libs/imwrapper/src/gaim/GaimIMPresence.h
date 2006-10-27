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

#ifndef GAIMIMPRESENCE_H
#define GAIMIMPRESENCE_H

//#include "GaimIMFactory.h"

#include <imwrapper/IMAccount.h>
#include <imwrapper/IMPresence.h>
#include <imwrapper/EnumPresenceState.h>

#include "GaimEnumIMProtocol.h"

/**
 * Stub for Instant Messaging presence.
 *
 * @ingroup model
 * @author Julien Bossart
 */
class GaimIMPresence : public IMPresence
{
	friend class GaimIMFactory;

public:

	GaimIMPresence(IMAccount & account);

	virtual void changeMyPresence(EnumPresenceState::PresenceState state, const std::string & note);
	virtual void changeMyAlias(const std::string & nickname);
	virtual void changeMyIcon(const OWPicture & picture);
	virtual void subscribeToPresenceOf(const std::string & contactId);
	virtual void unsubscribeToPresenceOf(const std::string & contactId);
	virtual void blockContact(const std::string & contactId);
	virtual void unblockContact(const std::string & contactId);
	virtual void authorizeContact(const std::string & contactId, bool authorized, const std::string message);

	bool equalsTo(std::string login, EnumIMProtocol::IMProtocol protocol);

private:


};

#endif	//GAIMIMPRESENCE_H
