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

#include "SipAddress.h"

#include "config/Authentication.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qregexp.h>

using namespace std;

static const QString SIP = "sip:";
static const QString AT = "@";

SipAddress::SipAddress(const std::string & sipUri, bool phoneNumber) {
	_isAPhoneNumber = phoneNumber;
	QString tmp(sipUri);

	//If the first character is <
	//then the SIP URI is like <sip:tanguy@krotoff.com>
	//and we just want sip:tanguy@krotoff.com
	if (tmp[0] == '<') {
		tmp = tmp.remove(QChar('>'));
		tmp = tmp.remove(QChar('<'));
	}

	_sipUri = tmp.ascii();
}

SipAddress SipAddress::fromPhoneNumber(const std::string & phoneNumber) {
	//Direct SIP address entered by the user
	QString tmp(phoneNumber);
	if (tmp.find(AT) != -1) {
		if (tmp.find(SIP) == -1) {
			tmp.prepend(SIP);
		}
		return SipAddress(tmp, false);
	}

	if (phoneNumber.empty()) {
		return SipAddress(string());
	}

	QString phoneNumberTmp(convertToValidPhoneNumber(phoneNumber));
	QString uri;

	if (phoneNumberTmp.find(SIP) == -1) {
		uri += SIP;
	}

	uri += phoneNumberTmp;

	if (phoneNumberTmp.find(AT) == -1 && phoneNumberTmp.find(SIP) == -1) {
		uri += AT;
		uri += Authentication::getInstance().getRealm();
	}

	return SipAddress(uri.ascii());
}

std::string SipAddress::getValidPhoneNumber() const {
	if (!_isAPhoneNumber) {
		return _sipUri;
	}

	QStringList afterSip = QStringList::split(SIP, _sipUri);
	QString tmp(afterSip[1]);
	if (tmp.isEmpty()) {
		tmp = afterSip[0];
	}
	QStringList beforeAt = QStringList::split(AT, tmp);
	if (beforeAt[0]) {
		return convertToValidPhoneNumber(beforeAt[0].ascii());
	} else {
		return "";
	}
}

std::string SipAddress::getDisplayName() const {
	QStringList beforeSip = QStringList::split(" <sip:", _sipUri);
	QString displayName = beforeSip[0];
	if (displayName == _sipUri) {
		return QString::null;
	}

	displayName = displayName.remove(QChar('"'));
	return displayName.ascii();
}

std::string SipAddress::getMinimalSipUri() const {
	QString uri;

	uri += SIP;
	QStringList afterSip = QStringList::split(SIP, _sipUri);
	QString tmp(afterSip[1]);
	if (tmp.isEmpty()) {
		tmp = afterSip[0];
	}
	uri += tmp;

	uri = uri.remove(QChar('>'));
	uri = uri.remove(QChar('<'));

	return uri.ascii();
}

std::string SipAddress::convertToValidPhoneNumber(const std::string & phoneNumber) {
	QString validPhoneNumber(phoneNumber);

	//Removes spaces / \ . : , ;
	validPhoneNumber = validPhoneNumber.remove(QChar(' '));
	/*validPhoneNumber = validPhoneNumber.remove(QChar('/'));
	validPhoneNumber = validPhoneNumber.remove(QChar('\\'));
	validPhoneNumber = validPhoneNumber.remove(QChar('.'));
	validPhoneNumber = validPhoneNumber.remove(QChar(':'));
	validPhoneNumber = validPhoneNumber.remove(QChar(','));
	validPhoneNumber = validPhoneNumber.remove(QChar(';'));

	//Removes (*)
	validPhoneNumber.replace(QRegExp("\\((.*)\\)"), "");*/

	//Replaces + by 00
	validPhoneNumber = validPhoneNumber.replace(QChar('+'), "00");

	return validPhoneNumber.ascii();
}
