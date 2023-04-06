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

#include "XmlWatcherListReader.h"

#include <qdom.h>
#include <qstring.h>

using namespace std;

XmlWatcherListReader::XmlWatcherListReader(const std::string & data) {
	static const string WATCHERLIST_TAG = "watcher-list";

	QString errorMsg;
	int errorLine = 0;
	int errorColumn = 0;
	QDomDocument doc;
	if (!doc.setContent(data, &errorMsg, &errorLine, &errorColumn)) {
		return;
	}

	//Print out the element names of all elements that are direct
	//children of the outermost element
	QDomElement docElem = doc.documentElement();

	QDomNode node = docElem.firstChild();
	while (!node.isNull()) {
		//Try to convert the node to an element
		QDomElement element = node.toElement();

		if (!element.isNull()) {
			//The node really is an element

			if (element.tagName() == WATCHERLIST_TAG) {
				parseWatcherList(element.toElement());
			}
		}
		node = node.nextSibling();
	}
}

void XmlWatcherListReader::parseWatcherList(const QDomElement & element) {
	static const string WATCHER_TAG = "watcher";

	QDomNode node = element.firstChild();
	while (!node.isNull()) {
		QDomElement elem = node.toElement();
		if (elem.tagName() == WATCHER_TAG) {
			_watcherList.addWatcher(parseWatcher(elem));
		}
		node = node.nextSibling();
	}
}

Watcher * XmlWatcherListReader::parseWatcher(const QDomElement & element) {
	static const string WATCHER_STATUS_TAG = "status";
	static const string WATCHER_STATUS_PENDING_TAG = "pending";
	static const string WATCHER_DISPLAYNAME_TAG = "display_name";

	string status = element.attribute(WATCHER_STATUS_TAG);
	string displayName/* = element.attribute(WATCHER_DISPLAYNAME_TAG)*/;
	string sipUri = element.text();
	SipAddress sipAddress(displayName + " " + sipUri);

	return new Watcher(sipAddress, status == WATCHER_STATUS_PENDING_TAG);
}
