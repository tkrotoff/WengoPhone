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

#ifndef GAIMIMFACTORY_H
#define GAIMIMFACTORY_H

#include <imwrapper/IMWrapperFactory.h>
#include <imwrapper/IMAccount.h>
#include "GaimChatMngr.h"
#include "GaimConnectMngr.h"
#include "GaimPresenceMngr.h"
#include "GaimContactListMngr.h"
/**
 *
 * @ingroup model
 * @author Julien Bossart
 */
class GaimIMFactory : public IMWrapperFactory {
public:

	GaimIMFactory();

	virtual IMConnect *createIMConnect(IMAccount & account);

	virtual IMChat *createIMChat(IMAccount & account);

	virtual IMPresence *createIMPresence(IMAccount & account);

	virtual IMContactList *createIMContactList(IMAccount & account);

private:

	GaimConnectMngr *ConnectMngr;
	GaimPresenceMngr *PresenceMngr;
	GaimChatMngr *ChatMngr;
	GaimContactListMngr *ContactListMngr;

	static void GaimIMInit();
};

#endif	//GAIMIMFACTORY_H
