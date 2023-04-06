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

#include "XmlCallHistoryWriter.h"
using namespace database;

#include "../config/Config.h"

#include <qdom.h>
#include <qstring.h>

#include <iostream>
using namespace std;

XmlCallHistoryWriter::XmlCallHistoryWriter(const CallHistory & callHistory) {
	QDomDocument doc("CallHistory");
	QDomElement root = doc.createElement("CallHistory");
	doc.appendChild(root);

	for (unsigned int i = 0; i < callHistory.size(); i++) {
		const CallLog & callLog = callHistory[i];
		root.appendChild(createElement(doc, callLog));
	}

	_data = doc.toString();
}

QDomElement XmlCallHistoryWriter::createElement(QDomDocument & doc, const CallLog & callLog) {
	QDomElement callLogTag = doc.createElement("callLog");

	QDomElement typeTag = doc.createElement("type");
	callLogTag.appendChild(typeTag);
	QDomText typeText = doc.createTextNode(callLog.getTypeString());
	typeTag.appendChild(typeText);

	QDomElement dateTimeTag = doc.createElement("dateTime");
	callLogTag.appendChild(dateTimeTag);
	QDomText dateTimeText = doc.createTextNode(callLog.getDateTime().toString(Qt::ISODate));
	dateTimeTag.appendChild(dateTimeText);

	QDomElement contactNameTag = doc.createElement("contactName");
	callLogTag.appendChild(contactNameTag);
	QDomCDATASection contactNameText = doc.createCDATASection(callLog.getContactName().utf8());
	contactNameTag.appendChild(contactNameText);
	/*QDomText contactNameText = doc.createTextNode(callLog.getContactName());
	contactNameTag.appendChild(contactNameText);*/

	QDomElement durationTag = doc.createElement("duration");
	callLogTag.appendChild(durationTag);
	QDomText durationText = doc.createTextNode(QString::number(callLog.getDurationForXml()));
	durationTag.appendChild(durationText);

	QDomElement phoneNumberTag = doc.createElement("phoneNumber");
	callLogTag.appendChild(phoneNumberTag);
	QDomText phoneNumberText = doc.createTextNode(callLog.getPhoneNumber());
	phoneNumberTag.appendChild(phoneNumberText);

	QDomElement smsTag = doc.createElement("sms");
	callLogTag.appendChild(smsTag);
	QDomCDATASection smsText = doc.createCDATASection(callLog.getSms().utf8());
	smsTag.appendChild(smsText);
	/*QDomText smsText = doc.createTextNode(callLog.getSms());
	smsTag.appendChild(smsText);*/

	return callLogTag;
}
