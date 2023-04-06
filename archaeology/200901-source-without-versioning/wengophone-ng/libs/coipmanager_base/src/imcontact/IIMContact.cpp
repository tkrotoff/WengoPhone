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

#include <coipmanager_base/imcontact/IIMContact.h>

#include <util/Logger.h>

IIMContact::IIMContact() 
	: Peer() {
}

IIMContact::IIMContact(EnumAccountType::AccountType accountType, const std::string & contactId) 
	: Peer(accountType, contactId) {
}

IIMContact::IIMContact(const IIMContact & imContact) 
	: Peer(imContact) {

	copy(imContact);
}

IIMContact & IIMContact::operator = (const IIMContact & imContact) {
	copy(imContact);
	return *this;
}

IIMContact * IIMContact::clone() const {
	return new IIMContact(*this);
}

void IIMContact::copy(const IIMContact & imContact) {
	Peer::copy(imContact);

	_accountId = imContact._accountId;
	_groupList = imContact._groupList;
}

IIMContact::~IIMContact() {
}

void IIMContact::setAccountId(const std::string & accountId) {
	_accountId = accountId;
}

std::string IIMContact::getAccountId() const {
	return _accountId;
}

void IIMContact::setContactId(const std::string & contactId) {
	Peer::setPeerId(contactId);
}

std::string IIMContact::getContactId() const {
	return Peer::getPeerId();
}

void IIMContact::setPeerId(const std::string & peerId) {
	Peer::setPeerId(peerId);
}

std::string IIMContact::getPeerId() const {
	return Peer::getPeerId();
}

void IIMContact::setAccountType(EnumAccountType::AccountType accountType) {
	Peer::setAccountType(accountType);
}

void IIMContact::setPresenceState(EnumPresenceState::PresenceState presenceState) {
	Peer::setPresenceState(presenceState);

	EnumConnectionState::ConnectionState state = EnumConnectionState::ConnectionStateDisconnected;
	if ((presenceState != EnumPresenceState::PresenceStateUnknown)
		&& (presenceState != EnumPresenceState::PresenceStateOffline)) {
		state = EnumConnectionState::ConnectionStateConnected;
	}
	setConnectionState(state);
}

void IIMContact::addToGroup(const std::string & group) {
	StringList::const_iterator it = 
		std::find(_groupList.begin(), _groupList.end(), group);
	if (it == _groupList.end()) {
		_groupList.push_back(group);
	}
}

void IIMContact::removeFromGroup(const std::string & group) {
	_groupList.remove(group);
}

StringList IIMContact::getGroupList() const {
	return _groupList;
}
