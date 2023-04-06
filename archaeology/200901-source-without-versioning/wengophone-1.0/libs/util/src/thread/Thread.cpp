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

#include <thread/Thread.h>

#include <thread/DefaultThreadFactory.h>

ThreadFactory * Thread::_factory = NULL;

void Thread::setFactory(ThreadFactory * factory) {
	_factory = factory;
}

Thread::Thread() {
	if (!_factory) {
		_factory = new DefaultThreadFactory();
	}
	_threadPrivate = _factory->create(this);
}

Thread::~Thread() {
	delete _threadPrivate;
}

void Thread::start(Priority priority) {
	_threadPrivate->start(priority);
}

void Thread::terminate() {
	_threadPrivate->terminate();
}

bool Thread::wait(unsigned long timeout) {
	return _threadPrivate->wait(timeout);
}

bool Thread::isAlive() const {
	return _threadPrivate->isAlive();
}
