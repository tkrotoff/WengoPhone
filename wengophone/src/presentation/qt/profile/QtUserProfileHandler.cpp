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

#include <presentation/qt/QtLanguage.h>
#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/login/QtLogin.h>

#include <control/profile/CUserProfileHandler.h>

QtUserProfileHandler::QtUserProfileHandler(CUserProfileHandler & cUserProfileHandler, 
	QtWengoPhone & qtWengoPhone) 
: QObjectThreadSafe(&qtWengoPhone),
 _cUserProfileHandler(cUserProfileHandler),
_qtWengoPhone(qtWengoPhone) {

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtUserProfileHandler::initThreadSafe, this));
	postEvent(event);
}

void QtUserProfileHandler::initThreadSafe() {
	// Connection for UserProfile change
	connect(this, SIGNAL(noCurrentUserProfileSetEventHandlerSignal()),
		SLOT(noCurrentUserProfileSetEventHandlerSlot()), Qt::QueuedConnection);
	connect(this, SIGNAL(currentUserProfileWillDieEventHandlerSignal()),
		SLOT(currentUserProfileWillDieEventHandlerSlot()), Qt::QueuedConnection);
	connect(this, SIGNAL(userProfileInitializedEventHandlerSignal()),
		SLOT(userProfileInitializedEventHandlerSlot()), Qt::QueuedConnection);
	qRegisterMetaType<WengoAccount>("WengoAccount");
	connect(this, SIGNAL(wengoAccountNotValidEventHandlerSignal(WengoAccount)),
		SLOT(wengoAccountNotValidEventHandlerSlot(WengoAccount)), Qt::QueuedConnection);
	////

	// Login Window
	_qtLogin = new QtLogin(_qtWengoPhone.getWidget(), _cUserProfileHandler);
	connect(_qtWengoPhone.getQtLanguage(), SIGNAL(translationChangedSignal()),
		_qtLogin, SLOT(slotUpdatedTranslation()));
	////
}

void QtUserProfileHandler::updatePresentation() {

}

void QtUserProfileHandler::updatePresentationThreadSafe() {

}

QtUserProfileHandler::~QtUserProfileHandler() {
	// FIXME: these objects should be deleted in GUI thread but this
	// destructor is called in model thread.
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

void QtUserProfileHandler::showLoginWindow() {
	_qtLogin->show();
}

void QtUserProfileHandler::showLoginWindowWithWengoAccount(const WengoAccount & wengoAccount) {
	_qtLogin->showWithWengoAccount(wengoAccount);
}
