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

#ifndef IMACCOUNT_H
#define IMACCOUNT_H

#include <imwrapper/EnumIMProtocol.h>
#include <imwrapper/EnumPresenceState.h>
#include <imwrapper/IMAccountParameters.h>

#include <util/Event.h>

#include <string>

/**
 * Instant Messaging account.
 *
 * Associates a login/password to an IM protocol.
 * Example: login: "bob@hotmail.com" password: "axBi9opZ" protocol: IMProtocolMSN
 *
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class IMAccount {
	friend class IMAccountXMLSerializer;
	friend class IMConnect;
	friend class IMPresence;
public:

	/**
	 * Emitted when the IMAccount has been changed.
	 *
	 * @param sender this class
	 */
	Event< void (IMAccount & sender) > imAccountChangedEvent;

	/**
	 * Emitted when the IMAccount will be deleted.
	 *
	 * @param sender this class
	 */
	Event< void (IMAccount & sender) > imAccountWillDieEvent;

	/**
	 * Emitted when the IMAccount has been destroyed.
	 *
	 * @param sender this class
	 */
	Event< void (IMAccount & sender) > imAccountDeadEvent;

	IMAccount();

	IMAccount(const std::string & login, const std::string & password, EnumIMProtocol::IMProtocol protocol);

	IMAccount(const IMAccount & imAccount);

	~IMAccount();

	const std::string & getLogin() const {
		return _login;
	}

	void setLogin(const std::string & login);

	const std::string & getPassword() const {
		return _password;
	}

	void setPassword(const std::string & password);

	EnumIMProtocol::IMProtocol getProtocol() const {
		return _protocol;
	}

	EnumPresenceState::PresenceState getPresenceState() const {
		return _presenceState;
	}

	bool isConnected() {
		return _connected;
	}

	IMAccountParameters & getIMAccountParameters() {
		return _imAccountParameters;
	}

	bool operator == (const IMAccount & imAccount) const;

	bool operator < (const IMAccount & imAccount) const;

private:

	/**
	 * Used by IMPresence to set the PresenceState.
	 */
	void setPresenceState(EnumPresenceState::PresenceState presenceState) {
		_presenceState = presenceState;
	}

	/**
	 * Used by IMConnect to set the connection state.
	 */
	void setConnected(bool connected) {
		_connected = connected;
	}

	/**
	 * This method exists because of Jabber that uses conatctId with a 'Resource'
	 * e.g.: 'blabla@jabber.org/Resource'.
	 *
	 * It will add '/WengoPhone' to the login id if the login does not contain
	 * a Resource.
	 *
	 * @param login the old login
	 * @return the corrected login
	 */
	static std::string correctedLogin(const std::string & login, EnumIMProtocol::IMProtocol protocol);

	/**
	 * @see Settings::valueChangedEvent
	 */
	void valueChangedEventHandler(Settings & sender, const std::string & key);

	std::string _login;

	std::string _password;

	EnumIMProtocol::IMProtocol _protocol;

	/** This Settings is used to contain more parameters (eg.: "use_http => true" for MSN protocol). */
	IMAccountParameters _imAccountParameters;

	/**
	 * Desired PresenceState for this IMAccount.
	 * If PresenceState is Offline, the IMAccount will not connect automatically.
	 */
	EnumPresenceState::PresenceState _presenceState;

	/** Connection state. */
	bool _connected;
};

#endif	//IMACCOUNT_H
