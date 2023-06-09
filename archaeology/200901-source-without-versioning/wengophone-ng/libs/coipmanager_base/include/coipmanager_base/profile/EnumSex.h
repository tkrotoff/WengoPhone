/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007 Wengo
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

#ifndef OWSEX_H
#define OWSEX_H

#include <coipmanager_base/coipmanagerbasedll.h>

#include <util/NonCopyable.h>

#include <string>

/**
 * Sex: male or female.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API EnumSex : NonCopyable {
public:

	enum Sex {
		SexUnknown,
		SexMale,
		SexFemale
	};

	/**
	 * Gets a string represencation of the sex.
	 *
	 * @param sex the sex to transform
	 * @result the string representing the sex
	 */
	static std::string toString(Sex sex);

	/**
	 * Gets the sex from a string representation.
	 *
	 * @param sex the string to transform.
	 * @return the sex represented by the string
	 */
	static Sex toSex(const std::string & sex);
};

#endif	//OWSEX_H
