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

#include "QtUserProfileHandler.h"

#include <control/profile/CUserProfileHandler.h>

#include <presentation/PFactory.h>
#include <presentation/qt/QtLanguage.h>
#include <presentation/qt/QtToolBar.h>
#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/login/QtLoginDialog.h>

#include <qtutil/SafeConnect.h>
#include <util/SafeDelete.h>

#include <QtCore/QTimer>
#include <QtGui/QMessageBox>

QtUserProfileHandler::QtUserProfileHandler(CUserProfileHandler & cUserProfileHandler,
	QtWengoPhone & qtWengoPhone)
	: QObject(&qtWengoPhone),
	_cUserProfileHandler(cUserProfileHandler),
	_qtWengoPhone(qtWengoPhone) {

	//Connection for UserProfile change
	SAFE_CONNECT_TYPE(this, SIGNAL(noCurrentUserProfileSetEventHandlerSignal()),
		SLOT(noCurrentUserProfileSetEventHandlerSlot()), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(currentUserProfileWillDieEventHandlerSignal()),
		SLOT(currentUserProfileWillDieEventHandlerSlot()), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(userProfileInitializedEventHandlerSignal()),
		SLOT(userProfileInitializedEventHandlerSlot()), Qt::QueuedConnection);
	qRegisterMetaType<SipAccount>("SipAccount");
	qRegisterMetaType<WengoAccount>("WengoAccount");
	SAFE_CONNECT_TYPE(this, SIGNAL(sipAccountNotValidEventHandlerSignal(SipAccount)),
		SLOT(sipAccountNotValidEventHandlerSlot(SipAccount)), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(wengoAccountNotValidEventHandlerSignal(WengoAccount)),
		SLOT(wengoAccountNotValidEventHandlerSlot(WengoAccount)), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(profileLoadedFromBackupsEventHandlerSignal(QString)),
		SLOT(profileLoadedFromBackupsEventHandlerSlot(QString)), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(profileCannotBeLoadedEventHandlerSignal(QString)),
		SLOT(profileCannotBeLoadedEventHandlerSlot(QString)), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(sipNetworkErrorEventHandlerSignal(SipAccount)),
		SLOT(sipNetworkErrorEventHandlerSlot(SipAccount)), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(wengoNetworkErrorEventHandlerSignal(WengoAccount)),
		SLOT(wengoNetworkErrorEventHandlerSlot(WengoAccount)), Qt::QueuedConnection);
}

void QtUserProfileHandler::updatePresentation() {
}

void QtUserProfileHandler::noCurrentUserProfileSetEventHandler() {
	noCurrentUserProfileSetEventHandlerSignal();
}

void QtUserProfileHandler::currentUserProfileWillDieEventHandler() {
	currentUserProfileWillDieEventHandlerSignal();
}

void QtUserProfileHandler::userProfileInitializedEventHandler() {
	userProfileInitializedEventHandlerSignal();
}

void QtUserProfileHandler::sipAccountNotValidEventHandler(const SipAccount & sipAccount) {
	if (sipAccount.getType() == SipAccount::SipAccountTypeWengo) {
		wengoAccountNotValidEventHandlerSignal(dynamic_cast<const WengoAccount &>(sipAccount));
	} else {
		sipAccountNotValidEventHandlerSignal(sipAccount);
	}
}

void QtUserProfileHandler::profileLoadedFromBackupsEventHandler(std::string profileName) {
	profileLoadedFromBackupsEventHandlerSignal(QString::fromStdString(profileName));
}

void QtUserProfileHandler::profileCannotBeLoadedEventHandler(std::string profileName) {
	profileCannotBeLoadedEventHandlerSignal(QString::fromStdString(profileName));
}

void QtUserProfileHandler::networkErrorEventHandler(const SipAccount & sipAccount) {
	if (sipAccount.getType() == SipAccount::SipAccountTypeWengo) {
		wengoNetworkErrorEventHandlerSignal(dynamic_cast<const WengoAccount &>(sipAccount));
	} else {
		sipNetworkErrorEventHandlerSignal(sipAccount);
	}
}

void QtUserProfileHandler::noCurrentUserProfileSetEventHandlerSlot() {
	_qtWengoPhone.uninstallQtBrowserWidget();
	showLoginWindow();
}

void QtUserProfileHandler::currentUserProfileWillDieEventHandlerSlot() {
	_qtWengoPhone.currentUserProfileWillDieEventHandlerSlot();
	_cUserProfileHandler.currentUserProfileReleased();

	// For some reason, if we call showLoginWindow directly, the CPU is at 100%
	// on MacOS X. Delaying showLoginWindow with a timer solves it.
	QTimer::singleShot(0, this, SLOT(showLoginWindow()));
}

void QtUserProfileHandler::userProfileInitializedEventHandlerSlot() {
	_qtWengoPhone.userProfileInitializedEventHandlerSlot();
}

void QtUserProfileHandler::profileLoadedFromBackupsEventHandlerSlot(QString profileName) {
	QMessageBox::warning(_qtWengoPhone.getWidget(), tr("@product@"),
		tr("A problem occured while loading your profile.\n"
			"The last backuped profile has been loaded: \n"
			"you may have lost last changes made"),
		QMessageBox::Ok, QMessageBox::NoButton);
}

void QtUserProfileHandler::profileCannotBeLoadedEventHandlerSlot(QString profileName) {
	QMessageBox::warning(_qtWengoPhone.getWidget(), tr("@product@"),
		tr("Your profile could not be loaded.\n"
			"Choose another profile or create a new one."),
		QMessageBox::Ok, QMessageBox::NoButton);
}

void QtUserProfileHandler::showLoginWindow() {

	QtLoginDialog dlg(&_qtWengoPhone, _cUserProfileHandler);
	if (dlg.exec() == QDialog::Accepted) {
		_qtWengoPhone.installQtBrowserWidget();
	} else {
		_qtWengoPhone.getQtToolBar().userProfileIsInitialized();
	}
}

void QtUserProfileHandler::showLoginWindowWithWengoAccount(const WengoAccount & wengoAccount) {

	QtLoginDialog dlg(&_qtWengoPhone, _cUserProfileHandler);
	dlg.setValidAccount(wengoAccount);
	if (dlg.exec() == QDialog::Accepted) {
		_qtWengoPhone.installQtBrowserWidget();
	} else {
		_qtWengoPhone.getQtToolBar().userProfileIsInitialized();
	}
}

void QtUserProfileHandler::sipNetworkErrorEventHandlerSlot(SipAccount sipAccount) {
	_qtWengoPhone.uninstallQtBrowserWidget();

	QtLoginDialog dlg(&_qtWengoPhone, _cUserProfileHandler);
	dlg.setInvalidAccount(sipAccount);
	dlg.setErrorMessage(tr("Network error."));
	if (dlg.exec() == QDialog::Accepted) {
		_qtWengoPhone.installQtBrowserWidget();
	} else {
		_qtWengoPhone.getQtToolBar().userProfileIsInitialized();
	}
}

void QtUserProfileHandler::wengoNetworkErrorEventHandlerSlot(WengoAccount wengoAccount) {
	_qtWengoPhone.uninstallQtBrowserWidget();

	QtLoginDialog dlg(&_qtWengoPhone, _cUserProfileHandler);
	dlg.setInvalidAccount(wengoAccount);
	dlg.setErrorMessage(tr("Network error."));
	if (dlg.exec() == QDialog::Accepted) {
		_qtWengoPhone.installQtBrowserWidget();
	} else {
		_qtWengoPhone.getQtToolBar().userProfileIsInitialized();
	}
}

void QtUserProfileHandler::sipAccountNotValidEventHandlerSlot(SipAccount sipAccount) {
	_qtWengoPhone.currentUserProfileWillDieEventHandlerSlot();
	_cUserProfileHandler.currentUserProfileReleased();
	
	//_qtWengoPhone.uninstallQtBrowserWidget();
	
	QtLoginDialog dlg(&_qtWengoPhone, _cUserProfileHandler);
	dlg.setInvalidAccount(sipAccount);
	if (dlg.exec() == QDialog::Accepted) {
		_qtWengoPhone.installQtBrowserWidget();
	} else {
		_qtWengoPhone.getQtToolBar().userProfileIsInitialized();
	}
}

void QtUserProfileHandler::wengoAccountNotValidEventHandlerSlot(WengoAccount wengoAccount) {
	_qtWengoPhone.uninstallQtBrowserWidget();

	QtLoginDialog dlg(&_qtWengoPhone, _cUserProfileHandler);
	dlg.setInvalidAccount(wengoAccount);
	
	if (dlg.exec() == QDialog::Accepted) {
		_qtWengoPhone.installQtBrowserWidget();
	} else {
		_qtWengoPhone.getQtToolBar().userProfileIsInitialized();
	}
}
