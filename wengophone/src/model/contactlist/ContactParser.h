/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef CONTACTPARSER_H
#define CONTACTPARSER_H

#include <model/imwrapper/EnumIMProtocol.h>

#include <string>

class Contact;
class TiXmlHandle;

/**
 * Parses the XML datas to create a Contact.
 *
 * This a separated class rather than to be inside the class Contact,
 * this allows more modularity and encapsulates the use of TinyXML.
 * What would be interresting is to create a separated XML VCard library that
 * uses TinyXML.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class ContactParser {
public:

	ContactParser(Contact & contact, const std::string & data);
	
private:

	void parseIMAccount(Contact & contact, const TiXmlHandle & handle, const std::string & protocolId, EnumIMProtocol::IMProtocol protocol);

};

#endif	//CONTACTPARSER_H
