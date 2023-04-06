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

#ifndef OWPEER_H
#define OWPEER_H

#include <coipmanager_base/EnumAccountType.h>
#include <coipmanager_base/EnumConnectionState.h>
#include <coipmanager_base/EnumPresenceState.h>

#include <util/Identifiable.h>
#include <util/OWPicture.h>

#include <string>

/**
 * Gather common attributes between an IMContact and an Account.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API Peer : public Identifiable {
public:

	Peer();

	Peer(EnumAccountType::AccountType accountType, const std::string & peerId);

	Peer(const Peer & peer);

	Peer & operator = (const Peer & peer);

	virtual Peer * clone() const;

	virtual ~Peer();

	virtual void setPeerId(const std::string & peerId);
	virtual std::string getPeerId() const;

	virtual void setAccountType(EnumAccountType::AccountType accountType);
	virtual EnumAccountType::AccountType getAccountType() const;

	virtual void setPresenceState(EnumPresenceState::PresenceState presenceState);
	virtual EnumPresenceState::PresenceState getPresenceState() const;

	virtual void setAlias(const std::string & alias);
	virtual std::string getAlias() const;

	virtual void setConnectionState(EnumConnectionState::ConnectionState state);
	virtual EnumConnectionState::ConnectionState getConnectionState() const;

	virtual void setIcon(const OWPicture & icon);
	virtual OWPicture getIcon() const;

protected:

	void copy(const Peer & peer);

	std::string _peerId;

	EnumAccountType::AccountType _accountType;

	OWPicture _icon;

	std::string _alias;

	EnumPresenceState::PresenceState _presenceState;

	EnumConnectionState::ConnectionState _connectionState;
};

#endif	//OWPEER_H
