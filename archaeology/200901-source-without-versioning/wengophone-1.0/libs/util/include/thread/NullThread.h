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

#ifndef NULLTHREAD_H
#define NULLTREAD_H

#include <thread/Thread.h>

/**
 * Empty (null) Thread implementation.
 *
 * @see Thread
 * @author Tanguy Krotoff
 */
class NullThread : public IThread {
public:

	/**
	 * Constructs a new QtThread.
	 *
	 * @param thread callback for the run() method
	 */
	NullThread(Thread * thread);

	virtual ~NullThread();

	virtual void start(Priority priority);

	virtual void terminate();

	virtual bool wait(unsigned long timeout);

	virtual bool isAlive() const;

protected:

	virtual void run();

private:

	/**
	 * Callback for the run() method.
	 *
	 * NullThread::run() calls _thread->run().
	 */
	Thread * _thread;
};

#endif	//NULLTREAD_H
