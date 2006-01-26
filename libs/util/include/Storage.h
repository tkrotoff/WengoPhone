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

#ifndef STORAGE_H
#define STORAGE_H

#include <Interface.h>

class Serializable;

/**
 * Storage interface that loads and saves data to a given format (File, RPC...).
 *
 * @author Tanguy Krotoff
 */
class Storage : Interface {
public:

	virtual ~Storage() {
	}

	/**
	 * Loads the datas from a media.
	 */
	virtual bool load(Serializable & serializable) = 0;

	/**
	 * Saves the datas to a media.
	 */
	virtual bool save(Serializable & serializable) = 0;
};

#endif //STORAGE_H
