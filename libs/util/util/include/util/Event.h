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

#ifndef OWEVENT_H
#define OWEVENT_H

#include <util/NonCopyable.h>

#include <boost/signal.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <list>

/**
 * Delegates/Design Pattern Observer.
 *
 * This class helps to implement the subject part of the observer design pattern.
 *
 * Uses boost::function in order to simplify the use of the pattern.
 * Works a bit like the C# observer pattern that uses delegates and events.
 *
 * Same syntax as boost::signal. boost::signal cannot be used directly since
 * there is an incompatible issue with the Qt library version 3.3.4.
 *
 * You can also connect an Event to another.
 *
 * Example (the boost::signal syntax is commented so that one can compare both):
 * <pre>
 * void showTime(MyTimer * sender, const std::string & time) {
 *     std::cout << time << std::endl;
 * }
 *
 * class Clock {
 * public:
 *     void showTime(MyTimer * sender, const std::string & time) const {
 *         std::cout << "Clock: " << time << std::endl;
 *     }
 * };
 *
 * class MyTimer {
 * public:
 *
 *     Event<void (MyTimer *, const std::string &)> secondEvent;
 *     //boost::signal<void (const std::string &)> secondEvent;
 *
 *     Event<void (MyTimer *, const std::string &)> minuteEvent;
 *     //boost::signal<void (const std::string &)> minuteEvent;
 *
 *     void tick() {
 *         //Sends the signal, updates the observers
 *         secondEvent(this, "Second Event");
 *         minuteEvent(this, "Minute Event");
 *     }
 * };
 *
 * MyTimer timer;
 * Clock clock;
 *
 * timer.secondEvent += &showTime;
 * //timer.secondEvent.connect(&showTime);
 * timer.secondEvent += boost::bind(&Clock::showTime, &clock, _1, _2);
 * //timer.secondEvent.connect(boost::bind(&Clock::showTime, &clock, _1);
 * timer.minuteEvent += boost::bind(&Clock::showTime, &clock, _1, _2);
 * //timer.minuteEvent.connect(boost::bind(&Clock::showTime, &clock, _1);
 * timer.tick();
 *    //Print: Second Event
 *    //       Clock: Second Event
 *    //       Clock: Minute Event
 *
 * timer.minuteEvent -= boost::bind(&Clock::showTime, &clock, _1, _2);
 * //timer.minuteEvent.disconnect(boost::bind(&Clock::showTime, &clock, _1));
 * timer.tick();
 *    //Print: Second Event
 *    //       Clock: Second Event
 * </pre>
 *
 * A good pratice is to always have the sender as the first parameter of the Event.
 *
 * @author Tanguy Krotoff
 */
template<typename Signature>
class Event : NonCopyable, public boost::signal<Signature> {
public:

	/**
	 * Connects a slot to this signal.
	 *
	 * Provides unicity when connecting a slot to a signal.
	 * Two identical slots cannot be connected, only one will be:
	 * this method checks first if the same slot was not connected already.
	 *
	 * @param slot callback function
	 * @return connection
	 */
	template<typename Slot>
	boost::signals::connection operator+=(const Slot & slot) {
		boost::signals::connection c;
		if (!alreadyConnected(slot)) {
			//The slot is not connected to the signal
			_slotList.push_back(slot);
			c = connect(slot);
		}
		//The slot is already connected to the signal
		return c;
	}

	/**
	 * Connects a signal to another signal.
	 *
	 * @param event signal to connect
	 * @return connection
	 */
	boost::signals::connection operator+=(const Event & event) {
		return connect(event);
	}

private:

	/**
	 * Checks if a slot is already present inside the slot list.
	 *
	 * @param observer callback function
	 * @return true if the slot is present inside the slot list; false otherwise
	 */
	template<typename Slot>
	bool alreadyConnected(const Slot & slot) {
		typename SlotList::iterator it = std::find(_slotList.begin(), _slotList.end(), slot);
		if (it != _slotList.end()) {
			return true;
		}
		return false;
	}

	/** Type list of slot. */
	typedef std::list<boost::function<Signature> > SlotList;

	/**
	 * The vector/collection/list of slot.
	 *
	 * Permits to provide unicity when connecting a slot to a signal.
	 */
	SlotList _slotList;
};

#endif	//OWEVENT_H
