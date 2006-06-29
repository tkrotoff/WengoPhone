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

#include "QtUserProfile.h"

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/profile/QtProfileDetails.h>

#include <control/profile/CUserProfile.h>

#include <model/account/SipAccount.h>
#include <model/config/Config.h>
#include <model/config/ConfigManager.h>
#include <model/contactlist/ContactProfile.h>
#include <model/presence/PresenceHandler.h>
#include <model/profile/UserProfile.h>

#include <imwrapper/IMContact.h>

#include <util/Logger.h>

#include <QtBrowser.h>
#include <WengoPhoneBuildId.h>

#include <QtGui>
#include <QMessageBox>
#include <QMetaType>

QtUserProfile::QtUserProfile(CUserProfile & cUserProfile, QtWengoPhone & qtWengoPhone)
	: QObjectThreadSafe(NULL),
	_qtWengoPhone(qtWengoPhone),
	_cUserProfile(cUserProfile) {

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtUserProfile::initThreadSafe, this));
	postEvent(event);
}

void QtUserProfile::initThreadSafe() {
	qRegisterMetaType<IMContact>("IMContact");

	connect(this, SIGNAL(loginStateChangedEventHandlerSignal(SipAccount *, int)),
		SLOT(loginStateChangedEventHandlerSlot(SipAccount *, int)), Qt::QueuedConnection);
	connect(this, SIGNAL(networkDiscoveryStateChangedEventHandlerSignal(SipAccount *, int)),
		SLOT(networkDiscoveryStateChangedEventHandlerSlot(SipAccount *, int)), Qt::QueuedConnection);
	connect(this, SIGNAL(authorizationRequestEventHandlerSignal(PresenceHandler *, IMContact, QString)),
		SLOT(authorizationRequestEventHandlerSlot(PresenceHandler *, IMContact, QString)), Qt::QueuedConnection);

	if (_cUserProfile.getUserProfile().getActivePhoneLine()) {
		setBrowserUrlToAccount();
	}
}

QtUserProfile::~QtUserProfile() {
}

void QtUserProfile::updatePresentation() {
}

void QtUserProfile::updatePresentationThreadSafe() {
}

void QtUserProfile::loginStateChangedEventHandler(SipAccount & sender,
	SipAccount::LoginState state) {
	loginStateChangedEventHandlerSignal(&sender, (int)state);
}

void QtUserProfile::networkDiscoveryStateChangedEventHandler(SipAccount & sender,
	SipAccount::NetworkDiscoveryState state) {
	networkDiscoveryStateChangedEventHandlerSignal(&sender, (int)state);
}

void QtUserProfile::authorizationRequestEventHandler(PresenceHandler & sender,
	const IMContact & imContact, const std::string & message) {
	authorizationRequestEventHandlerSignal(&sender, imContact, QString::fromStdString(message));
}

void QtUserProfile::loginStateChangedEventHandlerSlot(SipAccount * sender,
	int iState) {

	SipAccount::LoginState state = (SipAccount::LoginState) iState;

#ifdef OS_WINDOWS
	Config & config = ConfigManager::getInstance().getCurrentConfig();
#endif

	switch (state) {
	case SipAccount::LoginStateReady:
		//setBrowserUrlToAccount();
		break;

	case SipAccount::LoginStateConnected:
		setBrowserUrlToAccount();
		break;

	case SipAccount::LoginStateDisconnected:
#ifdef OS_WINDOWS
		if (config.getIEActiveX() && (sender->getType() == SipAccount::SipAccountTypeWengo)) {
			_qtWengoPhone.getQtBrowser()->setUrl(qApp->applicationDirPath().toStdString() +
				"/" + QtWengoPhone::LOCAL_WEB_DIR + "/loading.html");
		}
#endif
		break;

	case SipAccount::LoginStatePasswordError:
		break;

	case SipAccount::LoginStateNetworkError:
		break;

	default:
		LOG_FATAL("unknown state=" + String::fromNumber(state));
	}

	_qtWengoPhone.updatePresentation();
}

void QtUserProfile::networkDiscoveryStateChangedEventHandlerSlot(SipAccount * sender,
	int iState) {

	//SipAccount::NetworkDiscoveryState state = (SipAccount::NetworkDiscoveryState) iState;

}

void QtUserProfile::authorizationRequestEventHandlerSlot(PresenceHandler * sender,
	IMContact imContact, QString message) {

	QString request = QString("%1 (from %2) wants to see the presence state of %3.\n")
		.arg(QString::fromStdString(imContact.getContactId()))
		.arg(QString::fromStdString(EnumIMProtocol::toString(imContact.getProtocol())))
		.arg(QString::fromStdString(imContact.getIMAccount()->getLogin()));

	if (!message.isEmpty()) {
		request += QString("%1\n").arg(message);
	}

	int buttonClicked = QMessageBox::question(_qtWengoPhone.getWidget(),
		tr("WengoPhone - Authorization request"), request,
		tr("&Authorize"), tr("&Block"));

	if (buttonClicked == 0) {
		//TODO: give a personal message
		sender->authorizeContact(imContact, true, String::null);

		if (_cUserProfile.getCContactList().findContactThatOwns(imContact).empty()) {
			//If the contact is not in our ContactList
			ContactProfile contactProfile;
			contactProfile.addIMContact(imContact);
			QtProfileDetails qtProfileDetails(_cUserProfile, contactProfile,
				_qtWengoPhone.getWidget());
			if (qtProfileDetails.show()) {
				_cUserProfile.getCContactList().addContact(contactProfile);
			}
		}
	} else {
		// TODO: give a personal message
		// TODO: avoid direct access to model (as we are in the GUI thread)
		sender->authorizeContact(imContact, false, String::null);
	}
}

void QtUserProfile::setBrowserUrlToAccount() {
#ifdef OS_WINDOWS
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if (config.getIEActiveX() && _cUserProfile.getUserProfile().getActivePhoneLine()) {
		WengoAccount wengoAccount = *_cUserProfile.getUserProfile().getWengoAccount();
		std::string data = "?login=" + wengoAccount.getWengoLogin() +
			"&password=" + wengoAccount.getWengoPassword() +
			"&lang=" + config.getLanguage() +
			"&wl=" + std::string(WengoPhoneBuildId::SOFTPHONE_NAME) +
			"&page=softphoneng-web";
		if (_qtWengoPhone.getQtBrowser()) {
			_qtWengoPhone.getQtBrowser()->setUrl(QtWengoPhone::URL_WENGO_MINI_HOME + data);
		}
	}
#endif
}
