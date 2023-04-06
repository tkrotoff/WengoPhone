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

#include <coipmanager_base/imcontact/SipIMContact.h>

#include <util/String.h>

SipIMContact::SipIMContact() 
	: IIMContact(EnumAccountType::AccountTypeSIP, String::null),
	SipPeer() {
}

SipIMContact::SipIMContact(const std::string & fullIdentity) 
	: IIMContact(EnumAccountType::AccountTypeSIP, String::null),
	SipPeer() {

	SipPeer::copy(SipPeer::fromFullIdentity(fullIdentity));
}

SipIMContact::SipIMContact(const SipIMContact & sipIMContact)
	: IIMContact(sipIMContact),
	SipPeer(sipIMContact) {
}

SipIMContact::SipIMContact(const SipPeer & sipPeer) 
	: IIMContact(EnumAccountType::AccountTypeSIP, String::null),
	SipPeer(sipPeer) {
}

SipIMContact & SipIMContact::operator = (const SipIMContact & sipIMContact) {
	copy(sipIMContact);
	return *this;
}

SipIMContact * SipIMContact::clone() const {
	return new SipIMContact(*this);
}

SipIMContact::~SipIMContact() {
}

void SipIMContact::copy(const SipIMContact & sipIMContact) {
	IIMContact::copy(sipIMContact);
	SipPeer::copy(sipIMContact);
}

std::string SipIMContact::getPeerId() const {
	return getFullIdentity();
}
