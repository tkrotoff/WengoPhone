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

#include <NonCopyable.h>

#ifdef WIN32
#  pragma warning(push)
#  pragma warning(disable : 4251 4231 4660 4275)
#endif

#include <boost/function.hpp>
#include <boost/bind.hpp>

#ifdef WIN32
#  pragma warning(pop)
#endif

#include <list>

/**
 * Design Pattern Observer.
 *
 * This class helps to implement the subject part of the observer design pattern.
 *
 * Uses boost::function in order to simplify the use of the pattern.
 * Works a bit like the C# observer pattern that uses delegates and events.
 *
 * Same syntax as boost::signal. boost::signal cannot be used directly since
 * there is an incompatible issue with the Qt library version 3.3.4.
 *
 * Example (the boost::signal syntax is commented so that one can compare both):
 * <pre>
 * void showTime(const std::string & time) {
 *     std::cout << time << std::endl;
 * }
 *
 * class Clock {
 * public:
 *     void showTime(const std::string & time) const {
 *         std::cout << "Clock: " << time << std::endl;
 *     }
 * };
 *
 * class MyTimer {
 * public:
 *
 *     Event<void (const std::string &)> secondEvent;
 *     //boost::signal<void (const std::string &)> secondEvent;
 *
 *     Event<void (const std::string &)> minuteEvent;
 *     //boost::signal<void (const std::string &)> minuteEvent;
 *
 *     void tick() {
 *         secondEvent("Second Event");
 *         minuteEvent("Minute Event");
 *     }
 * };
 *
 * MyTimer timer;
 * Clock clock;
 *
 * timer.secondEvent += &showTime;
 * //timer.secondEvent.connect(&showTime);
 * timer.secondEvent += boost::bind(&Clock::showTime, &clock, _1);
 * //timer.secondEvent.connect(boost::bind(&Clock::showTime, &clock, _1);
 * timer.minuteEvent += boost::bind(&Clock::showTime, &clock, _1);
 * //timer.minuteEvent.connect(boost::bind(&Clock::showTime, &clock, _1);
 * timer.tick();
 *    //Print: Second Event
 *    //       Clock: Second Event
 *    //       Clock: Minute Event
 *
 * timer.minuteEvent -= boost::bind(&Clock::showTime, &clock, _1);
 * //timer.minuteEvent.disconnect(boost::bind(&Clock::showTime, &clock, _1);
 * timer.tick();
 *    //Print: Second Event
 *    //       Clock: Second Event
 * </pre>
 *
 * @author Tanguy Krotoff
 */
template<typename Signature>
class Event : NonCopyable {
public:

	~Event() {
		for (ObserverList::iterator it = _observerList.begin(); 
			it != _observerList.end(); ++it) {
				operator-=(it);
		}
		_observerList.clear();
	}

	/**
	 * Adds/attaches an observer to the list of observer.
	 *
	 * Two identical observers cannot be attached, only one will be:
	 * checks first if the same observer was not attached already.
	 *
	 * Equivalent to boost::signal::connect()
	 *
	 * @param observer callback function that will observe this object
	 */
	template<typename Observer>
	void operator+=(const Observer & observer) {
		operator-=(observer);
		_observerList.push_back(observer);
	}

	/**
	 * Removes/detaches an observer from the list of observer.
	 *
	 * Equivalent to boost::signal::disconnect()
	 *
	 * @param observer callback function to be removed
	 * @return if the observer has been removed from the list
	 */
	template<typename Observer>
	bool operator-=(const Observer & observer) {
		typename ObserverList::iterator pos = std::find(_observerList.begin(), _observerList.end(), observer);
		if (pos != _observerList.end()) {
			_observerList.erase(pos);
			return true;
		}
		return false;
	}

	/**
	 * Notifies all the observer from the list that the Subject has been changed.
	 */
	void operator()() const {
		for (ObserverList::const_iterator it = _observerList.begin(); 
			it != _observerList.end(); ++it) {
				(*it)();
		}
	}

	template<typename Arg1>
	void operator()(const Arg1 & arg1) const {
		for (ObserverList::const_iterator it = _observerList.begin(); 
			it != _observerList.end(); ++it) {
				(*it)(arg1);
		}
	}

	template<typename Arg1, typename Arg2>
	void operator()(const Arg1 & arg1, const Arg2 & arg2) const {
		for (ObserverList::const_iterator it = _observerList.begin(); 
			it != _observerList.end(); ++it) {
				(*it)(arg1, arg2);
		}
	}

	template<typename Arg1, typename Arg2, typename Arg3>
	void operator()(const Arg1 & arg1, const Arg2 & arg2, const Arg3 & arg3) const {
		for (ObserverList::const_iterator it = _observerList.begin(); 
			it != _observerList.end(); ++it) {
				(*it)(arg1, arg2, arg3);
		}
	}

	template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void operator()(const Arg1 & arg1, const Arg2 & arg2, const Arg3 & arg3, const Arg4 & arg4) const {
		for (ObserverList::const_iterator it = _observerList.begin(); 
			it != _observerList.end(); ++it) {
				(*it)(arg1, arg2, arg3, arg4);
		}
	}

private:

	/**
	 * Type list of observer.
	 */
	typedef std::list<boost::function<Signature> > ObserverList;

	/**
	 * The vector/collection/list of observer.
	 */
	ObserverList _observerList;
};

#endif	//EVENT_H
