/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWIDENTIFIABLE_H
#define OWIDENTIFIABLE_H

#include <util/owutildll.h>
#include <util/Cloneable.h>

#include <string>

/**
 * This has to be renamed to something else than Identifiable,
 * for example: Identifiable ;) or class Unique??
 *
 * An Identifiable object is uniquely identifiable via
 * its UUID.
 * Copying such an object will copy the UUID.
 * Comparing two Identifiable objects will compare the UUID.
 *
 * @author Philippe Bernery
 */
class Identifiable : public Cloneable {
public:

	OWUTIL_API Identifiable();

	OWUTIL_API Identifiable(const Identifiable & identifiable);

	OWUTIL_API virtual Identifiable * clone() const;

	OWUTIL_API virtual bool operator==(const Identifiable & identifiable) const;

	OWUTIL_API std::string getUUID() const;

	/**
	 * Use with cautious: a UUID is already generated in constructor.
	 * Changing it may corrupt some behaviour in the code.
	 */
	OWUTIL_API void setUUID(const std::string & uuid);

protected:

	OWUTIL_API void copy(const Identifiable & identifiable);

private:

	std::string _uuid;
};

#endif	//OWIDENTIFIABLE_H
