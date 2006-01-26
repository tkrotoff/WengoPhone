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

#ifndef CPRESENCE_H
#define CPRESENCE_H

#include <model/imwrapper/EnumPresenceState.h>

#include <NonCopyable.h>
#include <Event.h>

class CIMHandler;
class PPresence;
class Presence;
class IMPresence;

/**
 *
 * @ingroup control
 * @author Tanguy Krotoff
 */
class CPresence : NonCopyable {
public:

	CPresence(Presence & presence, CIMHandler & cIMHandler);

	/**
	 * @see IMPresence::presenceStateChangedEvent
	 */
	Event<void (IMPresence & sender, EnumPresenceState::PresenceState state, const std::string & note, const std::string & from)> presenceStateChangedEvent;

	/**
	 * @see IMPresence::presenceStatusEvent
	 */
	Event<void (IMPresence & sender, EnumPresenceState::MyPresenceStatus status)> myPresenceStatusEvent;

private:

	Presence & _presence;

	CIMHandler & _cIMHandler;

	PPresence * _pPresence;
};

#endif	//CPRESENCE_H
