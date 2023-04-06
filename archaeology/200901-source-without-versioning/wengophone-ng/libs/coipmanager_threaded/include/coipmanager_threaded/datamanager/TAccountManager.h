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

#ifndef OWTACCOUNTMANAGER_H
#define OWTACCOUNTMANAGER_H

#include <coipmanager_threaded/tcoipmanagerdll.h>

#include <coipmanager_base/EnumUpdateSection.h>
#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/account/AccountList.h>

#include <util/NonCopyable.h>

#include <QtCore/QObject>

class AccountManager;
class TCoIpManager;

/**
 * Threaded Account list manager.
 *
 * @see AccountManager
 * @ingroup TCoIpManager
 * @author Philippe Bernery
 */
class COIPMANAGER_THREADED_API TAccountManager : public QObject, NonCopyable {
	Q_OBJECT
public:

	TAccountManager(TCoIpManager & tCoIpManager);

	~TAccountManager();

	/**
	 * @see AccountManager::add
	 */
	bool add(const Account & account);

	/**
	 * @see AccountManager::remove
	 */
	bool remove(const std::string & accountId);

	/**
	 * @see AccountManager::update
	 */
	bool update(const Account & account,
		EnumUpdateSection::UpdateSection section = EnumUpdateSection::UpdateSectionUnknown);

Q_SIGNALS:

	/**
	 * @see AccountManager::accountAddedSignal
	 */
	void accountAddedSignal(std::string accountId);

	/**
	 * @see AccountManager::accountRemovedSignal
	 */
	void accountRemovedSignal(std::string accountId);

	/**
	 * @see AccountManager::accountUpdatedSignal
	 */
	void accountUpdatedSignal(std::string accountId, EnumUpdateSection::UpdateSection section);

private:

	AccountManager * getAccountManager();

	TCoIpManager & _tCoIpManager;
};

#endif	//OWTACCOUNTMANAGER_H
