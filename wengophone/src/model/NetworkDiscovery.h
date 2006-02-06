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

#ifndef NETWORKDISCOVERY_H
#define NETWORKDISCOVERY_H

#include <netlib.h>

#include <Event.h>

#include <string>

/**
 * Discover Network configuration.
 * 
 * @ingroup model
 * @author Philippe Bernery
 */
class NetworkDiscovery
{
public:

	enum DiscoveryResult {
		DiscoveryResultSSOCanConnect,
		DiscoveryResultSSOCannotConnect,
		DiscoveryResultSIPCanConnect,
		DiscoveryResultSIPCannotConnect,
	};

	/**
	 * Emitted when a poxy has been detected and needs a login/password.
	 * 
	 * @param proxyUrl the url of the detected proxy
	 * @param proxyPort the port of the detected proxy
	 */
	Event< void(NetworkDiscovery & sender, 
		const std::string & proxyUrl, int proxyPort) > proxyNeedsAuthenticationEvent;

	/**
	 * Emitted when given login/password are wrong.
	 *
	 * @param sender this class
	 * @param proxyUrl the url of the detected proxy
	 * @param proxyPort the port of the detected proxy
	 * @param proxyLogin the maybe wrong login
	 * @param proxyPassword the maybe wrong password
	 */
	Event< void(NetworkDiscovery & sender,
		const std::string & proxyUrl, int proxyPort,
		const std::string & proxyLogin, const std::string & proxyPassword)> wrongProxyAuthenticationEvent;

	/**
	 * Emitted when detectoin is finished and Settings are set.
	 * 
	 * @param result the result of the discovery
	 */
	Event< void(NetworkDiscovery & sender, 
		DiscoveryResult result) > discoveryDoneEvent;

	NetworkDiscovery();

	~NetworkDiscovery();

	/**
	 * Launch the discovery of the network configuration for simple Http connection.
	 * 
	 * If proxy settings are set in Settings, they are used.
	 */
	void discoverForSSO();

	/**
	 * Launch the discovery of the network configuration for sip packets sending.
	 * 
	 * If proxy settings are set in Settings, they are used.
	 * 
	 * @param sipServer SIP server to test
	 * @param sipPort SIP port to test
	 */
	void discoverForSIP(const std::string & sipServer, int sipPort);

private:

	/**
	 * 
	 * @return false if proxy is not set (usually when proxy needs authentication), true otherwise
	 */
	bool discoverProxySettings();

	bool udpTest(const std::string sipServer, unsigned sipPort);

	bool tunnelTest(const char * sipServer, int sipPort,
		const char * tunnelServer,
		const char * proxyAddress, int proxyPort,
		const char * proxyLogin, const char * proxyPassword);

	void setProxyConfig(const char * proxyAddress, int proxyPort,
		const char * proxyLogin, const char * proxyPassword);

	void setNatConfig(NatType nat);

	void setSSOConfig(bool ssoSSL);

	void setTunnelNeededConfig(bool needed);

	void setTunnelConfig(const char * server, int port, bool tunnelSSL);

	void setSIPConfig(int port);

	static const std::string _stunAddress;

	static const std::string _ssoAddress;

	static const std::string _ssoURL;

	static const unsigned _pingTimeout;

};

#endif /*NETWORKDISCOVERY_H*/
