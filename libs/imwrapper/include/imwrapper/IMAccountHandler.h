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

#ifndef IMACCOUNTHANDLER_H
#define IMACCOUNTHANDLER_H

#include <imwrapper/EnumIMProtocol.h>
#include <imwrapper/IMAccount.h>

#include <set>
#include <string>

/**
 * Instant Messaging account list.
 *
 * @author Philippe Bernery
 */
class IMAccountHandler : public std::set<IMAccount> {
public:

	/**
	 * @param protocol the desired protocol
	 * @return IMAccount with protocol 'protocol'
	 */
	std::set<IMAccount *> getIMAccountsOfProtocol(EnumIMProtocol::IMProtocol protocol);

	void removeIMAccount( const IMAccount & imaccount) {};

};

#endif	//IMACCOUNTHANDLER_H
