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

#include <coipmanager/connectmanager/EnumConnectionError.h>

std::string EnumConnectionError::toString(ConnectionError connectionError) {
	std::string result;

	switch (connectionError) {
	case ConnectionErrorUnknown:
		result = "an unknown error occured";
		break;
	case ConnectionErrorNoImplementation:
		result = "no implementation for this account type";
		break;
	case ConnectionErrorServer:
		result = "server error";
		break;
	case ConnectionErrorTimeout:
		result = "timeout";
		break;
	case ConnectionErrorTooMuchRetry:
		result = "too much connection retry";
		break;
	case ConnectionErrorInvalidLoginPassword:
		result = "Login/password error";
		break;
	};

	return result;
}
