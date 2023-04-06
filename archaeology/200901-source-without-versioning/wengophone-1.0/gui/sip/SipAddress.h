/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef SIPADDRESS_H
#define SIPADDRESS_H

#include <string>

/**
 * Class that wraps a SIP URI.
 *
 * @author Tanguy Krotoff
 */
class SipAddress {
public:

	SipAddress() {
	}

	/**
	 * Creates a SipAddress object from a SIP URI like: tanguy <sip:0145235854@wengo.fr>
	 *
	 * @param sipUri SIP URI used to create the SipAddress object
	 * @param phoneNumber of the sipUri param is a phone number or a real SIP address (e.g sip:toto@wengo.fr)
	 */
	SipAddress(const std::string & sipUri, bool phoneNumber = true);

	/**
	 * Converts the SIP URI to a valid phone number.
	 *
	 * SIP URI to convert: tanguy <sip:0145235854@wengo.fr>
	 *
	 * @return phone number (0145235854)
	 */
	std::string getValidPhoneNumber() const;

	/**
	 * Gets the display name part of the SIP URI.
	 *
	 * SIP URI to convert: tanguy <sip:tanguy_k@wengo.fr>
	 *
	 * @return display name (tanguy) or empty string if no display name inside the SIP URI
	 */
	std::string getDisplayName() const;

	/**
	 * Gets the pure/minimal/simplified SIP URI.
	 *
	 * SIP URI to convert: tanguy <sip:tanguy_k@wengo.fr>
	 *
	 * @return simplified SIP URI (sip:tanguy_k@wengo.fr)
	 */
	std::string getMinimalSipUri() const;

	/**
	 * Gets the complete SIP URI.
	 *
	 * @return complete SIP URI
	 */
	const std::string & getSipUri() const {
		return _sipUri;
	}

	/**
	 * Compares two SipAddress together.
	 *
	 * <code>
	 * SipAddress sipAddress1 = ...
	 * SipAddress sipAddress2 = ...
	 * if (sipAddress1 == sipAddress2) {
	 * }
	 * </code>
	 *
	 * @param sipAddress1 left part of the operator
	 * @param sipAddress2 right part of the operator
	 * @return true if both SipAddress are equals, false otherwise
	 */
	friend bool operator==(const SipAddress & sipAddress1, const SipAddress & sipAddress2) {
		return sipAddress1.getSipUri() == sipAddress2.getSipUri();
	}

	/**
	 * Converts a phone number into a SIP URI.
	 *
	 * @param phoneNumber phone number to convert into a Sip URI (0145235854)
	 * @return simplified SIP uri (sip:0145235854@wengo.fr)
	 */
	static SipAddress fromPhoneNumber(const std::string & phoneNumber);

private:

	/**
	 * Returns a valid phone number.
	 *
	 * Removes spaces and replaces + by 00 so we can enter
	 * phone numbers like "+33 1 47 08" rather than "003314708".
	 *
	 * @param phoneNumber input phone number like "+33 1 47 08"
	 * @return the valid phone number like "003314708"
	 */
	static std::string convertToValidPhoneNumber(const std::string & phoneNumber);

	/**
	 * Internal SIP URI that is going to be converted
	 * to different formats.
	 *
	 * A SIP URI is of the shapes:
	 * tanguy <sip:0145235854@wengo.fr>
	 * tanguy <sip:tanguy_k@wengo.fr>
	 * sip:tanguy_k@wengo.fr
	 *
	 * Display name: tanguy
	 * Phone number: 0145235854
	 * Mimimal SIP URI: sip:tanguy_k@wengo.fr
	 */
	std::string _sipUri;

	bool _isAPhoneNumber;
};

#endif	//SIPADDRESS_H
