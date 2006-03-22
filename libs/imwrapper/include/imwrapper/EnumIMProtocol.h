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

#ifndef ENUMIMPROTOCOL_H
#define ENUMIMPROTOCOL_H

#include <util/NonCopyable.h>

#include <string>
#include <map>

/**
 * Instant Messaging protocols.
 *
 * @author Philippe Bernery
 */
class EnumIMProtocol : NonCopyable {
public:
	EnumIMProtocol();

	enum IMProtocol {
		IMProtocolUnknown,
		IMProtocolAll,
		IMProtocolMSN,
		IMProtocolYahoo,
		IMProtocolAIM,
		IMProtocolICQ,
		IMProtocolJabber,
		IMProtocolSIPSIMPLE
	};

	/**
	 * Gets a protocol in string.
	 *
	 * @return the string
	 */
	std::string toString(IMProtocol protocol);

	/**
	 * Gets a string in protocol.
	 *
	 * @return the protocol
	 */
	IMProtocol toIMProtocol(const std::string & protocol);

private:
	typedef std::map<IMProtocol, std::string> ProtocolMap;
	ProtocolMap _protocolMap;
};

#endif	//ENUMIMPROTOCOL_H
