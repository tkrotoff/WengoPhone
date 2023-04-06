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

#ifndef SUBJECT_H
#define SUBJECT_H

#include <NonCopyable.h>

#include <string>
#include <vector>

class Observer;
class Event;

/**
 * Design Pattern Observer.
 *
 * @author Tanguy Krotoff
 */
class Subject : NonCopyable {
public:

	virtual ~Subject();

	/**
	 * Adds an Observer to the list of Observer.
	 *
	 * @param observer Observer that will observe this object
	 */
	virtual void addObserver(Observer & observer);

	/**
	 * Removes an Observer from the list of Observer.
	 *
	 * @param observer Observer to be removed
	 * @return if the Observer has been removed from the list
	 */
	virtual bool removeObserver(Observer & observer);

	/**
	 * Notifies all the Observer from the list that the Subject has been changed.
	 *
	 * @param event Event that happened
	 */
	virtual void notify(const Event & event);

private:

	/**
	 * Type list of Observer.
	 */
	typedef std::vector<Observer *> ObserverList;

	/**
	 * The vector/collection/list of Observer that maintains this class.
	 */
	ObserverList _observerList;
};

#endif	//SUBJECT_H
