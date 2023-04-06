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

#include <networkdiscovery/EnumProxyAuthType.h>

#include <util/Logger.h>

#include <QtCore/QMutex>
#include <QtCore/QString>

#include <map>

typedef std::map<EnumProxyAuthType::ProxyAuthType, std::string> ProxyAuthTypeMap;
static ProxyAuthTypeMap _proxyAuthTypeMap;
static QMutex _mutex;
 
static void init() {
	QMutexLocker lock(&_mutex);

	_proxyAuthTypeMap[EnumProxyAuthType::ProxyAuthTypeUnknown] = "ProxyAuthTypeUnknown";
	_proxyAuthTypeMap[EnumProxyAuthType::ProxyAuthTypeBasic] = "ProxyAuthTypeBasic";
	_proxyAuthTypeMap[EnumProxyAuthType::ProxyAuthTypeDigest] = "ProxyAuthTypeDigest";
	_proxyAuthTypeMap[EnumProxyAuthType::ProxyAuthTypeNTLM] = "ProxyAuthTypeNTLM";
}

std::string EnumProxyAuthType::toString(ProxyAuthType proxyAuthType) {
	init();

	QMutexLocker lock(&_mutex);
	std::string result = _proxyAuthTypeMap[proxyAuthType];
	if (result.empty()) {
		LOG_FATAL("unknown ProxyAuthType:" + QString::number(proxyAuthType).toStdString());
	}

	return result;
}

EnumProxyAuthType::ProxyAuthType EnumProxyAuthType::toProxyAuthType(const std::string & authType) {
	init();

	QMutexLocker lock(&_mutex);
	for (ProxyAuthTypeMap::const_iterator it = _proxyAuthTypeMap.begin();
		it != _proxyAuthTypeMap.end(); ++it) {

		if ((*it).second == authType) {
			return (*it).first;
		}
	}

	LOG_FATAL("unknown ProxyAuthType:" + authType);
	return ProxyAuthTypeUnknown;
}

EnumProxyAuthType::ProxyAuthType EnumProxyAuthType::toProxyAuthType(const EnumAuthType & authType) {
	ProxyAuthType result = ProxyAuthTypeUnknown;

	switch (authType) {
	case proxyAuthUnknown:
		result = ProxyAuthTypeUnknown;
		break;
	case proxyAuthBasic:
		result = ProxyAuthTypeBasic;
		break;
	case proxyAuthDigest:
		result = ProxyAuthTypeDigest;
		break;
	case proxyAuthNTLM:
		result = ProxyAuthTypeNTLM;
		break;
	default:
		LOG_FATAL("unknown EnumAuthType");
	}

	return result;
}
