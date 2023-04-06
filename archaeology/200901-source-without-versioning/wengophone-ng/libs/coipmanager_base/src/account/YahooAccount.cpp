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

#include <coipmanager_base/account/YahooAccount.h>

#include <util/Logger.h>
#include <util/String.h>

const std::string YahooAccount::DEFAULT_YAHOO_SERVER = "scs.msg.yahoo.com";
const std::string YahooAccount::DEFAULT_YAHOO_JAPAN_SERVER = "cs.yahoo.co.jp";
const int YahooAccount::DEFAULT_YAHOO_PORT = 5050;
const std::string YahooAccount::DEFAULT_YAHOO_XFER_HOST = "filetransfer.msg.yahoo.com";
const std::string YahooAccount::DEFAULT_YAHOO_JAPAN_XFER_HOST = "filetransfer.msg.yahoo.co.jp";
const int YahooAccount::DEFAULT_YAHOO_XFER_PORT = 80;
const std::string YahooAccount::DEFAULT_YAHOO_ROOM_LIST_LOCALE = "us";

YahooAccount::YahooAccount()
	: IAccount(String::null, String::null, EnumAccountType::AccountTypeYahoo) {
	_yahooServer = DEFAULT_YAHOO_SERVER;
	_yahooServerPort = DEFAULT_YAHOO_PORT;
	_isYahooJapan = false;
	_xferServer = DEFAULT_YAHOO_XFER_HOST;
	_xferPort = DEFAULT_YAHOO_XFER_PORT;
	_roomListLocale = DEFAULT_YAHOO_ROOM_LIST_LOCALE;
}

YahooAccount::YahooAccount(const std::string & login, const std::string & password)
	: IAccount(login, password, EnumAccountType::AccountTypeYahoo) {

	_yahooServer = DEFAULT_YAHOO_SERVER;
	_yahooServerPort = DEFAULT_YAHOO_PORT;
	_isYahooJapan = false;
	_xferServer = DEFAULT_YAHOO_XFER_HOST;
	_xferPort = DEFAULT_YAHOO_XFER_PORT;
	_roomListLocale = DEFAULT_YAHOO_ROOM_LIST_LOCALE;
}

YahooAccount::YahooAccount(const YahooAccount & account)
	: IAccount(account) {
	copy(account);
}

YahooAccount::YahooAccount(const IAccount * iAccount) {
	const YahooAccount * account = dynamic_cast<const YahooAccount *>(iAccount);
	if (account) {
		copy(*account);
	} else {
		LOG_FATAL("given IAccount is not an YahooAccount");
	}
}

void YahooAccount::copy(const YahooAccount & account) {
	IAccount::copy(account);
	_yahooServer = account._yahooServer;
	_yahooServerPort = account._yahooServerPort;
	_isYahooJapan = account._isYahooJapan;
	_xferServer = account._xferServer;
	_xferPort = account._xferPort;
	_roomListLocale = account._roomListLocale;
}

YahooAccount::~YahooAccount() {
}

YahooAccount * YahooAccount::clone() const {
	return new YahooAccount(*this);
}

void YahooAccount::setServer(const std::string & yahooServer) {
	_yahooServer = yahooServer;
}

std::string YahooAccount::getServer() const {
	return _yahooServer;
}

void YahooAccount::setServerPort(int yahooServerPort) {
	_yahooServerPort = yahooServerPort;
}

int YahooAccount::getServerPort() const {
	return _yahooServerPort;
}

void YahooAccount::setYahooJapan(bool isYahooJapan) {
	_isYahooJapan = isYahooJapan;
	_yahooServerPort = DEFAULT_YAHOO_PORT;
	_xferPort = DEFAULT_YAHOO_XFER_PORT;

	if (_isYahooJapan) {
		_yahooServer = DEFAULT_YAHOO_JAPAN_SERVER;
		_xferServer = DEFAULT_YAHOO_JAPAN_XFER_HOST;
	} else {
		_yahooServer = DEFAULT_YAHOO_SERVER;
		_xferServer = DEFAULT_YAHOO_XFER_HOST;
	}
}

bool YahooAccount::isYahooJapan() const {
	return _isYahooJapan;
}

void YahooAccount::setXferServer(const std::string & xferServer) {
	_xferServer = xferServer;
}

std::string YahooAccount::getXferHost() const {
	return _xferServer;
}

void YahooAccount::setXferPort(int xferPort) {
	_xferPort = xferPort;
}

int YahooAccount::getXferPort() const {
	return _xferPort;
}

void YahooAccount::setRoomListLocale(const std::string & locale) {
	_roomListLocale = locale;
}

std::string YahooAccount::getRoomListLocale() const {
	return _roomListLocale;
}
