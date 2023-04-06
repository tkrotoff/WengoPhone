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

#ifndef OWSIPIMCONTACT_H
#define OWSIPIMCONTACT_H

#include <coipmanager_base/imcontact/IIMContact.h>
#include <coipmanager_base/peer/SipPeer.h>

/**
 * Interface for IMContact.
 * Represents the most little part of Contact: its id on a accountType.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API SipIMContact : public IIMContact, public SipPeer {
public:

	SipIMContact();

	SipIMContact(const std::string & fullIdentity);

	SipIMContact(const SipIMContact & sipIMContact);

	SipIMContact(const SipPeer & sipPeer);

	SipIMContact & operator = (const SipIMContact & sipIMContact);

	virtual SipIMContact * clone() const;

	virtual ~SipIMContact();

	virtual std::string getPeerId() const;

protected:

	void copy(const SipIMContact & sipIMContact);
};

#endif //OWSIPIMCONTACT_H
