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

#ifndef GAIMIMCONNECT_H
#define GAIMIMCONNECT_H

//#include "GaimIMFactory.h"

#include <imwrapper/IMAccount.h>
#include <imwrapper/IMConnect.h>
class IMAccountParameters;

/**
 * Gaim IM connection.
 *
 * @ingroup model
 * @author Julien Bossart
 */
class GaimIMConnect : public IMConnect {
	friend class GaimIMFactory;
public:

	GaimIMConnect(IMAccount &account);

	void connect();

	void disconnect();

	bool equalsTo(std::string login, EnumIMProtocol::IMProtocol protocol);


private:

	void *CreateAccount(void);
	void InitConn(void);

	void GaimIMConnect::AddAccountParams(void *gaimAccount);
	void AddMSNAccountParams(void *gaimAccount, IMAccountParameters & mParams);
	void AddYahooAccountParams(void *gaimAccount, IMAccountParameters & mParams);
	void AddOscarAccountParams(void *gaimAccount, IMAccountParameters & mParams);
	void AddJabberAccountParams(void *gaimAccount, IMAccountParameters & mParams);
};

#endif	//GAIMIMCONNECT_H
