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

#ifndef OWENUMCONNECTIONERROR_H
#define OWENUMCONNECTIONERROR_H

#include <coipmanager/coipmanagerdll.h>

#include <util/NonCopyable.h>

#include <string>

class EnumConnectionError : NonCopyable {
public:

	/**
	 * Error code for connection.
	 */
	enum ConnectionError {
		/** An unknown error occured. */
		ConnectionErrorUnknown,

		/** No implementation for this account type. */
		ConnectionErrorNoImplementation,

		/** Server error. */
		ConnectionErrorServer,

		/** Timeout while trying to connect. */
		ConnectionErrorTimeout,

		/** Too much retry. */
		ConnectionErrorTooMuchRetry,

		/** Login/password error. */
		ConnectionErrorInvalidLoginPassword,
	};

	/**
	 * Gets a human readable string of a ConnectionError.
	 */
	COIPMANAGER_API static std::string toString(ConnectionError connectionError);
};

#endif	//OWENUMCONNECTIONERROR
