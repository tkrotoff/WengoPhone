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

#include "QtUserProfileHandler.h"

#include <presentation/PFactory.h>
#include <presentation/qt/QtLanguage.h>
#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/login/QtLogin.h>

#include <control/profile/CUserProfileHandler.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

QtUserProfileHandler::QtUserProfileHandler(CUserProfileHandler & cUserProfileHandler,
	QtWengoPhone & qtWengoPhone)
	: QObjectThreadSafe(&qtWengoPhone),
	_cUserProfileHandler(cUserProfileHandler),
	_qtWengoPhone(qtWengoPhone) {

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtUserProfileHandler::initThreadSafe, this));
	postEvent(event);

	//Connection for UserProfile change
	SAFE_CONNECT_TYPE(this, SIGNAL(noCurrentUserProfileSetEventHandlerSignal()),
		SLOT(noCurrentUserProfileSetEventHandlerSlot()), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(currentUserProfileWillDieEventHandlerSignal()),
		SLOT(currentUserProfileWillDieEventHandlerSlot()), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(userProfileInitializedEventHandlerSignal()),
		SLOT(userProfileInitializedEventHandlerSlot()), Qt::QueuedConnection);
	qRegisterMetaType<WengoAccount>("WengoAccount");
	SAFE_CONNECT_TYPE(this, SIGNAL(wengoAccountNotValidEventHandlerSignal(WengoAccount)),
		SLOT(wengoAccountNotValidEventHandlerSlot(WengoAccount)), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(profileLoadedFromBackupsEventHandlerSignal(QString)),
		SLOT(profileLoadedFromBackupsEventHandlerSlot(QString)), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(profileCannotBeLoadedEventHandlerSignal(QString)),
		SLOT(profileCannotBeLoadedEventHandlerSlot(QString)), Qt::QueuedConnection);
}

void QtUserProfileHandler::initThreadSafe() {
	//Login Window
	_qtLogin = new QtLogin(_qtWengoPhone.getWidget(), _cUserProfileHandler);
}

void QtUserProfileHandler::updatePresentation() {
}

void QtUserProfileHandler::updatePresentationThreadSafe() {
}

QtUserProfileHandler::~QtUserProfileHandler() {
	//FIXME: these objects should be deleted in GUI thread but this
	//destructor is called in model thread.
	/*
	if (_qtLogin) {
		delete _qtLogin;
		_qtLogin = NULL;
	}
	*/
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

void QtUserProfileHandler::wengoAccountNotValidEventHandler(const WengoAccount & wengoAccount) {
	wengoAccountNotValidEventHandlerSignal(wengoAccount);
}

void QtUserProfileHandler::profileLoadedFromBackupsEventHandler(std::string profileName) {
	profileLoadedFromBackupsEventHandlerSignal(QString::fromStdString(profileName));
}

void QtUserProfileHandler::profileCannotBeLoadedEventHandler(std::string profileName) {
	profileCannotBeLoadedEventHandlerSignal(QString::fromStdString(profileName));
}

void QtUserProfileHandler::noCurrentUserProfileSetEventHandlerSlot() {
	showLoginWindow();
}

void QtUserProfileHandler::currentUserProfileWillDieEventHandlerSlot() {
	_qtWengoPhone.currentUserProfileWillDieEventHandlerSlot();
	_cUserProfileHandler.currentUserProfileReleased();
}

void QtUserProfileHandler::userProfileInitializedEventHandlerSlot() {
	_qtWengoPhone.userProfileInitializedEventHandlerSlot();
}

void QtUserProfileHandler::wengoAccountNotValidEventHandlerSlot(WengoAccount wengoAccount) {
	_qtLogin->showWithInvalidWengoAccount(wengoAccount);
}

void QtUserProfileHandler::profileLoadedFromBackupsEventHandlerSlot(QString profileName) {
	QMessageBox::warning(_qtWengoPhone.getWidget(), "WengoPhone",
		tr("A problem occured while loading your profile.\n"
			"The last backuped profile has been loaded: \n"
			"you may have lost last changes made"),
		QMessageBox::Ok, QMessageBox::NoButton);
}

void QtUserProfileHandler::profileCannotBeLoadedEventHandlerSlot(QString profileName) {
	QMessageBox::warning(_qtWengoPhone.getWidget(), "WengoPhone",
		tr("Your profile could not be loaded.\n"
			"Choose another profile or create a new one."),
		QMessageBox::Ok, QMessageBox::NoButton);
}

void QtUserProfileHandler::showLoginWindow() {
	_qtLogin->show();
}

void QtUserProfileHandler::showLoginWindowWithWengoAccount(const WengoAccount & wengoAccount) {
	_qtLogin->showWithWengoAccount(wengoAccount);
}
