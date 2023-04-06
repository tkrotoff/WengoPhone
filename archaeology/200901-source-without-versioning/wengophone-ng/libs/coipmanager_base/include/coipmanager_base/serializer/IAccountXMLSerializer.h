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

#ifndef OWIACCOUNTXMLSERIALIZER_H
#define OWIACCOUNTXMLSERIALIZER_H

#include <coipmanager_base/coipmanagerbasedll.h>

#include <string>

class IAccount;
class TiXmlHandle;

/**
 * Base class for Account serialization.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API IAccountXMLSerializer {
public:

	IAccountXMLSerializer(IAccount & account);

	virtual ~IAccountXMLSerializer();

	virtual std::string serialize() = 0;

	virtual bool unserialize(TiXmlHandle & rootElt) = 0;

protected:

	IAccount & _account;
};

#endif	//OWIACCOUNTXMLSERIALIZER_H
