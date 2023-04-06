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

#ifndef THREAD_H
#define THREAD_H

#include <NonCopyable.h>

#ifdef WIN32
#  pragma warning(push)
#  pragma warning(disable : 4251 4231 4660 4275)
#endif

#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/bind.hpp>

#ifdef WIN32
#  pragma warning(pop)
#endif

/**
 * Thread helper for boost.
 *
 * Example:
 * <pre>
 * class MyThread : public Thread {
 * protected:
 *     virtual void run() {
 *         std::cout << "Hello World!" << std::endl;
 *     }
 * };
 *
 * MyThread * myThread = new MyThread();
 * //start() calls MyThread::run() in a different thread
 * myThread->start();
 * </pre>
 *
 * @see boost::thread
 * @see ThreadPrivate
 * @see QThread
 * @see java.lang.Thread
 * @see java.lang.Runnable
 * @see java.lang.Object
 * @author Tanguy Krotoff
 */
class Thread : NonCopyable {
public:

	virtual ~Thread() {
		delete _thread;
	}

	/**
	 * Starts/creates the Thread with a given priority.
	 *
	 * Calls the pure virtual method run().
	 */
	void start() {
		_thread = new boost::thread(boost::bind(&Thread::run, this));
	}

	/**
	 * Blocks a Thread.
	 *
	 * The Thread will block until the thread associated with this
	 * Thread object has finished execution (i.e. when it returns from run()).
	 *
	 * This provides similar functionality to the POSIX pthread_join() function.
	 */
	void join() {
		_thread->join();
	}

protected:

	/**
	 * Run implementation of the Thread.
	 *
	 * Starting the Thread using the method start() causes this method
	 * to be called in a separately executing Thread.
	 */
	virtual void run() = 0;

	/**
	 * Causes the current thread to sleep.
	 *
	 * System independent sleep.
	 *
	 * @param seconds number of seconds the current thread should sleep
	 */
	static void sleep(unsigned long seconds) {
		if (seconds == 0) {
			boost::thread::yield();
			return;
		}

		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC);
		xt.sec += seconds;
		boost::thread::sleep(xt);
	}

	/**
	 * Causes the current thread to sleep.
	 *
	 * System independent sleep.
	 *
	 * @param milliseconds number of milliseconds the current thread should sleep
	 */
	static void msleep(unsigned long milliseconds) {
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

private:

	boost::thread * _thread;
};

#endif	//THREAD_H
