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

#ifndef OWPEERXMLSERIALIZER_H
#define OWPEERXMLSERIALIZER_H

#include <coipmanager_base/coipmanagerbasedll.h>

#include <serialization/Serializable.h>

#include <string>

class Peer;
class TiXmlHandle;

/**
 * Serialize a Peer object.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API PeerXMLSerializer : public Serializable {
public:

	/**
	 * @param rootEltName this is the name of the root element.
	 * Is must be given because the 'accountType' attribute is saved as
	 * an XML attribute, not an element.
	 * e.g: "<im accountType='accountType'/>"
	 */
	PeerXMLSerializer(Peer & peer, const std::string & rootEltName);

	/**
	 * This serialize method only start the serialization. It will not close
	 * the XML stream with _rootEltName, thus sub-class can add more data
	 * to the stream.
	 */
	virtual std::string serialize();

	bool unserializeContent(TiXmlHandle & rootElt);

protected:

	virtual bool unserialize(const std::string & data);

	Peer & _peer;

	std::string _rootEltName;
};

#endif	//OWPEERXMLSERIALIZER_H
