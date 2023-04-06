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

#ifndef QTTHREADFACTORY_H
#define QTTHREADFACTORY_H

#include <thread/ThreadFactory.h>
#include <thread/QtThread.h>

/**
 * Creates QtThread.
 *
 * @author Tanguy Krotoff
 */
class QtThreadFactory : public ThreadFactory {
public:

	/**
	 * Constructs a new ThreadFactory.
	 */
	QtThreadFactory() { }

	virtual ~QtThreadFactory() { }

	virtual IThread * create(Thread * thread) const {
		return new QtThread(thread);
	}
};

#endif	//QTTHREADFACTORY_H
