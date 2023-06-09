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

#ifndef OWSIPACCOUNT_H
#define OWSIPACCOUNT_H

#include "EnumSipLoginState.h"

#include <imwrapper/Account.h>

#include <util/Event.h>
#include <util/Interface.h>

#include <model/network/NetworkDiscovery.h>

#include <string>

/**
 * Contains informations of a SIP account.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class SipAccount : public Account {
public:

	/**
	 * Emitted when the account tried to register.
	 *
	 * @param sender this class
	 * @param state the SIP account login state
	 */
	Event< void (SipAccount & sender, EnumSipLoginState::SipLoginState state) > loginStateChangedEvent;

	enum NetworkDiscoveryState {
		/** Initial NetworkDiscoveryState. */
		NetworkDiscoveryStateUnknown,

		/** Network discovery has been successful. */
		NetworkDiscoveryStateOk,

		/** An error occured in testHTTP. */
		NetworkDiscoveryStateHTTPError,

		/** An error occured in testSIP. */
		NetworkDiscoveryStateSIPError,

		/** A proxy has been detected and it needs authentication. */
		NetworkDiscoveryStateProxyNeedsAuthentication,

		/** A generic error occured while discovering the network. */
		NetworkDiscoveryStateError
	};

	/**
	 * Emitted when an event occured while detecting network configuration.
	 *
	 * @param sender this class
	 * @param state state of the network discovery
	 */
	Event< void (SipAccount & sender, NetworkDiscoveryState state) > networkDiscoveryStateChangedEvent;

	SipAccount();

	SipAccount(const SipAccount & sipAccount);

	virtual ~SipAccount();

	/**
	 * Initializes the SIP account.
	 *
	 * Discover network and set configuration.
	 */
	virtual void init() = 0;

	/**
	 * @return the full identity
	 * e.g: identity: toto
	 * realm: voip.wengo.fr
	 * full identity: sip:toto@voip.wengo.fr
	 */
	const std::string getFullIdentity() const {
		return std::string("sip:") + getIdentity() 
			+ std::string("@") + getRealm();
	}

	/**
	 * @return the user identity
	 */
	const std::string & getIdentity() const {
		return _identity;
	}

	/**
	 * @return the user name. Will be sent in the "From: " header
	 *         of a SIP transaction. Usually same as Identity
	 */
	const std::string & getUsername() const {
		return _username;
	}

	/**
	 * @return the realm of this SipAccount.
	 * e.g: from a wengo account: test@voip.openwengo.org,
	 * getRealm will return voip.openwengo.org
	 */
	const std::string & getRealm() const {
		return _realm;
	}

	/**
	 * @return the password.
	 */
	const std::string & getPassword() const {
		return _password;
	}

	/**
	 * @return the display name
	 */
	const std::string & getDisplayName() const {
		return _displayName;
	}

	/**
	 * @return the register server address
	 */
	const std::string & getRegisterServerHostname() const {
		return _registerServerHostname;
	}

	/**
	 * @return the register server port
	 */
	unsigned getRegisterServerPort() const {
		return _registerServerPort;
	}

	/**
	 * @return the SIP proxy server address
	 */
	const std::string & getSIPProxyServerHostname() const {
		return _sipProxyServerHostname;
	}

	/**
	 * @return the SIP proxy server port
	 */
	unsigned getSIPProxyServerPort() const {
		return _sipProxyServerPort;
	}

	/**
	 * @return the tunnel server address. Currently only useful for a WengoAccount
	 */
	const std::string & getHttpTunnelServerHostname() const {
		return _httpTunnelServerHostname;
	}

	/**
	 * @return the tunnel server port. Currently only useful for a WengoAccount
	 */
	unsigned getHttpTunnelServerPort() const {
		return _httpTunnelServerPort;
	}

	/**
	 * @return true if HttpTunnel must be used with SSL
	 */
	bool httpTunnelHasSSL() const {
		return _httpTunnelWithSSL;
	}

	/**
	 * @return true if the SIP connection needs a HttpTunnel
	 */
	bool isHttpTunnelNeeded() const {
		return _needsHttpTunnel;
	}

	/**
	 * @return the local SIP port to use
	 */
	unsigned getLocalSIPPort() const {
		return _localSIPPort;
	}

	bool hasAutoLogin() const {
		return _autoLogin;
	}

	/**
	 * @return true if this SipAccount is connected
	 */
	bool isConnected() const {
		return _isConnected;
	}

	/**
	 * Sets connection state.
	 */
	void setConnected(bool connected);

	enum SipAccountType {
		/** Basic SIP account. */
		SipAccountTypeBasic,

		/** Wengo SIP account. */
		SipAccountTypeWengo
	};

	/**
	 * Gets the type of SIP account for this SipAccount.
	 *
	 * @return SIP account type
	 */
	virtual SipAccountType getType() const = 0;

	NetworkDiscoveryState getLastNetworkDiscoveryState() const {
		return _lastNetworkDiscoveryState;
	}

	EnumSipLoginState::SipLoginState getLastLoginState() const {
		return _lastLoginState;
	}

	/**
	 * Gets the VLine used by this SipAccount.
	 */
	int getVLineID() const {
		return _vLineId;
	}

	/**
	 * Sets the VLine ID used by this SipAccount.
	 */
	void setVLineID(int vLineId) {
		_vLineId = vLineId;
	}

protected:
	
	// Inherited from Account
	virtual void copyTo(Account * account) const;
	////

	/**
	 * Copy a SipAccount.
	 */
	void copy(const SipAccount & sipAccount);

	/** The identity. */
	std::string _identity;

	/** The username. */
	std::string _username;

	/** The realm. */
	std::string _realm;

	/** The password. */
	std::string _password;

	/** The display name. */
	std::string _displayName;

	/** The register server hostname. */
	std::string _registerServerHostname;

	/** The register server port. */
	unsigned _registerServerPort;

	/** The SIP proxy server hostname. */
	std::string _sipProxyServerHostname;

	/** The SIP proxy server port. */
	unsigned _sipProxyServerPort;

	/** True if the SIP connection needs a HttpTunnel. */
	bool _needsHttpTunnel;

	/** The HttpTunnel server address. Currently only useful for a WengoAccount */
	std::string _httpTunnelServerHostname;

	/** The HttpTunnel server port. Currently only useful for a WengoAccount */
	unsigned _httpTunnelServerPort;

	/** True if HttpTunnel must use SSL. */
	bool _httpTunnelWithSSL;

	/** The local SIP port to use. */
	unsigned _localSIPPort;

	/** The NetworkDiscovery object. */
	NetworkDiscovery _networkDiscovery;

	/** True if this SipAccount is connected. */
	bool _isConnected;

	/** True if this SipAccount must login automatically. */
	bool _autoLogin;

	/** The STUN server used by NetworkDiscovery to check NAT and UDP connection. */
	std::string _stunServer;

	/** Used to remember the last NetworkDiscoveryState. */
	NetworkDiscoveryState _lastNetworkDiscoveryState;

	/** Used to remember the last LoginState. */
	EnumSipLoginState::SipLoginState _lastLoginState;

	/** VLine ID used by this SipAccount. */
	int _vLineId;
};

#endif	//OWSIPACCOUNT_H
