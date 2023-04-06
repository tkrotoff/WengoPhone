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

#include <coipmanager_base/peer/Peer.h>

#include <util/Logger.h>

Peer::Peer()
	: Identifiable() {
	_connectionState = EnumConnectionState::ConnectionStateDisconnected;
	_presenceState = EnumPresenceState::PresenceStateUnknown;
	_accountType = EnumAccountType::AccountTypeUnknown;
}

Peer::Peer(EnumAccountType::AccountType accountType, const std::string & peerId)
	: Identifiable() {
	_connectionState = EnumConnectionState::ConnectionStateDisconnected;
	_peerId = peerId;
	_presenceState = EnumPresenceState::PresenceStateUnknown;
	_accountType = accountType;
}

Peer::Peer(const Peer & peer)
	: Identifiable(peer) {
	copy(peer);
}

Peer & Peer::operator = (const Peer & imContact) {
	copy(imContact);
	return *this;
}

Peer * Peer::clone() const {
	return new Peer(*this);
}

void Peer::copy(const Peer & peer) {
	Identifiable::copy(peer);

	_connectionState = peer.getConnectionState();
	_peerId = peer.getPeerId();
	_presenceState = peer.getPresenceState();
	_accountType = peer.getAccountType();
	_icon = peer.getIcon();
	_alias = peer.getAlias();
}

Peer::~Peer() {
}

void Peer::setPeerId(const std::string & peerId) {
	_peerId = peerId;
}

std::string Peer::getPeerId() const {
	return _peerId;
}

void Peer::setAccountType(EnumAccountType::AccountType accountType) {
	_accountType = accountType;
}

EnumAccountType::AccountType Peer::getAccountType() const {
	return _accountType;
}

void Peer::setPresenceState(EnumPresenceState::PresenceState presenceState) {
	_presenceState = presenceState;
}

EnumPresenceState::PresenceState Peer::getPresenceState() const {
	return _presenceState;
}

void Peer::setAlias(const std::string & alias) {
	_alias = alias;
}

std::string Peer::getAlias() const {
	return _alias;
}

void Peer::setConnectionState(EnumConnectionState::ConnectionState state) {
	_connectionState = state;
}

EnumConnectionState::ConnectionState Peer::getConnectionState() const {
	return _connectionState;
}

void Peer::setIcon(const OWPicture & icon) {
	_icon = icon;
}

OWPicture Peer::getIcon() const {
	return _icon;
}
