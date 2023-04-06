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

#ifndef OWIAXACCOUNT_H
#define OWIAXACCOUNT_H

#include <coipmanager_base/account/SipAccount.h>

/**
 * IAX account.
 *
 * @author Yann Biancheri
 */
class COIPMANAGER_BASE_API IAXAccount : public SipAccount {
public:

	IAXAccount();

	IAXAccount(const std::string & login, const std::string & password);

	IAXAccount(const IAXAccount & account);

	explicit IAXAccount(const IAccount * iAccount);

	virtual ~IAXAccount();

	virtual IAXAccount * clone() const;

	virtual std::string getIdentity() const;

	virtual std::string getDisplayName() const;

protected:

	void copy(const IAXAccount & account);
};

#endif	//OWIAXACCOUNT_H
