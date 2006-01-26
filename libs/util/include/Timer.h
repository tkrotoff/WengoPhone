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

#ifndef TIMER_H
#define TIMER_H

#include <Event.h>
#include <Thread.h>

/**
 * A facility for threads to schedule tasks for future execution in a background thread.
 *
 * @see java.util.Timer
 * @see java.util.TimerTask
 * @see QTimer
 * @author Tanguy Krotoff
 */
class Timer : Thread {
public:

	/**
	 * Timer timeout occured.
	 *
	 * @param sender this class
	 */
	Event<void (Timer & sender)> timeoutEvent;

	/**
	 * Last timer timeout event.
	 *
	 * @param sender this class
	 */
	Event<void (Timer & sender)> lastTimeoutEvent;

	Timer();

	~Timer();

	/**
	 * Starts the specified task for execution at the specified time.
	 *
	 * @param firstTime first time in milliseconds the timer should start
	 * @param timeout number of milliseconds timeout for the execution of the task
	 * @param nbShots number of task shots/retries, if 0 then no limit
	 */
	void start(unsigned firstTime, unsigned timeout, unsigned nbShots = 0);

	/**
	 * Stops/Terminates the task.
	 */
	void stop();

private:

	void run();

	unsigned _nbRetry;

	unsigned _nbShots;

	unsigned _timeout;

	unsigned _firstTime;

	bool _stop;
};

#endif	//TIMER_H
