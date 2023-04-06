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

#include "XmlBuildIdReader.h"
using namespace database;

#include <qdom.h>
#include <qstring.h>

#include <iostream>
using namespace std;
#include <cassert>

XmlBuildIdReader::XmlBuildIdReader(const QString & data) {
	Q_ULLONG buildId = 0;
	QString type;
	QString name;
	QString url;

	QDomDocument doc;
	if (!doc.setContent(data)) {
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

			if (element.tagName() == "version") {
				node = element.firstChild();
				while (!node.isNull()) {
					element = node.toElement();
					if (element.tagName() == "build") {
						buildId = element.text().toULongLong();
						assert(buildId != 0 && "Build Id number is incorrect: it's not a number");
					}
					if (element.tagName() == "url") {
						url = element.text();
					}
					if (element.tagName() == "type") {
						type = element.text();
					}
					if (element.tagName() == "name") {
						name = element.text();
					}
					node = node.nextSibling();
				}
			}
		}
		node = node.nextSibling();
	}
	_buildId = UpdateBuildId(buildId, url, type, name);
}
