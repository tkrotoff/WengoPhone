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

#include <coipmanager_base/account/IAccount.h>
#include <coipmanager_base/peer/SipPeer.h>

#include <networkdiscovery/EnumNatType.h>

#include <string>

/**
 * Contains informations of a SIP account.
 *
 * @ingroup coipmanager
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API SipAccount : public IAccount, public SipPeer {
public:

	SipAccount();

	SipAccount(const std::string & identity, const std::string & password);

	SipAccount(const SipAccount & sipAccount);

	explicit SipAccount(const IAccount * iAccount);

	virtual SipAccount * clone() const;

	virtual ~SipAccount();

	/**
	 * Register server address.
	 */
	void setRegisterServerHostname(const std::string & server);
	const std::string & getRegisterServerHostname() const;

	/**
	 * Register server port.
	 */
	void setRegisterServerPort(unsigned port);
	unsigned getRegisterServerPort() const;

	/**
	 * SIP proxy server address e.g outbound proxy.
	 */

	/** FIXME Not used yet */
	void setUseSIPProxyServer(bool useSipProxy);
	/** FIXME Not used yet */
	bool useSIPProxyServer() const;

	void setSIPProxyServerHostname(const std::string & server);
	const std::string & getSIPProxyServerHostname() const;

	/**
	 * SIP proxy server port.
	 */
	void setSIPProxyServerPort(unsigned port);
	unsigned getSIPProxyServerPort() const;

	/**
	 * Http tunnel is supported by this account.
	 */
	void setHttpTunnelSupported(bool supported);
	bool isHttpTunnelSupported() const;

	/**
	 * Tunnel server address. Currently only useful for a WengoAccount
	 */
	void setHttpTunnelServerHostname(const std::string & server);
	const std::string & getHttpTunnelServerHostname() const;

	/**
	 * SSL Tunnel server address. Currently only useful for a WengoAccount
	 */
	void setHttpsTunnelServerHostname(const std::string & server);
	const std::string & getHttpsTunnelServerHostname() const;

	/**
	 * Tunnel server port. Currently only useful for a WengoAccount
	 */
	void setHttpTunnelServerPort(unsigned port);
	unsigned getHttpTunnelServerPort() const;

	/**
	 * HttpTunnel has SSL.
	 */
	void setUseSSLForHttpTunnel(bool useSSL);
	bool useSSLForHttpTunnel() const;

	/**
	 * HttpTunnel needed.
	 */
	void setHttpTunnelNeeded(bool needed);
	bool isHttpTunnelNeeded() const;

//	enum SipAccountType {
		/** Basic SIP account. */
//		SipAccountTypeBasic,

		/** Wengo SIP account. */
//		SipAccountTypeWengo
//	};

	/**
	 * Gets the type of SIP account for this SipAccount.
	 *
	 * @return SIP account type
	 */
//	virtual SipAccountType getType() const = 0;

	/**
	 * Virtual phoneline id used by this SipAccount.
	 */
	void setVirtualLineId(int lineId);
	int getVirtualLineId() const;

	/**
	 * Nat type.
	 */
	void setNatType(EnumNatType::NatType natType);
	EnumNatType::NatType getNatType() const;

	virtual std::string getPeerId() const;

protected:

	/**
	 * Copy a SipAccount.
	 */
	void copy(const SipAccount & sipAccount);

	/** The register server hostname. */
	std::string _registerServerHostname;

	/** The register server port. */
	unsigned _registerServerPort;

	/**
	 * If SIP proxy server (e.g outbound proxy) should be used or not.
	 * FIXME Not used yet
	 */
	bool _useSipProxyServer;

	/** The SIP proxy server hostname. */
	std::string _sipProxyServerHostname;

	/** The SIP proxy server port. */
	unsigned _sipProxyServerPort;

	/** True if account support Http tunneling. */
	bool _httpTunnelSupported;

	/** True if the SIP connection needs a HttpTunnel. */
	bool _needsHttpTunnel;

	/** The HttpTunnel server address. Currently only useful for a WengoAccount */
	std::string _httpTunnelServerHostname;

	/** The HttpTunnel server port. Currently only useful for a WengoAccount */
	unsigned _httpTunnelServerPort;

	/** The HttpTunnel SSL server address. Currently only useful for a WengoAccount */
	std::string _httpsTunnelServerHostname;

	/** True if HttpTunnel must use SSL. */
	bool _httpTunnelWithSSL;

	/** VLine ID used by this SipAccount. */
	int _lineId;

	/** Nat type. */
	EnumNatType::NatType _natType;
};

#endif //OWSIPACCOUNT_H
