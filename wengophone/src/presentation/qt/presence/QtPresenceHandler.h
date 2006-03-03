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

#ifndef QTPRESENCEHANDLER_H
#define QTPRESENCEHANDLER_H

#include <presentation/PPresenceHandler.h>

class CPresenceHandler;
class IMContact;

/**
 *
 * @ingroup presentation
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class QtPresenceHandler : public PPresenceHandler {
public:
	QtPresenceHandler(CPresenceHandler & cPresenceHandler);

	void changeMyPresence(EnumPresenceState::PresenceState state,	
		const std::string & note, IMAccount * imAccount = NULL);

	void subscribeToPresenceOf(const IMContact & imContact);

	void blockContact(const IMContact & imContact);

	void unblockContact(const IMContact & imContact);

	void updatePresentation();

private:

	QtPresenceHandler();

	void presenceStateChangedEventHandler(PresenceHandler & sender,
		EnumPresenceState::PresenceState state, const std::string & note,
		const IMContact & imContact);

	void myPresenceStatusEventHandler(PresenceHandler & sender,
		const IMAccount & imAccount, EnumPresenceState::MyPresenceStatus status);

	void subscribeStatusEventHandler(PresenceHandler & sender,
		const IMContact & imContact, IMPresence::SubscribeStatus status);

	void updatePresentationThreadSafe();

	void initThreadSafe();

	CPresenceHandler & _cPresenceHandler;

};

#endif //QTPRESENCEHANDLER_H
