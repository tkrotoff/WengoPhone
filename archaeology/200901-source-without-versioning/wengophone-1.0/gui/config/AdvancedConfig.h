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

#ifndef ADVANCEDCONFIG_H
#define ADVANCEDCONFIG_H

#include <observer/Subject.h>
#include <singleton/Singleton.h>

#include <qstring.h>

/**
 * Singleton class to record advanced configuration settings
 * such as sip port or NAT configuration.
 *
 * @author Philippe Kajmar
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class AdvancedConfig : public Subject, public Singleton<AdvancedConfig> {
	friend class Singleton<AdvancedConfig>;
public:

	/// number of NAT configurations
	static const unsigned short NB_NAT_CONFIG;

	/// automatic
	static const char * AUTOMATIC;

	/// nonde
	static const char * NONE;

	/// full cone
	static const char * FULL_CONE;

	/// restriced cone
	static const char * RESTRICT_CONE;

	/// port restricted cone
	static const char * PORT_RESTRICT_CONE;

	/// symetric
	static const char * SYMMETRIC;

	/// advanced config tag
	static const QString ADVANCEDCONFIG_TAG;

	/// sip port tag
	static const QString SIPPORT_TAG;

	/// nat tag
	static const QString NAT_TAG;

	static const QString ECHO_CANCELLER_TAG;

	static const QString USE_HTTP_PROXY_TAG;

	static const QString USE_PROXY_AUTHENTICATION;
	
	static const QString USE_SSL;
	
	static const QString HTTP_PROXY_URL_TAG;

	static const QString HTTP_PROXY_PORT_TAG;

	static const QString HTTP_PROXY_USERID_TAG;
	
	static const QString HTTP_PROXY_USERPASSWORD_TAG;
	
	static const QString MAINWINDOWX_TAG;
	static const QString MAINWINDOWY_TAG;
	static const QString MAINWINDOWWIDTH_TAG;
	static const QString MAINWINDOWHEIGHT_TAG;

	static const QString AUTOSTART_TAG;

	void setAutoStart(bool b) {
		_autoStart = b;
	}
	
	bool getAutoStart() {
		return _autoStart;
	}

	/// sipPort accessor
	void setSipPort(const QString & sipPort);

	/// sipPort accessor
	QString getSipPort() const;

	/// nat accessor
	void setNat(const QString & nat);

	/// nat accessor
	QString getNat() const;

	void setEchoCanceller(bool echoCanceller);

	bool hasEchoCanceller() const {
		return _echoCanceller;
	}

	bool isHttpProxyUsed() const {
		return _useHttpProxy;
	}

	void setUseHttpProxy(bool useHttpProxy);

	bool isProxyAuthentication() const {
		return _proxyAuthentication;
	}

	void setProxyAuthentication(bool useHttpProxy);
	
	bool useSSL() const {
		return _ssl;
	}

	void setSSL(bool ssl);
	
	QString getHttpProxyUrl() const {
		return _httpProxyUrl;
	}

	void setHttpProxyUrl(const QString & httpProxyUrl);

	QString getHttpProxyPort() const {
		return _httpProxyPort;
	}

	void setHttpProxyPort(const QString & httpProxyPort);

	QString getHttpProxyUserID() const {
		return _httpProxyUserID;
	}

	void setHttpProxyUserID(const QString & userid);

	QString getHttpProxyUserPassword() const {
		return _httpProxyUserPassword;
	}

	void setHttpProxyUserPassword(const QString & passwd);
	
	int getWindowX() {
		return _x;
	}
	
	void setWindowX(int x) {
		_x = x;
	}
	
	int getWindowY() {
		return _y;
	}
	
	void setWindowY(int y) {
		_y = y;
	}
	
	int getWindowWidth() {
		return _width;
	}
	
	void setWindowWidth(int w) {
		_width = w;
	}
	
	int getWindowHeight() {
		return _height;
	}
	
	void setWindowHeight(int h) {
		_height = h;
	}
	
	/// set default settings
	void defaultSettings();
	
	/// test if settings are valid
	bool isValid() const;
	
private:
	
	AdvancedConfig();
	
	bool _autoStart;
	
	/// sip port
	QString _sipPort;
	
	/// nat type
	QString _nat;
	
	bool _echoCanceller;
	
	bool _useHttpProxy;
	
	bool _proxyAuthentication;
	
	bool _ssl;
	
	QString _httpProxyUrl;
	
	QString _httpProxyPort;
	
	QString _httpProxyUserID;
	
	QString _httpProxyUserPassword;
	
	//main window position
	int _x;
	int _y;
	
	//main window size
	int _width;
	int _height;
};

#endif	//ADVANCEDCONFIG_H
