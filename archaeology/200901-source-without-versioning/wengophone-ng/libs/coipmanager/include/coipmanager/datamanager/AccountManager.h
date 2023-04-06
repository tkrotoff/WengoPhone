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

#ifndef OWACCOUNTMANAGER_H
#define OWACCOUNTMANAGER_H

#include <coipmanager_base/EnumUpdateSection.h>
#include <coipmanager_base/account/AccountList.h>

#include <coipmanager/coipmanagerdll.h>

#include <QtCore/QMutex>
#include <QtCore/QObject>

class AccountMutator;

/**
 * Account list manager.
 *
 * It accesses an AccountList in a thread-safe manner.
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_API AccountManager : public QObject {
	Q_OBJECT
public:

	AccountManager(AccountList & accountList);

	~AccountManager();

	/**
	 * Adds an Account.
	 *
	 * Checks if the account is not already in the list.
	 *
	 * @return true if actually added (the Account was not present)
	 */
	bool add(const Account & account);

	/**
	 * Removes an Account.
	 *
	 * @return true if actually removed (the Account was present).
	 */
	bool remove(const std::string & accountId);

	/**
	 * Updates an Account.
	 *
	 * @param section used to specify which part of the Account has been updated.
	 *        This parameter is used for emitting accountUpdatedEvent and optimize
	 *        some part of the code. It is usually only specified by internal classes of
	 *        CoIpManager.
	 * @return true if actually update (the Account was updated).
	 */
	bool update(const Account & account,
		EnumUpdateSection::UpdateSection section = EnumUpdateSection::UpdateSectionUnknown);

Q_SIGNALS:

	/**
	 * Emitted when an Account has been added.
	 *
	 * @param accountId UUID of the added Account
	 */
	void accountAddedSignal(std::string accountId);

	/**
	 * Emitted when an Account has been removed.
	 *
	 * @param accountId UUID of the removed Account
	 */
	void accountRemovedSignal(std::string accountId);

	/**
	 * Emitted when an Account has been updated.
	 *
	 * @param accountId UUID of the updated Account
	 * @param section updated section
	 */
	void accountUpdatedSignal(std::string accountId, EnumUpdateSection::UpdateSection section);

private Q_SLOTS:

	void accountUpdatedSlot(Account account, EnumUpdateSection::UpdateSection section);

private:

	static AccountList::iterator findAccount(AccountList & accountList, const Account & account);

	AccountList & _accountList;

	QMutex * _mutex;
};

#endif	//OWACCOUNTMANAGER_H
