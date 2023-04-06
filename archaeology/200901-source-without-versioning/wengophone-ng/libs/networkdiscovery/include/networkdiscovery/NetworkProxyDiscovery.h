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

#ifndef OWNETWORKPROXYDISCOVERY_H
#define OWNETWORKPROXYDISCOVERY_H

#include <networkdiscovery/networkdiscoverydll.h>

#include <networkdiscovery/EnumProxyAuthType.h>

#include <util/Singleton.h>

#include <string>

/**
 * Network proxy data container.
 */
class NetworkProxy {
public:

	NETWORKDISCOVERY_API NetworkProxy();

	NETWORKDISCOVERY_API NetworkProxy(const NetworkProxy & networkProxy);

	NETWORKDISCOVERY_API NetworkProxy & operator = (const NetworkProxy & networkProxy);

	NETWORKDISCOVERY_API std::string getLogin() const;
	NETWORKDISCOVERY_API void setLogin(const std::string & login);

	NETWORKDISCOVERY_API std::string getPassword() const;
	NETWORKDISCOVERY_API void setPassword(const std::string & password);

	NETWORKDISCOVERY_API std::string getServer() const;
	NETWORKDISCOVERY_API void setServer(const std::string & server);

	NETWORKDISCOVERY_API int getServerPort() const;
	NETWORKDISCOVERY_API void setServerPort(int port);

	NETWORKDISCOVERY_API EnumProxyAuthType::ProxyAuthType getProxyAuthType() const;
	NETWORKDISCOVERY_API void setProxyAuthType(EnumProxyAuthType::ProxyAuthType proxyAuthType);

private:

	void copy(const NetworkProxy & networkProxy);

	std::string _login;

	std::string _password;

	std::string _server;

	int _serverPort;

	EnumProxyAuthType::ProxyAuthType _proxyAuthType;
};

/**
 * Network proxy discovery.
 *
 * This class gets proxy information from the system:
 *  - from Internet Explorer on Windows
 *  - from System Preferences on Mac OS X (not implemented yet)
 * 
 * @author Julien Bossart
 * @author Philippe Bernery
 */
class NETWORKDISCOVERY_API NetworkProxyDiscovery : public Singleton<NetworkProxyDiscovery> {
	friend class Singleton<NetworkProxyDiscovery>;
public:

	enum NetworkProxyDiscoveryError {
		/** NetworkProxy cannot be automatically detected. */
		NetworkProxyDiscoveryErrorCannotBeDetected,

		/** NetworkProxy has been detected. */
		NetworkProxyDiscoveryErrorDetected,

		/** NetworkProxyDiscovery needs authentication parameters to continue discovery. */
		NetworkProxyDiscoveryErrorNeedsAuthentication,
	};

	/**
	 * This methods retrieves NetworkProxy information.
	 * It fills the networkProxy variable if found.
	 * @return NetworkProxyDiscoveryCannotBeDetected if proxy cannot be detected,
	 * NetworkProxyDiscoveryDetected if detected,
	 * NetworkProxyDiscoveryNeedsAuthentication if proxy needs authentication.
	 * In the last case, sets authentication on the networkProxy object and call again
	 * the method. If NetworkProxyDiscoveryDetected is returned, information are valid,
	 * NetworkProxyDiscoveryErrorNeedsAuthentication is returned otherwise.
	 */
	NetworkProxyDiscoveryError getNetworkProxy(NetworkProxy & networkProxy) const;

private:

	NetworkProxyDiscovery();

	~NetworkProxyDiscovery();

};

#endif //OWNETWORKPROXYDISCOVERY_H
