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

#ifndef IMWRAPPERFACTORY_H
#define IMWRAPPERFACTORY_H

#include <Interface.h>

class IMAccount;
class IMChat;
class IMConnect;
class IMPresence;

/**
 * Factories for the Instant Messaging wrapper component.
 *
 * Depending on the factory instanciated, the IM implementation component will use Gaim
 * or what ever you prefer.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class IMWrapperFactory : Interface {
public:

	virtual ~IMWrapperFactory() {
	}

	/**
	 * Sets the IM implementation to use.
	 *
	 * @param factory factory to create the SIP implementation component
	 */
	static void setFactory(IMWrapperFactory * factory) {
		_factory = factory;
	}

	/**
	 * Gets the factory that will instanciate a IM implementation plugin.
	 *
	 * @return IM implementation
	 */
	static IMWrapperFactory & getFactory() {
		return *_factory;
	}

	/**
	 * Instantiates a IM connect implementation.
	 *
	 * @param account IM account
	 * @return IM connect implementation instance
	 */
	virtual IMConnect * createIMConnect(IMAccount & account) = 0;

	/**
	 * Instantiates a IM chat implementation.
	 *
	 * @param account IM account
	 * @return IM chat implementation instance
	 */
	virtual IMChat * createIMChat(IMAccount & account) = 0;

	/**
	 * Instantiates a IM presence implementation.
	 *
	 * @param account IM account
	 * @return IM presence implementation instance
	 */
	virtual IMPresence * createIMPresence(IMAccount & account) = 0;

private:

	static IMWrapperFactory * _factory;
};

#endif	//IMWRAPPERFACTORY_H
