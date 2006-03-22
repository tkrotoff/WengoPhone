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

#ifndef WENGOACCOUNTSERIALIZER_H
#define WENGOACCOUNTSERIALIZER_H

#include "WengoAccount.h"

#include <util/Base64.h>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/archive_exception.hpp>

class WengoAccountSerializer {
public:

	static const unsigned int SERIALIZATION_VERSION = 1;

	WengoAccountSerializer(WengoAccount & wengoAccount)
		: _wengoAccount(wengoAccount) { }

private:

	friend class boost::serialization::access;

	template < class Archive >
	void load(Archive & ar, const unsigned int version) {
		if (version == SERIALIZATION_VERSION) {
			std::string login;
			std::string password;
			bool autoLogin;

			ar >> BOOST_SERIALIZATION_NVP(login);
			ar >> BOOST_SERIALIZATION_NVP(password);
			ar >> BOOST_SERIALIZATION_NVP(autoLogin);

			_wengoAccount._wengoLogin = login;
			_wengoAccount._wengoPassword = Base64::decode(password);
			_wengoAccount._autoLogin = autoLogin;
		}
	}

	template < class Archive >
	void save(Archive & ar, const unsigned int version) const {
		if (version == SERIALIZATION_VERSION) {
			std::string login = _wengoAccount._wengoLogin;
			std::string password = Base64::encode(_wengoAccount._wengoPassword);
			bool autoLogin = _wengoAccount._autoLogin;

			ar << BOOST_SERIALIZATION_NVP(login);
			ar << BOOST_SERIALIZATION_NVP(password);
			ar << BOOST_SERIALIZATION_NVP(autoLogin);
		}
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER()

	WengoAccount & _wengoAccount;
};

BOOST_CLASS_VERSION(WengoAccountSerializer, WengoAccountSerializer::SERIALIZATION_VERSION)

#endif	//WENGOACCOUNTSERIALIZER_H
