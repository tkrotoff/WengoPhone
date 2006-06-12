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

#ifndef THREAD_H
#define THREAD_H

#include <util/Interface.h>
#include <util/Logger.h>
#include <thread/RecursiveMutex.h>
#include <thread/Mutex.h>
#include <boost/thread/detail/lock.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <vector>

/**
 * Event for Thread.
 *
 * Used when sending an event from another thread than the current one.
 *
 * @author Tanguy Krotoff
 */
class ThreadEvent {
public:

	virtual ~ThreadEvent() {
	}

	/** Calls the callback. */
	virtual void callback() = 0;
};

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
class Thread : Interface {
public:

	virtual ~Thread() {
		//FIXME Do not delete the boost::thread otherwise it crashes
		//delete _thread;
		_terminate = false;
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

	/**
	 * Adds a ThreadEvent to the thread main loop.
	 *
	 * The ThreadEvent will be executed in a threaded way inside the run() method.
	 * Warning: do not delete ThreadEvent, the method runEvents() will do it
	 * once the ThreadEvent has been executed.
	 *
	 * FIXME
	 *
	 * @param event to inject inside the thread main loop
	 */
	virtual void postEvent(ThreadEvent * event) {
		Mutex::ScopedLock ScopedLock(_mutex);
		_eventList.push_back(event);
		ScopedLock.unlock();
		_condition.notify_all();
	}

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
	 * @see sleep()
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

protected:

	/**
	 * Thread main loop.
	 *
	 * Starting the Thread using the method start() causes this method
	 * to be called in a separately executing Thread.
	 * Returning from this method will end the execution of the thread.
	 *
	 * Example of implementation:
	 * <pre>
	 * mutable Mutex _mutex;
	 * bool _terminate = false;
	 *
	 * void init() {
	 *     ...
	 * }
	 *
	 * void terminate() {
	 *     _terminate = true;
	 * }
	 *
	 * void run() {
	 *     init();
	 *
	 *     //Keeps the thread running until terminate() is called
	 *     while (!_terminate) {
	 *         runEvents();
	 *     }
	 * }
	 * </pre>
	 */
	virtual void run() = 0;

	/**
	 * Runs the events inserted inside the main thread loop via postEvent().
	 */
	virtual void runEvents() {
		Mutex::ScopedLock ScopedLock(_mutex);
		while (1){
			for (unsigned int i = 0; i < _eventList.size(); i++) {
				ScopedLock.unlock();
				_eventList[i]->callback();
				ScopedLock.lock();
				delete _eventList[i];
			}
			_eventList.clear();
			if (_terminate){
				return;
			}
			_condition.wait(ScopedLock);			
		}
	}
	/**
	 * Terminates the execution of the thread.
	 *
	 * Typical implementation:
	 * <pre>
	 * void terminate() {
	 *     _terminate = true;
	 * }
	 *
	 * void run() {
	 *     //Keeps the thread running until terminate() is called
	 *     while (!_terminate) {
	 *         ...
	 *     }
	 * }
	 * </pre>
	 */
	virtual void terminate() {
		Mutex::ScopedLock ScopedLock(_mutex);
		_terminate = true;
		ScopedLock.unlock();
		_condition.notify_all();
	}

	/** Defines the vector of ThreadEvent. */
	typedef std::vector < ThreadEvent * > Events;

	/** List of PostEvent. */
	Events _eventList;

	/**
	 * Mutex used for postEvent().
	 *
	 * FIXME
	 */
	// mutable RecursiveMutex _mutex;
	mutable Mutex _mutex;
	boost::condition _condition;

	/**
	 * If this thread should be terminate or not.
	 *
	 * By default _terminate == false.
	 * @see terminate()
	 */
	bool _terminate;

private:

	/** Boost thread. */
	boost::thread * _thread;
};

/**
 * ThreadEvent with 0 argument.
 *
 * The callback inside ThreadEvent has 0 argument.
 *
 * @author Tanguy Krotoff
 */
template<typename Signature>
class ThreadEvent0 : public ThreadEvent {
public:

	template<typename Callback>
	ThreadEvent0(const Callback & callback)
		: ThreadEvent(),
		_callback(callback) {
	}

	void callback() {
		_callback();
	}

private:

	/** Callback function. */
	boost::function<Signature> _callback;
};

/**
 * ThreadEvent with 1 argument.
 *
 * The callback inside ThreadEvent has 1 argument.
 *
 * @author Tanguy Krotoff
 */
template<typename Signature, typename Arg1>
class ThreadEvent1 : public ThreadEvent {
public:

	template<typename Callback>
	ThreadEvent1(const Callback & callback, const Arg1 & arg1)
		: ThreadEvent(),
		_callback(callback),
		_arg1(arg1) {
	}

	void callback() {
		_callback(_arg1);
	}

private:

	/** Callback function. */
	boost::function<Signature> _callback;

	Arg1 _arg1;
};

/**
 * ThreadEvent with 2 arguments.
 *
 * The callback inside ThreadEvent has 2 arguments.
 *
 * @author Tanguy Krotoff
 */
template<typename Signature, typename Arg1, typename Arg2>
class ThreadEvent2 : public ThreadEvent {
public:

	template<typename Callback>
	ThreadEvent2(const Callback & callback, const Arg1 & arg1, const Arg2 & arg2)
		: ThreadEvent(),
		_callback(callback),
		_arg1(arg1),
		_arg2(arg2) {
	}

	void callback() {
		_callback(_arg1, _arg2);
	}

private:

	/** Callback function. */
	boost::function<Signature> _callback;

	Arg1 _arg1;

	Arg2 _arg2;
};

/**
 * ThreadEvent with 3 arguments.
 *
 * The callback inside ThreadEvent has 3 arguments.
 *
 * @author Tanguy Krotoff
 */
template<typename Signature, typename Arg1, typename Arg2, typename Arg3>
class ThreadEvent3 : public ThreadEvent {
public:

	template<typename Callback>
	ThreadEvent3(const Callback & callback, const Arg1 & arg1, const Arg2 & arg2, const Arg3 & arg3)
		: ThreadEvent(),
		_callback(callback),
		_arg1(arg1),
		_arg2(arg2),
		_arg3(arg3) {
	}

	void callback() {
		_callback(_arg1, _arg2, _arg3);
	}

private:

	/** Callback function. */
	boost::function<Signature> _callback;

	Arg1 _arg1;

	Arg2 _arg2;

	Arg3 _arg3;
};

#endif	//THREAD_H
