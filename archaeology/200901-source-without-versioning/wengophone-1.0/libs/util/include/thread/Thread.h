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

#include <climits>

class ThreadFactory;

/**
 * Thread class wrapper/interface.
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
 * @see ThreadPrivate
 * @see QThread
 * @see java.lang.Thread
 * @see java.lang.Runnable
 * @see java.lang.Object
 * @author Tanguy Krotoff
 * @stereotype interface
 */
class IThread : NonCopyable {
public:

	enum Priority {
		/** Scheduled only when no other threads are running. */
		IdlePriority,

		/** Scheduled less often than LowPriority. */
		LowestPriority,

		/** Scheduled less often than LowPriority. */
		LowPriority,

		/** The default priority of the operating system. */
		NormalPriority,

		/** Scheduled more often than NormalPriority. */
		HighPriority,

		/** Scheduled more often then HighPriority. */
		HighestPriority,

		/** Scheduled as often as possible. */
		TimeCriticalPriority,

		/** Use the same priority as the creating thread (this is the default). */
		InheritPriority
	};

	/**
	 * Thread destructor.
	 *
	 * Note that deleting a Thread object will not stop the execution of
	 * the thread it represents. Deleting a running Thread (i.e.
	 * isRunning() returns true) will probably result in a program crash.
	 * You can wait() on a thread to make sure that it has finished.
	 */
	virtual ~IThread() { }

	/**
	 * Starts/creates the Thread with a given priority.
	 *
	 * Calls the pure virtual method run().
	 *
	 * @param priority how the Operating System should schedule the Thread
	 */
	virtual void start(Priority priority = InheritPriority) = 0;

	/**
	 * Terminates the execution of the thread.
	 *
	 * The thread may or may not be terminated immediately,
	 * depending on the operating system's scheduling policies.
	 * Use Thread::wait() after terminate() for synchronous termination.
	 *
	 * When the thread is terminated, all threads waiting for the
	 * the thread to finish will be woken up.
	 *
	 * This function is dangerous, and its use is discouraged.
	 * The thread can be terminated at any point in its code path. Threads
	 * can be terminated while modifying data. There is no chance for
	 * the thread to cleanup after itself, unlock any held mutexes, etc.
	 * In short, use this function only if absolutely necessary.
	 */
	virtual void terminate() = 0;

	/**
	 * Blocks a Thread.
	 *
	 * The Thread will block until either of these conditions is met:
	 * - The thread associated with this Thread object has finished execution
	 * (i.e. when it returns from run()).
	 * - timeout milliseconds has elapsed.
	 *
	 * This provides similar functionality to the POSIX pthread_join() function.
	 *
	 * @param timeout stops the Thread during timeout milliseconds,
	 *        if timeout equals ULONG_MAX Thread will never time out
	 * @return true if the thread has finished or the thread has not been started yet;
	 *         false if the wait time out
	 */
	virtual bool wait(unsigned long timeout = ULONG_MAX) = 0;

	/**
	 * Tests if this thread is alive.
	 *
	 * A thread is alive if it has been started and has not yet died.
	 *
	 * @return true if this thread is alive; false otherwise
	 */
	virtual bool isAlive() const = 0;

protected:

	/**
	 * Run implementation of the Thread.
	 *
	 * Starting the Thread using the method start() causes this method
	 * to be called in a separately executing Thread.
	 */
	virtual void run() = 0;
};


/**
 * Thread implementation.
 *
 * Thread implementation is inside the class ThreadPrivate.
 *
 * @see IThread
 * @author Tanguy Krotoff
 */
class Thread : public IThread {
public:

	static void setFactory(ThreadFactory * factory);

	/**
	 * Constructs a new Thread.
	 *
	 * The Thread does not begin executing until start() is called.
	 */
	Thread();

	virtual ~Thread();

	virtual void start(Priority priority = InheritPriority);

	virtual void terminate();

	virtual bool wait(unsigned long timeout = ULONG_MAX);

	virtual bool isAlive() const;

	virtual void run() = 0;

private:

	static ThreadFactory * _factory;

	/**
	 * System-dependant implementation.
	 */
	IThread * _threadPrivate;
};

#endif	//THREAD_H
