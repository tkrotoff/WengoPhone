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

#ifndef PHAPIFACTORY_H
#define PHAPIFACTORY_H

#include <sipwrapper/SipWrapperFactory.h>
#include <imwrapper/IMWrapperFactory.h>

class IMAccount;
class IMConnect;
class IMChat;
class IMPresence;
class PhApiWrapper;

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class PhApiFactory : public SipWrapperFactory, public IMWrapperFactory {
public:

	PhApiFactory();

	SipWrapper * createSipWrapper();

	IMConnect * createIMConnect(IMAccount & account);

	IMChat * createIMChat(IMAccount & account);

	IMPresence * createIMPresence(IMAccount & account);

	IMContactList * createIMContactList(IMAccount & account);

private:

	PhApiWrapper * _phApiWrapperInstance;
};

#endif	//PHAPIFACTORY_H
