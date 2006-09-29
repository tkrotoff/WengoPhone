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

#include <thread/Thread.h>

#include <thread/ThreadEvent.h>

#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

Thread::Events Thread::_eventList;
Mutex Thread::_mutex;
Condition Thread::_condition;

Thread::~Thread() {
	//FIXME Do not delete the boost::thread otherwise it crashes
	//delete _thread;
}

void Thread::start() {
	_terminate = false;
	_thread = new boost::thread(boost::bind(&Thread::run, this));
}

void Thread::join() {
	_thread->join();
}

void Thread::postEvent(IThreadEvent * event) {
	Mutex::ScopedLock ScopedLock(_mutex);
	_eventList.push_back(event);
	ScopedLock.unlock();

	_condition.notify_all();
}

void Thread::sleep(unsigned long seconds) {
	if (seconds == 0) {
		boost::thread::yield();
		return;
	}

	boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);
	xt.sec += seconds;
	boost::thread::sleep(xt);
}

void Thread::msleep(unsigned long milliseconds) {
	static const unsigned int MILLISECONDS_PER_SECOND = 1000;
	static const unsigned int NANOSECONDS_PER_MILLISECOND = 1000000;

	if (milliseconds == 0) {
		boost::thread::yield();
		return;
	}

	unsigned int sec = 0;
	//If larger than 1 second, do some voodoo for the boost::xtime struct
	if (milliseconds >= MILLISECONDS_PER_SECOND) {
		//Converts ms > 1000 into secs + remaining ms
		unsigned int secs = milliseconds / MILLISECONDS_PER_SECOND;
		milliseconds = milliseconds - secs * MILLISECONDS_PER_SECOND;
		sec += secs;
	}
	milliseconds *= NANOSECONDS_PER_MILLISECOND;

	boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);
	xt.nsec += milliseconds;
	xt.sec += sec;
	boost::thread::sleep(xt);
}

void Thread::runEvents() {
	Mutex::ScopedLock ScopedLock(_mutex);
	while (true) {
		for (unsigned i = 0; i < _eventList.size(); i++) {
			ScopedLock.unlock();
			_eventList[i]->callback();
			ScopedLock.lock();
			delete _eventList[i];
		}
		_eventList.clear();
		if (_terminate) {
			return;
		}
		_condition.wait(ScopedLock);
	}
}

void Thread::terminate() {
	Mutex::ScopedLock ScopedLock(_mutex);
	_terminate = true;
	ScopedLock.unlock();

	_condition.notify_all();
}

