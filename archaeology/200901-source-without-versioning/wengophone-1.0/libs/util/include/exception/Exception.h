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

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

#ifdef WIN32
#  pragma warning(disable : 4290)
#endif

/**
 * Exception base class.
 *
 * I recreated this exception hierarchie because Visual C++ 2003 seems
 * to not like stdexcept and invalid_argument from the STL.
 *
 * @see std::exception
 * @see java.lang.Exception
 * @author Tanguy Krotoff
 */
class Exception {
public:

	Exception() {
	}

	virtual ~Exception() {
	}

	/**
	 * Constructs a new Exception with the specified detail message.
	 *
	 * @param message the detail message
	 */
	Exception(const std::string & message) {
		_message = message;
	}

	/**
	 * Gets the detail message string.
	 *
	 * @return the detail message
	 */
	virtual std::string what() const {
		return _message;
	}

private:

	Exception & operator=(const Exception &);

	/** Detail message of this Exception. */
	std::string _message;
};

#endif	//EXCEPTION_H
