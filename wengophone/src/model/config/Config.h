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

#ifndef CONFIG_H
#define CONFIG_H

#include <Settings.h>

#include <string>

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class Config : public Settings {
public:

	Config(const std::string & name);

	~Config();

	std::string getName() const {
		return _name;
	}

	/** String key. Network branch */	
	static const std::string NETWORK_KEY;

	/** Boolean key. True if SSL connection to SSO is available */
	static const std::string NETWORK_SSO_SSL_KEY;

	/** String key. Type of NAT. Can be
	 * - cone
	 * - restricted
	 * - portRestricted
	 * - sym
	 * - symfirewall
	 * - blocked
	 */
	static const std::string NETWORK_NAT_TYPE_KEY;

	/** Integer key. Client SIP Port to use */
	static const std::string NETWORK_SIP_LOCAL_PORT_KEY;

	/** String value. SIP server */
	static const std::string NETWORK_SIP_SERVER_KEY;

	/** Boolean key. True if tunnel is SSL */
	static const std::string NETWORK_TUNNEL_SSL_KEY;

	/** String key. Tunnel server. Empty if no tunnel is needed */
	static const std::string NETWORK_TUNNEL_SERVER_KEY;

	/** Boolean key. True if Http Tunnel needed */
	static const std::string NETWORK_TUNNEL_NEEDED_KEY;

	/** Integer key. Tunnel server port */
	static const std::string NETWORK_TUNNEL_PORT_KEY;

	/** String key. Proxy server. Empty if no proxy */
	static const std::string NETWORK_PROXY_SERVER_KEY;

	/** Integer key. Proxy server port */
	static const std::string NETWORK_PROXY_PORT_KEY;

	/** String key. Proxy login. Empty if not needed */
	static const std::string NETWORK_PROXY_LOGIN_KEY;

	/** String key. Proxy Password. Empty if not needed */
	static const std::string NETWORK_PROXY_PASSWORD_KEY;

private:

	std::string _name;

};

#endif	//CONFIG_H
