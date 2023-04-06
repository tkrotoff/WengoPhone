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

#ifndef OWENUMPROXYAUTHTYPE_H
#define OWENUMPROXYAUTHTYPE_H

#include <networkdiscovery/networkdiscoverydll.h>

#include <util/NonCopyable.h>

#include <netlib.h>

#include <string>

/**
 * Proxy authentication type.
 *
 * @see EnumAuthType (from netlib)
 * @author Philippe Bernery
 */
class EnumProxyAuthType : NonCopyable {
public:

	enum ProxyAuthType {
		ProxyAuthTypeUnknown,
		ProxyAuthTypeBasic,
		ProxyAuthTypeDigest,
		ProxyAuthTypeNTLM,
	};

	/**
	 * Converts a ProxyAuthType into a string.
	 *
	 * @return the string
	 */
	NETWORKDISCOVERY_API static std::string toString(ProxyAuthType proxyAuthType);

	/**
	 * Converts a string into a ProxyAuthType.
	 *
	 * @return the NatType
	 */
	NETWORKDISCOVERY_API static ProxyAuthType toProxyAuthType(const std::string & authType);

	/**
	 * Converts an EnumAuthType (type from netlib) to a ProxyAuthType
	 */
	NETWORKDISCOVERY_API static ProxyAuthType toProxyAuthType(const EnumAuthType & authType);

};

#endif //OWENUMPROXYAUTHTYPE_H
