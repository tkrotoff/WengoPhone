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

#ifndef IDGENERATOR_H
#define IDGENERATOR_H

#include <NonCopyable.h>

/**
 * Generated a unique ID based on the seconds since 1970.
 *
 * @see ctime
 * @author Tanguy Krotoff
 */
class IdGenerator : NonCopyable {
public:

	/**
	 * Generates a unique ID.
	 *
	 * @return unique ID generated
	 */
	static int generate();
};

#endif	//IDGENERATOR_H
