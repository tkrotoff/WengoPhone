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

#ifndef XMLWATCHERLISTREADER_H
#define XMLWATCHERLISTREADER_H

#include "WatcherList.h"

#include <NonCopyable.h>

#include <string>

class QDomElement;

/**
 * Reads the WatcherList from a XML stream
 * and creates a WatcherList object.
 *
 * Example of a XML stream:
 * <code>
 * <?xml version="1.0"?>
 * <watcherinfo xmlns="urn:ietf:params:xml:ns:watcherinfo" version="0" state="partial">
 *   <watcher-list resource="sip:wtest4@len1.host.wengo.fr" package="presence.winfo">
 *     <watcher status="active" event="subscribe" id="" display_name="toto">sip:toto@wengo.fr</watcher>
 *     <watcher status="pending" event="subscribe" id="" display_name="leboulet">sip:leboulet@wengo.fr</watcher>
 *   </watcher-list>
 * </watcherinfo>
 * </code>
 *
 * @author Tanguy Krotoff
 */
class XmlWatcherListReader : NonCopyable {
public:

	/**
	 * Parses the XML stream to get the WatcherList.
	 *
	 * @param data XML stream represented as a string
	 */
	XmlWatcherListReader(const std::string & data);

	~XmlWatcherListReader() {
	}

	/**
	 * Gets the WatcherList from the XML stream.
	 *
	 * @return the WatcherList
	 */
	const WatcherList & getWatcherList() const {
		return _watcherList;
	}

private:

	/**
	 * Parses the XML corresponding to WatcherList.
	 */
	void parseWatcherList(const QDomElement & element);

	/**
	 * Parses the XML corresponding to Watcher.
	 */
	Watcher * parseWatcher(const QDomElement & element);

	/**
	 * WatcherList object that represents the XML stream in memory.
	 */
	WatcherList _watcherList;
};

#endif	//XMLWATCHERLISTREADER_H
