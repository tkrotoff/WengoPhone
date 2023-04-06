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

#include "XmlCallHistoryReader.h"
using namespace database;

#include <qdom.h>
#include <qstring.h>

#include <iostream>
using namespace std;

XmlCallHistoryReader::XmlCallHistoryReader(const QString & data) {
	QDomDocument doc("CallHistory");
	if (!doc.setContent(data)) {
		return;
	}

	_callHistory = &CallHistory::getInstance();

	//Print out the element names of all elements that are direct
	//children of the outermost element
	QDomElement docElem = doc.documentElement();

	QDomNode node = docElem.firstChild();
	while (!node.isNull()) {
		//Try to convert the node to an element
		QDomElement element = node.toElement();

		if (!element.isNull()) {
			//The node really is an element

			if (element.tagName() == "callLog") {
				CallLog * callLog = new CallLog();
				QDomNode node2 = element.firstChild();

				while (!node2.isNull()) {
					//Try to convert the node to an element
					QDomElement element2 = node2.toElement();

					if (!element2.isNull()) {
						//The node really is an element

						if (element2.tagName() == "type") {
							QString type = element2.text();

							//For compatibility with previous versions
							if (!type.contains("Call")
								&& !type.contains("Sms")) {
									type.prepend("Call");
							}
							callLog->setType(type);
						}
						if (element2.tagName() == "dateTime") {
							callLog->setDateTime(QDateTime::fromString(element2.text(), Qt::ISODate));
						}
						if (element2.tagName() == "contactName") {
							callLog->setContactName(element2.text());
						}
						if (element2.tagName() == "duration") {
							callLog->setDuration(element2.text().toULong());
						}
						if (element2.tagName() == "phoneNumber") {
							callLog->setPhoneNumber(element2.text());
						}
						if (element2.tagName() == "sms") {
							callLog->setSms(element2.text());
						}
					}
					node2 = node2.nextSibling();
				}

				_callHistory->addCallLog(*callLog, false);
			}
		}
		node = node.nextSibling();
	}
}
