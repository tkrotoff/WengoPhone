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

#ifndef OWNETWORKDISCOVERY_H
#define OWNETWORKDISCOVERY_H

#include <networkdiscovery/EnumNatType.h>
#include <networkdiscovery/NetworkProxyDiscovery.h>

#include <util/Singleton.h>

#include <string>

/**
 * Discover Network configuration.
 *
 * @ingroup model
 * @author Philippe Bernery
 */
class NETWORKDISCOVERY_API NetworkDiscovery : public Singleton<NetworkDiscovery> {
	friend class Singleton<NetworkDiscovery>;
public:

	/**
	 * Sets Network proxy information. Used by below tests.
	 */
	void setNetworkProxy(NetworkProxy networkProxy);

	/**
	 * Tests if an url is joinable by Http.
	 *
	 * @param the url to test (e.g: "ws.wengo.fr:443/softphone-sso/sso.php")
	 * @param true if a SSL connection must be tested
	 * @return true if connection ok
	 */
	bool testHTTP(const std::string & url, bool ssl);

	/**
	 * Gets the Nat type
	 *
	 * @param stunServer the STUN server
	 * @param natType [out] the Nat type will be saved in this variable
	 * @return true if connection ok
	 */
	bool testNAT(const std::string & stunServer, EnumNatType::NatType & natType);

	/**
	 * Tests if a server respond to a SIP ping.
	 *
	 * @param server the server to test
	 * @param port the server port
	 * @param localPort the local port to use
	 * @param realm realm of the server
	 * @return true if ok
	 */
	bool testSIP(const std::string & server, int port,
		int localPort, const std::string & realm);

	/**
	 * Tests if a HttpTunnel can be create and if SIP can pass through this tunnel.
	 *
	 * @param tunnelServer the tunnel server
	 * @param tunnelPort the tunnel port
 	 * @param ssl true if tunnel must be done with SSL
	 * @param sipServer the SIP server to ping
	 * @param sipServerPort the SIP server port
	 * @param realm realm of the server
	 * @return true if ok
	 */
	bool testSIPHTTPTunnel(const std::string & tunnelServer, int tunnelPort, bool ssl,
		const std::string & sipServer, int sipServerPort, const std::string & realm);

	/**
	 * @return a free local port.
	 */
	int getFreeLocalPort();

private:

	NetworkDiscovery();

	void owsl_init();

	void owsl_terminate();

	bool owslInitialized;

	static const int PING_TIMEOUT = 3;

	static const int HTTP_TIMEOUT = 10;

	NetworkProxy _networkProxy;

	bool _owslInitialized;

};

#endif	//OWNETWORKDISCOVERY_H
