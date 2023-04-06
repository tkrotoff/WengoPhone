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

#include <NonCopyable.h>

class TimerTask;

/**
 * A facility for threads to schedule tasks for future execution in a background thread.
 *
 * Wrapper around QTimer from the Qt library without slot/signal.
 *
 * Example:
 * <pre>
 * class SayHelloTask : public TimerTask {
 * public:
 *     SayHelloTask(const std::string & helloMsg) {
 *         _helloMsg = helloMsg;
 *     }
 *     virtual void run() {
 *         std::cout << _helloMsg << endl;
 *     }
 * private:
 *     std::string _helloMsg;
 * };
 *
 * Timer * timer = new Timer(new SayHelloTask("Bonjour !"));
 * timer->start(1000);
 * </pre>
 *
 * @see QTimer
 * @see java.util.Timer
 * @see java.util.TimerTask
 * @author Tanguy Krotoff
 */
class Timer : NonCopyable {
public:

	/**
	 * Constructs a Timer.
	 *
	 * One task = one timer.
	 *
	 * @param task task to perform after a Timer timeout
	 */
	Timer(TimerTask * task);

	~Timer();

	/**
	 * Starts the specified task for execution at the specified time.
	 *
	 * @param timeout number of milliseconds timeout for the execution of the task
	 * @param nbShots number of task shots/retries, if 0 then no limit
	 */
	void start(unsigned int timeout, unsigned int nbShots = 0);

	/**
	 * Calls right now and only once the specified task.
	 *
	 * This is provided for convenience so that the user of this class
	 * does not have to keep a pointer on the TimerTask in order to call
	 * the method run(). This method will do it for him.
	 */
	void singleShotNow();

	/**
	 * Stops/Terminates the task.
	 */
	void stop();

	//Public because of moc
	class TimerPrivate;

private:

	TimerPrivate * _timer;
};


/**
 * A task that can be scheduled for one-time or repeated execution by a Timer.
 *
 * A TimerTask object is deleted by the Timer itself inside its destructor.
 *
 * @see java.util.TimerTask
 * @author Tanguy Krotoff
 */
class TimerTask : NonCopyable {
public:

	/**
	 * The action to be performed by this TimerTask.
	 */
	virtual void run() = 0;

protected:

	virtual ~TimerTask() { }
};

#endif	//TIMER_H
