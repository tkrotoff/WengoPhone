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

#ifndef OWYAHOOACCOUNT_H
#define OWYAHOOACCOUNT_H

#include <coipmanager_base/account/IAccount.h>

/**
 * Yahoo account.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API YahooAccount : public IAccount {
public:

	/**
	 * @name defaults Yahoo parameters
	 * @{
	 */
	static const std::string DEFAULT_YAHOO_SERVER;

	static const std::string DEFAULT_YAHOO_JAPAN_SERVER;

	static const int DEFAULT_YAHOO_PORT;

	static const std::string DEFAULT_YAHOO_XFER_HOST;

	static const std::string DEFAULT_YAHOO_JAPAN_XFER_HOST;

	static const int DEFAULT_YAHOO_XFER_PORT;

	static const std::string DEFAULT_YAHOO_ROOM_LIST_LOCALE;
	/** @} */

	YahooAccount();

	YahooAccount(const std::string & login, const std::string & password);

	YahooAccount(const YahooAccount & account);

	explicit YahooAccount(const IAccount * iAccount);

	virtual ~YahooAccount();

	virtual YahooAccount * clone() const;

	void setServer(const std::string & yahooServer);
	std::string getServer() const;

	void setServerPort(int yahooServerPort);
	int getServerPort() const;

	void setYahooJapan(bool isYahooJapan);
	bool isYahooJapan() const;

	void setXferServer(const std::string & xferServer);
	std::string getXferHost() const;

	void setXferPort(int xferPort);
	int getXferPort() const;

	void setRoomListLocale(const std::string & locale);
	std::string getRoomListLocale() const;

protected:

	void copy(const YahooAccount & account);

	/** Yahoo server. */
	std::string _yahooServer;

	/** Yahoo server port. */
	int _yahooServerPort;

	/** Is Yahoo Japan used? */
	bool _isYahooJapan;

	/** Transfer server. */
	std::string _xferServer;

	/** Transfer port. */
	int _xferPort;

	/** Room list locale. */
	std::string _roomListLocale;

};

#endif	//OWYAHOOACCOUNT_H
