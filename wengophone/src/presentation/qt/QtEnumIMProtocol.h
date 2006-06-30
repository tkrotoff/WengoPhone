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

#ifndef QTENUMIMPROTOCOL_H
#define QTENUMIMPROTOCOL_H

#include <util/NonCopyable.h>

#include <imwrapper/EnumIMProtocol.h>

#include <QString>

/**
 * Instant Messaging protocols.
 *
 * @see EnumIMProtocol
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class QtEnumIMProtocol : NonCopyable {
public:

	enum IMProtocol {
		IMProtocolUnknown,
		IMProtocolMSN,
		IMProtocolYahoo,
		IMProtocolAIMICQ,
		IMProtocolJabber,
		IMProtocolGoogleTalk,
		IMProtocolSIPSIMPLE,
		IMProtocolWengo
	};

	/**
	 * Converts a protocol into a string.
	 *
	 * @return the string
	 */
	static QString toString(IMProtocol protocol);

	/**
	 * Converts a string into a protocol.
	 *
	 * @return the protocol
	 */
	static IMProtocol toIMProtocol(const QString & protocol);

	/**
	 * Converts an EnumIMProtocol into a QtEnumIMProtocol.
	 *
	 * @return the protocol
	 */
	static IMProtocol toIMProtocol(EnumIMProtocol::IMProtocol protocol);
};

#endif	//QTENUMIMPROTOCOL_H
