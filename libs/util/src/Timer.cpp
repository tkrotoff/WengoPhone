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

#include <Timer.h>

Timer::Timer() {
	_firstTime = 0;
	_timeout = 0;
	_nbShots = 0;

	_stop = false;
	_nbRetry = 0;
}

Timer::~Timer() {
	_stop = true;
}

void Timer::start(unsigned firstTime, unsigned timeout, unsigned nbShots) {
	_firstTime = firstTime;
	_timeout = timeout;
	_nbShots = nbShots;

	_stop = false;
	_nbRetry = 0;

	Thread::start();
}

void Timer::stop() {
	_stop = true;
}

void Timer::run() {
	msleep(_firstTime);
	while (((_nbShots == 0) || (_nbRetry < _nbShots)) && !_stop) {
		_nbRetry++;
		if (_nbRetry == _nbShots) {
			lastTimeoutEvent(*this);
		} else {
			timeoutEvent(*this);
		}
		msleep(_timeout);
	}
}

void Timer::join() {
	Thread::join();
}
