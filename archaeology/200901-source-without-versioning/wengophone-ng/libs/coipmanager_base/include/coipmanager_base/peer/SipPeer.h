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

#ifndef OWSIPPEER_H
#define OWSIPPEER_H

#include <coipmanager_base/coipmanagerbasedll.h>
#include <coipmanager_base/EnumAccountType.h>

#include <string>

/**
 * Gather common attributes between a SipIMContact and a SipAccount.
 *
 * This class does not inherit Peer because of multiple inheritance problem,
 * e.g: SipAccount inherits IAccount and SipPeer and IAccount inherits Peer.
 * SipPeer could inherit Peer but IAccount could not inherit SipPeer (has all
 * Account are not SIP based account).
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API SipPeer {
public:

	SipPeer();

	SipPeer(const SipPeer & sipPeer);

	/**
	 * Creates a SipPeer from a String with the following form:
	 * "displayName <sip:identity@realm>"
	 */
	static SipPeer fromString(const std::string & str);

	/**
	 * Creates a SipPeer from a String with the following form:
	 * "sip:identity@realm"
	 */
	static SipPeer fromFullIdentity(const std::string & str);

	SipPeer & operator = (const SipPeer & sipPeer);

	virtual ~SipPeer();

	/**
	 * User identity.
	 */
	virtual void setIdentity(const std::string & identity);
	virtual std::string getIdentity() const;

	/**
	 * User name. Will be sent in the "From: " header
	 * of a SIP transaction. Usually same as Identity
	 */
	virtual void setUsername(const std::string & username);
	virtual std::string getUsername() const;

	/**
	 * Realm.
	 * e.g: for test@voip.openwengo.org,
	 * getRealm will return voip.openwengo.org
	 */
	virtual void setRealm(const std::string & realm);
	virtual std::string getRealm() const;

	/**
	 * Display name.
	 */
	virtual void setDisplayName(const std::string & displayName);
	virtual std::string getDisplayName() const;

	/**
	 * @return the full identity
	 * e.g: identity: toto
	 * realm: voip.wengo.fr
	 * full identity: sip:toto@voip.wengo.fr
	 */
	std::string getFullIdentity() const;

	/**
	 * @return this string: "displayName <sip:identity@realm>"
	 */
	std::string toString() const;

protected:

	void copy(const SipPeer & sipPeer);

	/** The identity. */
	std::string _identity;

	/** The username. */
	std::string _username;

	/** The realm. */
	std::string _realm;

	/** The display name. */
	std::string _displayName;
};

#endif	//OWSIPPEER_H
