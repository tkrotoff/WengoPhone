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

#ifndef __EVENT_H
#define __EVENT_H

#include <NonCopyable.h>

#include <boost/signal.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

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
	 * @param slot to connect
	 */
	template<typename T>
	void operator+=(const T & slot) {
		connect(slot);
	}

private:
};

#endif	//__EVENT_H
