/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#include "QtBrowserWidget.h"

#include "QtWengoPhone.h"

#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>
#include <control/CWengoPhone.h>

#include <model/account/wengo/WengoAccount.h>
#include <model/profile/UserProfile.h>
#include <model/config/Config.h>
#include <model/config/ConfigManager.h>
#include <model/network/NetworkProxyDiscovery.h>

#include <WengoPhoneBuildId.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <qtutil/SafeConnect.h>

#include <owbrowser/QtBrowser.h>

#include <QtGui/QtGui>

static const std::string ANCHOR_CONTACTLIST = "openwengo_phonebook";
static const std::string ANCHOR_HISTORY = "openwengo_log";
static const std::string ANCHOR_CONFIGURATION = "openwengo_configuration";
static const std::string ANCHOR_DIALPAD = "openwengo_dial";
static const std::string ANCHOR_ADDCONTACT = "openwengo_addcontact";
static const std::string ANCHOR_SELFCARE = "openwengo_selfcare";
static const std::string ANCHOR_FORUM = "openwengo_forum";
static const std::string LOCAL_WEB_DIR = "webpages/windows";

QtBrowserWidget::QtBrowserWidget(QtWengoPhone & qtWengoPhone)
	: _qtWengoPhone(qtWengoPhone) {
	_qtBrowser = NULL;

#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	if (config.getIEActiveXEnable()) {
		//Embedded Browser
		_qtBrowser = new QtBrowser(NULL);
		_qtBrowser->urlClickedEvent += boost::bind(&QtBrowserWidget::urlClickedEventHandler, this, _1);
	}
#endif

	//browser re-initialization
	SAFE_CONNECT(&_qtWengoPhone, SIGNAL(userProfileDeleted()), SLOT(userProfileDeleted()));

	loadDefaultURL();
}

QtBrowserWidget::~QtBrowserWidget() {
	OWSAFE_DELETE(_qtBrowser);
}

QWidget * QtBrowserWidget::getWidget() const {
	if (!_qtBrowser) {
		return NULL;
	}

	return (QWidget*) _qtBrowser->getWidget();
}

void QtBrowserWidget::userProfileDeleted() {
	if (!_qtBrowser) {
		return;
	}

	std::string defaultURL = QCoreApplication::applicationDirPath().toStdString() + "/" + LOCAL_WEB_DIR + "/logged-off.html";
	_qtBrowser->setUrl(defaultURL);
}

void QtBrowserWidget::loadDefaultURL() {
	if (!_qtBrowser) {
		return;
	}

	std::string defaultURL = QCoreApplication::applicationDirPath().toStdString() + "/" + LOCAL_WEB_DIR + "/loading.html";
	_qtBrowser->setUrl(defaultURL);
}

void QtBrowserWidget::loadAccountURL() {
	if (!_qtBrowser) {
		return;
	}

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	CUserProfile * cUserProfile = _qtWengoPhone.getCWengoPhone().getCUserProfileHandler().getCUserProfile();
	if (cUserProfile) {
		UserProfile & userProfile = cUserProfile->getUserProfile();

		if (userProfile.getActivePhoneLine()) {
			WengoAccount wengoAccount = *userProfile.getWengoAccount();

			std::string data = "login=" + wengoAccount.getWengoLogin() +
				"&password=" + wengoAccount.getWengoPassword() +
				"&lang=" + config.getLanguage() +
				"&wl=" + std::string(WengoPhoneBuildId::getSoftphoneName()) +
				"&page=softphoneng-web";

			NetworkProxy::ProxyAuthType proxyAuthType = NetworkProxyDiscovery::getInstance().getNetworkProxy().getProxyAuthType();
			if (proxyAuthType == NetworkProxy::ProxyAuthTypeDigest) {
				//HTTPS cannot be used when the HTTP proxy is in digest:
				//ActiveX Internet Explorer crashes!
				_qtBrowser->setUrl(std::string("http://www.wengo.fr/auth/auth.php"), data, false);
			} else {
				_qtBrowser->setUrl(std::string("https://www.wengo.fr/auth/auth.php"), data, false);
			}
		}
	}
}

void QtBrowserWidget::urlClickedEventHandler(std::string url) {
	LOG_DEBUG(url);

	//Find anchor
	std::string anchor;
	int sharpPos = QString::fromStdString(url).indexOf('#');
	if (sharpPos != -1) {
		anchor = QString::fromStdString(url).right(url.length() - sharpPos - 1).toStdString();
	}

	if (anchor == ANCHOR_CONTACTLIST) {
		//showContactList();
	}
	else if (anchor == ANCHOR_HISTORY) {
		//showHistory();
	}
	else if (anchor == ANCHOR_CONFIGURATION) {
		//showConfig();
	}
	else if (anchor == ANCHOR_DIALPAD) {
		//showDialpad();
	}
	else if (anchor == ANCHOR_ADDCONTACT) {
		//addContact();
	}
	else if (anchor == ANCHOR_SELFCARE) {
		//showWengoAccount();
	}
	else if (anchor == ANCHOR_FORUM) {
		//showWengoForum();
	}
}
