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

#ifndef MUTEX_H
#define MUTEX_H

#ifdef WIN32
#  pragma warning(push)
#  pragma warning(disable : 4251 4231 4660 4275)
#endif

#include <boost/thread/mutex.hpp>

#ifdef WIN32
#  pragma warning(pop)
#endif

/**
 * Mutex helper for boost.
 *
 * Example:
 * <pre>
 * class Counter {
 * public:
 *
 *     Counter() : _counter(0) { }
 *
 *     void increment() {
 *         Mutex::ScopedLock ScopedLock(_mutex);
 *         _counter++;
 *     }
 *
 *     void decrement() {
 *         Mutex::ScopedLock ScopedLock(_mutex);
 *         _counter--;
 *     }
 *
 *     int getValue() const {
 *         Mutex::ScopedLock ScopedLock(_mutex);
 *         return _counter;
 *     }
 *
 * private:
 *     mutable Mutex _mutex;
 *     int _counter;
 * };
 * <pre>
 *
 * @author Tanguy Krotoff
 */
class Mutex : public boost::mutex {
public:

	typedef scoped_lock ScopedLock;
};

#endif	//MUTEX_H
