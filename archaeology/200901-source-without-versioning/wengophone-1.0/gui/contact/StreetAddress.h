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

#ifndef STREETADDRESS_H
#define STREETADDRESS_H

#include <qstring.h>

/**
 * Class for the living address of a Contact.
 *
 * @author Tanguy Krotoff
 */
class StreetAddress {
public:

	StreetAddress() {
	}

	StreetAddress(const StreetAddress & streetAddress) {
		setStreet(streetAddress.getStreet());
		setStateProvince(streetAddress.getStateProvince());
		setCity(streetAddress.getCity());
		setZipCode(streetAddress.getZipCode());
		setCountry(streetAddress.getCountry());
	}

	~StreetAddress() {
	}

	StreetAddress & operator=(const StreetAddress & streetAddress) {
		if (&streetAddress != this) {
			setStreet(streetAddress.getStreet());
			setStateProvince(streetAddress.getStateProvince());
			setCity(streetAddress.getCity());
			setZipCode(streetAddress.getZipCode());
			setCountry(streetAddress.getCountry());
		}
		return *this;
	}

	void setStreet(const QString & street) { _street = street; }
	QString getStreet() const { return _street; }

	void setStateProvince(const QString & stateProvince) { _stateProvince = stateProvince; }
	QString getStateProvince() const { return _stateProvince; }

	void setCity(const QString & city) { _city = city; }
	QString getCity() const { return _city; }

	void setZipCode(const QString & zipCode) { _zipCode = zipCode; }
	QString getZipCode() const { return _zipCode; }

	void setCountry(const QString & country) { _country = country; }
	QString getCountry() const { return _country; }

private:

	QString _street;
	QString _stateProvince;
	QString _city;
	QString _zipCode;
	QString _country;
};

#endif	//STREETADDRESS_H
