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

#ifndef OWSIPACCOUNT_H
#define OWSIPACCOUNT_H

#include "EnumSipLoginState.h"

#include <imwrapper/Account.h>

#include <thread/Mutex.h>
#include <thread/Timer.h>

#include <util/Event.h>
#include <util/Interface.h>

#include <model/network/NetworkDiscovery.h>

#include <string>

class NetworkObserver;

/**
 * Contains informations of a SIP account.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 * @author Xavier Desjardins
 */
class SipAccount : public Account {
public:

	/** Empty SipAccount. */
	static SipAccount empty;

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

		/** A sip error occured while discovering the network. */
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

	SipAccount(const std::string & username,const std::string & userid ,
		const std::string & password, const std::string & realm,
		const std::string & registerservername, unsigned registerserverport,
		const std::string & SIPproxyservername, unsigned SIPproxyserverport,
		bool rememberPassword, bool enablePIM);

	SipAccount(const SipAccount & sipAccount);

	virtual ~SipAccount();

	virtual SipAccount * clone() const;

	virtual SipAccount & operator = (const SipAccount & sipAccount);

	virtual bool operator== (const SipAccount & other) const;

	/**
	 * Initializes the SIP account.
	 *
	 * Discover network and set configuration.
	 */
	virtual void init();

	/**
	 * @return the full identity
	 * e.g: identity: toto
	 * realm: voip.wengo.fr
	 * full identity: toto@voip.wengo.fr
	 */
	virtual const std::string getFullIdentity() const {
		return getIdentity() + std::string("@") + getRealm();
	}

	/**
	 * @return the sip address
	 * e.g: identity: toto
	 * realm: voip.wengo.fr
	 * full identity: sip:toto@voip.wengo.fr
	 */
	const std::string getSIPAddress() const {
		return std::string("sip:") + getIdentity() + std::string("@") + getRealm();
	}

	/**
	 * @return the user identity
	 */
	const std::string & getIdentity() const {
		return _identity;
	}
	/**
	 * @param identity the user identity
	 */
	void setIdentity(const std::string & identity) { _identity = identity; }

	/**
	 * @return the user name. Will be sent in the "From: " header
	 *         of a SIP transaction. Usually same as Identity
	 */
	const std::string & getUsername() const {
		return _username;
	}
	/**
	 * @param the user name. Will be sent in the "From: " header
	 *         of a SIP transaction. Usually same as Identity
	 */
	void setUsername(const std::string & username) { _username = username; }
	
	/**
	 * @return the realm of this SipAccount.
	 * e.g: from a wengo account: test@voip.openwengo.org,
	 * getRealm will return voip.openwengo.org
	 */
	const std::string & getRealm() const {
		return _realm;
	}

	/**
	 * @param the realm of this SipAccount.
	 * e.g: from a wengo account: test@voip.openwengo.org,
	 * getRealm will return voip.openwengo.org
	 */
	void setRealm(const std::string & realm) { _realm = realm; }

	/**
	 * @return the password.
	 */
	const std::string & getPassword() const {
		return _password;
	}

	/**
	 * @param the password.
	 */
	void setPassword(const std::string & password){ _password = password; }

	/**
	 * @return the display name
	 */
	const std::string & getDisplayName() const {
		return _displayName;
	}

	/**
	 * @param the display name
	 */
	void setDisplayName(const std::string & displayName) { _displayName = displayName; }

	/**
	 * @return the register server address
	 */
	const std::string & getRegisterServerHostname() const {
		return _registerServerHostname;
	}

	/**
	 * @param the register server address
	 */
	void setRegisterServerHostname(const std::string & server) { _registerServerHostname = server; }

	/**
	 * @return the register server port
	 */
	unsigned getRegisterServerPort() const {
		return _registerServerPort;
	}

	/**
	 * @param the register server port
	 */
	void setRegisterServerPort(unsigned port) { _registerServerPort = port; }

	/**
	 * @return the SIP proxy server address
	 */
	const std::string & getSIPProxyServerHostname() const {
		return _sipProxyServerHostname;
	}

	/**
	 * @param SIP proxy server address.
	 */
	void setSIPProxyServerHostname(const std::string & server) { _sipProxyServerHostname = server; }

	/**
	 * @return the SIP proxy server port
	 */
	unsigned getSIPProxyServerPort() const {
		return _sipProxyServerPort;
	}

	/**
	 * @param SIP proxy server port.
	 */
	void setSIPProxyServerPort(unsigned port) { _sipProxyServerPort = port; }

	/**
	 * @return the http tunnel server address. Currently only useful for a WengoAccount
	 */
	const std::string & getHttpTunnelServerHostname() const {
		return _httpTunnelServerHostname;
	}

	/**
	 * @return the http tunnel server port. Currently only useful for a WengoAccount
	 */
	unsigned getHttpTunnelServerPort() const {
		return _httpTunnelServerPort;
	}

	/**
	 * @return the https tunnel server address. Currently only useful for a WengoAccount
	 */
	const std::string & getHttpsTunnelServerHostname() const {
		return _httpsTunnelServerHostname;
	}

	/**
	 * @return the https tunnel server port. Currently only useful for a WengoAccount
	 */
	unsigned getHttpsTunnelServerPort() const {
		return _httpsTunnelServerPort;
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

	/**
	 * @return True if remember password is activated.
	 */
	bool isPasswordRemembered() const {
		return _rememberPassword;
	}

	/**
	 * @return Set remember password.
	 */
	void setRememberPassword(bool newState) {
		_rememberPassword = newState;
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
	
	/**
	 * PhoneLine can not connect.
	 */
	void phoneLineAuthenticationError();

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
	virtual SipAccountType getType() const {
		return SipAccountTypeBasic;
	}

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

	/**
	 * @return true if empty
	 */
	virtual bool isEmpty() const;

	/**
	 * @return password set by user
	 */
	virtual const std::string getUserPassword() const {
		return _password;
	}

	/**
	 * @return userprofile's name
	 */
	virtual const std::string getUserProfileName() const {
		return _userProfileName;
	}

	/**
	 * Sets the userprofile's name
	 *
	 * @param newUserProfileName name of the userprofile
	 */
	void setUserProfileName(std::string newUserProfileName) {
		_userProfileName = newUserProfileName;
	}

	/**
	 * Sets visible name
	 *
	 * param name name to be displayed to the user
	 */
	void setVisibleName(const std::string name) {
		_visibleName = name;
	}

	/**
	 * @return name setted by the user
	 */
	virtual const std::string getVisibleName() const {
		return _visibleName;
	}

	/**
	 * @return voicemail number
	 */
	const std::string getVoicemailNumber() {
		return _voicemail;
	}

	/**
	 * Sets the voicemail number
	 * @param number new voicemail number
	 */
	void setVoicemailNumber(std::string number) {
		_voicemail = number;
	}

	/**
	 * Activate PIM(SIP/SIMPLE) support.
	 * @param enable if True SIP/SIMPLE is enabled.
	 */
	void enablePIM(const bool enable) {
		_enablePIM = enable;
	}

	/**
	 * @return True if SIP/SIMPLE is enabled.
	 */
	const bool isPIMEnabled() const {
		return _enablePIM;
	}

protected:

	/**
	 * Copy a SipAccount.
	 */
	virtual void copy(const SipAccount & sipAccount);

	//virtual void initTimeoutEventHandler();

	void initLastTimeoutEventHandler();
	void initLastTimeoutEventHandlerThreadSafe();

	void connectionIsUpEventHandler(NetworkObserver & sender);

	void connectionIsDownEventHandler(NetworkObserver & sender);

	/**
	 * Discovers network for SIP.
	 *
	 * Please contact network@openwengo.com before any modifications.
	 */
	virtual bool discoverForSIP();

	virtual EnumSipLoginState::SipLoginState discoverNetwork();

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

	/** The HttpsTunnel server address. Currently only useful for a WengoAccount */
	std::string _httpsTunnelServerHostname;

	/** The HttpsTunnel server port. Currently only useful for a WengoAccount */
	unsigned _httpsTunnelServerPort;
	
	/** True if HttpTunnel must use SSL. */
	bool _httpTunnelWithSSL;

	/** The local SIP port to use. */
	unsigned _localSIPPort;

	/** The NetworkDiscovery object. */
	NetworkDiscovery _networkDiscovery;

	/** True if network discovery is running. */
	bool _discoveringNetwork;

	/** True if this SipAccount is connected. */
	bool _isConnected;

	/** True if this SipAccount must remember is password. */
	bool _rememberPassword;

	/** The STUN server used by NetworkDiscovery to check NAT and UDP connection. */
	std::string _stunServer;

	/** Used to remember the last NetworkDiscoveryState. */
	NetworkDiscoveryState _lastNetworkDiscoveryState;

	/** Used to remember the last LoginState. */
	EnumSipLoginState::SipLoginState _lastLoginState;

	/** VLine ID used by this SipAccount. */
	int _vLineId;

	/** Number of testSIP retry. */
	static unsigned short _testSIPRetry;

	/** name of the userprofile that uses this sipaccount */
	std::string _userProfileName;

	/** name set and visiable by user */
	std::string _visibleName;

	/** voicemail number */
	std::string _voicemail;

	/** enable Presence and Instant Messaging support (SIP/SIMPLE) */
	bool _enablePIM;

	Mutex _mutex;

	Timer _initTimer;

	/** True if initialization has been successul. */
	bool _initSuccessful;

	/** count timer call */
	int _initTimerCounter;
	
	/** maxium timer call */
	static int _initTimerMaximum;
	
	/** init timer time interval */
	static int _initTimerInterval;
};

#endif	//OWSIPACCOUNT_H
