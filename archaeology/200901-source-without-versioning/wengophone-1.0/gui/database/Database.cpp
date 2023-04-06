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

#include "Database.h"
using namespace database;

#include "Source.h"
#include "Target.h"
#include "VCardReader.h"
#include "VCardWriter.h"
#include "config/Config.h"
#include "config/Authentication.h"
#include "XmlConfigReader.h"
#include "XmlConfigWriter.h"
#include "contact/Contact.h"
#include "update/UpdateBuildId.h"
#include "XmlBuildIdReader.h"
#include "callhistory/CallHistory.h"
#include "XmlCallHistoryReader.h"
#include "XmlCallHistoryWriter.h"
#include "XmlAuthReader.h"
#include "XmlAuthWriter.h"
#include "XmlUserReader.h"
#include "XmlUserWriter.h"

#include <qdir.h>
#include <qstring.h>

Database::Database() {
}

Database::~Database() {
}

QString Database::extract(const Source & source) const {
	return source.read();
}

Contact & Database::transformFromVCard(const QString & data) const {
	VCardReader reader(data);
	return reader.getContact();
}

QString Database::transformToVCard(const Contact & contact) const {
	VCardWriter writer(contact);
	return writer.getData();
}

Config & Database::transformConfigFromXml(const QString & data) const {
	XmlConfigReader reader(data);
	return reader.getConfig();
}

QString Database::transformConfigToXml(const Config & config) const {
	XmlConfigWriter writer(config);
	return writer.getData();
}

UpdateBuildId Database::transformBuilIdFromXml(const QString & data) const {
	XmlBuildIdReader reader(data);
	return reader.getBuildId();
}

CallHistory & Database::transformCallHistoryFromXml(const QString & data) const {
	XmlCallHistoryReader reader(data);
	return reader.getCallHistory();
}

QString Database::transformCallHistoryToXml(const CallHistory & callHistory) const {
	XmlCallHistoryWriter writer(callHistory);
	return writer.getData();
}

QString Database::transformAuthToXml(const Authentication & auth) const {
	XmlAuthWriter writer(auth);
	return writer.getData();
}

Authentication & Database::transformAuthFromXml(const QString & data) const {
	XmlAuthReader reader(data);
	return reader.getAuth();
}

QString Database::transformUserToXml(const User & user) const {
	XmlUserWriter writer(user);
	return writer.getData();
}

User Database::transformUserFromXml(const QString & data) const {
	XmlUserReader reader(data);
	return reader.getUser();
}

void Database::load(const QString & data, Target & target) const {
	target.write(data);
}
