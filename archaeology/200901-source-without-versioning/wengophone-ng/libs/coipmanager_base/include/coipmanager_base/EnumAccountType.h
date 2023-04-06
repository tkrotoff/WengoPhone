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

#ifndef OWENUMACCOUNTTYPE_H
#define OWENUMACCOUNTTYPE_H

#include <coipmanager_base/coipmanagerbasedll.h>

#include <util/NonCopyable.h>

#include <string>

/**
 * WengoPhone supported AccountTypes.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API EnumAccountType : NonCopyable {
public:

	enum AccountType {
		AccountTypeUnknown,
		AccountTypeAIM,
		AccountTypeGTalk,
		AccountTypeIAX,
		AccountTypeICQ,
		AccountTypeJabber,
		AccountTypeMSN,
		AccountTypeSIP,
		AccountTypeWengo,
		AccountTypeYahoo,
	};

	/**
	 * Converts a AccountType into a string.
	 *
	 * @return the string
	 */
	static std::string toString(AccountType AccountType);

	/**
	 * Converts a string into a AccountType.
	 *
	 * @return the AccountType
	 */
	static AccountType toAccountType(const std::string & accountType);
};

#endif	//OWENUMACCOUNTTYPE_H
