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

#include "AdvancedConfig.h"

#include <observer/Event.h>

#include <qstring.h>

const unsigned short AdvancedConfig::NB_NAT_CONFIG = 6;
const char * AdvancedConfig::AUTOMATIC = "auto";
const char * AdvancedConfig::NONE = "none";
const char * AdvancedConfig::FULL_CONE = "fcone";
const char * AdvancedConfig::RESTRICT_CONE = "rcone";
const char * AdvancedConfig::PORT_RESTRICT_CONE = "prcone";
const char * AdvancedConfig::SYMMETRIC = "sym";

const QString AdvancedConfig::ADVANCEDCONFIG_TAG = "advancedConfig";
const QString AdvancedConfig::SIPPORT_TAG = "sipPort";
const QString AdvancedConfig::NAT_TAG = "nat";
const QString AdvancedConfig::ECHO_CANCELLER_TAG = "echoCanceller";
const QString AdvancedConfig::USE_HTTP_PROXY_TAG = "useHttpProxy";
const QString AdvancedConfig::USE_PROXY_AUTHENTICATION = "proxyAuthentication";
const QString AdvancedConfig::USE_SSL = "ssl";
const QString AdvancedConfig::HTTP_PROXY_URL_TAG = "httpProxyUrl";
const QString AdvancedConfig::HTTP_PROXY_PORT_TAG = "httpProxyPort";
const QString AdvancedConfig::HTTP_PROXY_USERID_TAG = "httpProxyUserID";
const QString AdvancedConfig::HTTP_PROXY_USERPASSWORD_TAG = "httpProxyUserPassword";

const QString AdvancedConfig::MAINWINDOWX_TAG = "x";
const QString AdvancedConfig::MAINWINDOWY_TAG = "y";
const QString AdvancedConfig::MAINWINDOWWIDTH_TAG = "width";
const QString AdvancedConfig::MAINWINDOWHEIGHT_TAG = "height";
const QString AdvancedConfig::AUTOSTART_TAG = "autostart";

AdvancedConfig::AdvancedConfig() {
	defaultSettings();
}

/**
 * @param sipPort
 * @brief set sip port
 */
void AdvancedConfig::setSipPort(const QString & sipPort) {
	_sipPort = sipPort;
	notify(Event());
}

/**
 * @return sip port used
 */
QString AdvancedConfig::getSipPort() const {
	return _sipPort;
}

/**
 * @param nat
 * @brief set NAT type
 */
void AdvancedConfig::setNat(const QString & nat) {
	_nat = nat;
	notify(Event());
}

/**
 * @return NAT type
 */
QString AdvancedConfig::getNat() const {
	return _nat;
}

void AdvancedConfig::setEchoCanceller(bool echoCanceller) {
	_echoCanceller = echoCanceller;
	notify(Event());
}

/**
 * @brief set default settings
 */
void AdvancedConfig::defaultSettings() {
  	_sipPort = "5060";
	_nat = NONE;
	_echoCanceller = true;
	_x = 100;
	_y = 100;
	_width = 350;
	_height = 500;
	_ssl = true;	//By default SSL is enabled
	_useHttpProxy = false;
	_autoStart = true;
	notify(Event());
}

/**
 * @return if advanced settings are valid
 */
bool AdvancedConfig::isValid() const {
	bool ok;

	//Test port
	if (_sipPort.isEmpty() || _sipPort.length() >= 6) {
		return false;
	}

	_sipPort.toLong(&ok, 10);
	if (!ok) {
		return false;
	}

	//Tests nat
	if (_nat != AUTOMATIC
		&& _nat != NONE
		&& _nat != FULL_CONE
		&& _nat != RESTRICT_CONE
		&& _nat != PORT_RESTRICT_CONE
		&& _nat != SYMMETRIC) {

		return false;
	}

	return true;
}

void AdvancedConfig::setUseHttpProxy(bool useHttpProxy) {
	_useHttpProxy = useHttpProxy;
	notify(Event());
}

void AdvancedConfig::setProxyAuthentication(bool b) {
	_proxyAuthentication = b;
	notify(Event());
}

void AdvancedConfig::setSSL(bool ssl) {
	_ssl = ssl;
	notify(Event());
}

void AdvancedConfig::setHttpProxyUrl(const QString & httpProxyUrl) {
	_httpProxyUrl = httpProxyUrl;
	notify(Event());
}

void AdvancedConfig::setHttpProxyPort(const QString & httpProxyPort) {
	_httpProxyPort = httpProxyPort;
	notify(Event());
}

void AdvancedConfig::setHttpProxyUserID(const QString & userid) {
	_httpProxyUserID = userid;
	notify(Event());
}

void AdvancedConfig::setHttpProxyUserPassword(const QString & passwd) {
	_httpProxyUserPassword = passwd;
	notify(Event());
}

