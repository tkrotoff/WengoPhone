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

#ifndef OWACCOUNTMUTATOR_H
#define OWACCOUNTMUTATOR_H

#include <coipmanager/coipmanagerdll.h>

#include <coipmanager_base/EnumUpdateSection.h>
#include <coipmanager_base/account/Account.h>

#include <util/Singleton.h>

#include <QtCore/QObject>

/**
 * Class that has no link to CoIpManager but that can update/change an Account
 * must inherit it and emit the accountUpdatedEvent when account has been changed.
 *
 * AccountManager class is registered to this event.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API AccountMutator : public QObject, public Singleton<AccountMutator> {
	Q_OBJECT
	friend class Singleton<AccountMutator>;
public:

	static AccountMutator & getInstance();

	/**
	 * @see accountUpdatedSignal
	 */
	static void emitAccountUpdatedSignal(Account account, EnumUpdateSection::UpdateSection section);

Q_SIGNALS:

	/**
	 * Event saying that Account has been modified and needs to be re-synchronized.
	 *
	 * @param account Account updated
	 * @param section which Account section has been modified
	 */
	void accountUpdatedSignal(Account account, EnumUpdateSection::UpdateSection section);
};

#endif //OWACCOUNTMUTATOR_H
