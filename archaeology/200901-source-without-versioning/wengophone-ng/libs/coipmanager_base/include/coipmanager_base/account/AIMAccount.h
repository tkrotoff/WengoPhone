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

#ifndef OWAIMACCOUNT_H
#define OWAIMACCOUNT_H

#include <coipmanager_base/account/OscarAccount.h>

/**
 * AIM account.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API AIMAccount : public OscarAccount {
public:

	AIMAccount();

	AIMAccount(const std::string & login, const std::string & password);

	AIMAccount(const AIMAccount & account);

	explicit AIMAccount(const IAccount * iAccount);

	virtual ~AIMAccount();

	virtual AIMAccount * clone() const;

protected:

	void copy(const AIMAccount & account);
};

#endif	//OWAIMACCOUNT_H
