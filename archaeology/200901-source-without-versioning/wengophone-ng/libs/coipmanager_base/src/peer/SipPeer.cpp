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

#include <coipmanager_base/peer/SipPeer.h>

#include <util/String.h>
#include <util/StringList.h>

SipPeer::SipPeer() {
}

SipPeer::SipPeer(const SipPeer & sipPeer) {
	copy(sipPeer);
}

SipPeer & SipPeer::operator = (const SipPeer & sipPeer) {
	copy(sipPeer);
	return *this;
}

SipPeer::~SipPeer() {
}

void SipPeer::setIdentity(const std::string & identity) {
	_identity = identity;
}

std::string SipPeer::getIdentity() const {
	return _identity;
}

void SipPeer::setUsername(const std::string & username) {
	_username = username;
}

std::string SipPeer::getUsername() const {
	return _username;
}

void SipPeer::setRealm(const std::string & realm) {
	_realm = realm;
}

std::string SipPeer::getRealm() const {
	return _realm;
}

void SipPeer::setDisplayName(const std::string & displayName) {
	_displayName = displayName;
}

std::string SipPeer::getDisplayName() const {
	return _displayName;
}

std::string SipPeer::getFullIdentity() const {
	return std::string("sip:") + getIdentity() + std::string("@") + getRealm();
}

std::string SipPeer::toString() const {
	return getDisplayName() + " <" + getFullIdentity() + ">";
}

void SipPeer::copy(const SipPeer & sipPeer) {
	_identity = sipPeer._identity;
	_username = sipPeer._username;
	_realm = sipPeer._realm;
	_displayName = sipPeer._displayName;
}

SipPeer SipPeer::fromString(const std::string & str) {
	SipPeer result;
	String fullId = String(str);

	std::string displayName = fullId.split(" ")[0];
	result.setDisplayName(displayName);

	std::string::size_type clIndex = fullId.find(':', 0);
	std::string::size_type atIndex = fullId.find('@', 0);
	std::string identity = fullId.substr(clIndex + 1, atIndex - clIndex - 1);
	result.setIdentity(identity);
	result.setUsername(identity);

	std::string realm = fullId.split("@")[1];
	result.setRealm(realm.substr(0, realm.size() - 1));

	return result;
}

SipPeer SipPeer::fromFullIdentity(const std::string & str) {
	SipPeer result;
	String fullId = String(str);

	std::string::size_type clIndex = fullId.find(':', 0);
	std::string::size_type atIndex = fullId.find('@', 0);
	std::string identity = fullId.substr(clIndex + 1, atIndex - clIndex - 1);
	result.setDisplayName(identity);
	result.setIdentity(identity);
	result.setUsername(identity);

	std::string realm = fullId.split("@")[1];
	result.setRealm(realm);

	return result;
}
