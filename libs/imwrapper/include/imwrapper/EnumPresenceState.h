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

#ifndef ENUMPRESENCESTATE_H
#define ENUMPRESENCESTATE_H

#include <util/NonCopyable.h>

#include <string>

/**
 * Presence states (online, offline, away...).
 *
 * @author Philippe Bernery
 */
class EnumPresenceState : NonCopyable {
public:

	enum PresenceState {
		/** Buddy online. */
		PresenceStateOnline,

		/** Buddy offline. */
		PresenceStateOffline,

		/** User invisible. */
		PresenceStateInvisible,

		/** Buddy away. */
		PresenceStateAway,

		/** Buddy do not disturb. */
		PresenceStateDoNotDisturb,

		/** Buddy user defined status by a simple string ("I'm on the phone", "Under the shower"...). */
		PresenceStateUserDefined,

		/** Forward call to phone. */
		PresenceStateForward,

		/** Not used for the moment. */
		PresenceStateUnknown
	};

	enum MyPresenceStatus {
		/** My presence has been changed successfully. */
		MyPresenceStatusOk,

		/** An eror occured while changing my presence. */
		MyPresenceStatusError
	};

	/**
	 * Get a string representation of the presence state.
	 */
	static std::string toString(PresenceState presenceState);

	/**
	 * Get a presence state from a string representation.
	 */
	static PresenceState fromString(const std::string & presenceState);

};

#endif	//ENUMPRESENCESTATE_H
