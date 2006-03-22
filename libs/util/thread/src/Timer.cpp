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

#include <thread/Timer.h>

#include <thread/Thread.h>

class PrivateThread : Thread {
public:

	Event<void (PrivateThread & thread)> timeoutEvent;

	Event<void (PrivateThread & thread)> lastTimeoutEvent;

	PrivateThread();

	~PrivateThread();

	void start(unsigned firstTime, unsigned timeout, unsigned nbShots);

	void stop();

	bool isStopped() const {
		return _stop;
	}

private:

	/** @see Thread::start() */
	void run();

	/** Number of retries currenlty done, _nbRetry is always <= _nbShots. */
	unsigned _nbRetry;

	/** @see start() */
	unsigned _nbShots;

	/** @see start() */
	unsigned _timeout;

	/** @see start() */
	unsigned _firstTime;

	/** Stops or not the timer. */
	bool _stop;
};

PrivateThread::PrivateThread() {
	_firstTime = 0;
	_timeout = 0;
	_nbShots = 0;

	_stop = false;
	_nbRetry = 0;
}

PrivateThread::~PrivateThread() {
	stop();
}

void PrivateThread::start(unsigned firstTime, unsigned timeout, unsigned nbShots) {
	_firstTime = firstTime;
	_timeout = timeout;
	_nbShots = nbShots;

	_stop = false;
	_nbRetry = 0;

	Thread::start();
}

void PrivateThread::stop() {
	_stop = true;
}

void PrivateThread::run() {
	msleep(_firstTime);

	while ((_nbShots == 0) || (_nbRetry < _nbShots)) {
		_nbRetry++;

		//If timer has been stopped
		if (_stop) {
			return;
		}

		if (_nbRetry == _nbShots) {
			lastTimeoutEvent(*this);
		} else {
			timeoutEvent(*this);
		}
		msleep(_timeout);
	}
}


Timer::Timer() {
}

Timer::~Timer() {
	stop();
	for (register unsigned i = 0; i != _threadList.size(); i++) {
		delete _threadList[i];
	}
	_threadList.clear();
}

void Timer::start(unsigned firstTime, unsigned timeout, unsigned nbShots) {
	PrivateThread * thread = new PrivateThread();
	thread->timeoutEvent += boost::bind(&Timer::timeoutEventHandler, this, _1);
	thread->lastTimeoutEvent += boost::bind(&Timer::lastTimeoutEventHandler, this, _1);
	thread->start(firstTime, timeout, nbShots);
	_threadList += thread;
}

void Timer::stop() {
	for (register unsigned i = 0; i != _threadList.size(); i++) {
		_threadList[i]->stop();
	}
}

void Timer::timeoutEventHandler(PrivateThread & thread) {
	if (!thread.isStopped()) {
		timeoutEvent(*this);
	}
}

void Timer::lastTimeoutEventHandler(PrivateThread & thread) {
	if (!thread.isStopped()) {
		lastTimeoutEvent(*this);
	}
}
