/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

/**
* @file AudioCallEvent.h
* @author Philippe KAJMAR
* @brief decalaration of AudioCallEvent class
*/

#ifndef AUDIOCALLEVENT_H
# define AUDIOCALLEVENT_H

#include <observer/Event.h>

/**
* @brief notify events from AudioCall class
*/ 
class AudioCallEvent : public Event {
public:
	enum Kind {
		no_kind,
		CallIdChanged,
		local_hold,
		call_action,
		hang_up_action,
		use
	};

	/// construct a register event
	AudioCallEvent(Kind kind);

	/// get kind
	Kind getKind() const;

private:

	/// kind of AudioCall Event
	Kind _kind;
};

#endif	//AUDIOCALLEVENT_H
