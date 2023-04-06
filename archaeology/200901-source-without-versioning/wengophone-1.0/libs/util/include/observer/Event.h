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

#ifndef EVENT_H
#define EVENT_H

#include <string>

/**
 * Event in the case of the design pattern Observer.
 *
 * @see Observer
 * @see Subject
 *
 * @author Tanguy Krotoff
 */
class Event {
public:

	Event() {
	}

	/**
	 * Constructs an Event.
	 *
	 * @param typeEvent string that gives the name of this Event
	 */
	Event(const std::string & typeEvent) {
		_typeEvent = typeEvent;
	}

	/**
	 * Gets the type of Event.
	 *
	 * @return the type of Event
	 */
	virtual const std::string & getTypeEvent() const {
		return _typeEvent;
	}

	virtual ~Event() {
	}

private:

	//Event(const Event &);
	Event & operator=(const Event &);

	/**
	 * String that gives the type of Event.
	 */
	std::string _typeEvent;
};

#endif	//EVENT_H
