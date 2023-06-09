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

#include <imwrapper/IMAccount.h>

#include <settings/SettingsXMLSerializer.h>

using namespace std;

IMAccount::IMAccount() 
	: Account(EnumIMProtocol::IMProtocolUnknown) {
	_connected = false;
	_presenceState = EnumPresenceState::PresenceStateOnline;
	_imAccountParameters.valueChangedEvent +=
		boost::bind(&IMAccount::valueChangedEventHandler, this, _1, _2);
}

IMAccount::IMAccount(const std::string & login, const std::string & password, EnumIMProtocol::IMProtocol protocol) 
	: Account(protocol) {
	_connected = false;
	_presenceState = EnumPresenceState::PresenceStateOnline;
	_login = correctedLogin(login, protocol);
	_password = password;
	_imAccountParameters.valueChangedEvent +=
		boost::bind(&IMAccount::valueChangedEventHandler, this, _1, _2);
}

IMAccount::IMAccount(const IMAccount & imAccount) 
	: Account(imAccount) {
	copy(imAccount);
}

IMAccount & IMAccount::operator = (const IMAccount & imAccount) {
	Account::copy((const Account &) imAccount);
	copy(imAccount);
	return *this;
}

Account * IMAccount::createCopy() const {
	Account * result = new IMAccount();

	copyTo(result);

	return result;
}

void IMAccount::copyTo(Account * account) const {
	Account::copyTo(account);
	IMAccount * imAccount = dynamic_cast<IMAccount *>(account);
	imAccount->_connected = _connected;
	imAccount->_presenceState = _presenceState;
	imAccount->_login = _login;
	imAccount->_password = _password;
	imAccount->_imAccountParameters = _imAccountParameters;
}

void IMAccount::copy(const IMAccount & imAccount) {
	_connected = imAccount._connected;
	_presenceState = imAccount._presenceState;
	_login = imAccount._login;
	_password = imAccount._password;
	_imAccountParameters = imAccount._imAccountParameters;
	//FIXME: we should copy the events
	_imAccountParameters.valueChangedEvent +=
		boost::bind(&IMAccount::valueChangedEventHandler, this, _1, _2);
}
	
IMAccount::~IMAccount() {
	imAccountWillDieEvent(*this);
	imAccountDeadEvent(*this);
}
	
bool IMAccount::operator == (const IMAccount & imAccount) const {
	return ((_login == imAccount._login)
			&& (_protocol == imAccount._protocol));
}

bool IMAccount::operator < (const IMAccount & imAccount) const {
	return ((_login < imAccount._login)
			|| ((_login == imAccount._login) && (_protocol < imAccount._protocol)));
}

bool IMAccount::empty() const {
	return _login.empty() 
		&& _password.empty() 
		&& (_protocol == EnumIMProtocol::IMProtocolUnknown);
}

void IMAccount::setLogin(const std::string & login) {
	_login = correctedLogin(login, _protocol);
	imAccountChangedEvent(*this);
}

void IMAccount::setPassword(const string & password) {
	_password = password;
	imAccountChangedEvent(*this);
}

string IMAccount::correctedLogin(const string & login, EnumIMProtocol::IMProtocol protocol) {
	string result = login;

	if (protocol == EnumIMProtocol::IMProtocolJabber) {
		string::size_type index = login.find('/');

		if (index == string::npos) {
			result += "/WengoPhone";
		}
	}

	return result;
}

void IMAccount::valueChangedEventHandler(Settings & sender, const std::string & key) {
	imAccountChangedEvent(*this);
}
