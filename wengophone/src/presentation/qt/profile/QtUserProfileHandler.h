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

#ifndef OWQTUSERPROFILEHANDLER_H
#define OWQTUSERPROFILEHANDLER_H

#include <presentation/PUserProfileHandler.h>

#include <model/account/wengo/WengoAccount.h>

#include <qtutil/QObjectThreadSafe.h>

class CUserProfileHandler;
class QtLogin;
class QtWengoPhone;

/**
 * Qt implementation of UserProfileHandler Presentation layer.
 *
 * @author Philippe Bernery
 */
class QtUserProfileHandler : public QObjectThreadSafe, public PUserProfileHandler {

	Q_OBJECT

public:

	QtUserProfileHandler(CUserProfileHandler & cUserProfileHandler, QtWengoPhone & qtWengoPhone);

	virtual ~QtUserProfileHandler();

	/**
	 * @see CUserProfileHandler::noCurrentUserProfileSetEventHandler
	 */
	void noCurrentUserProfileSetEventHandler();

	/**
	 * @see CUserProfileHandler::currentUserProfileWillDieEventHandler
	 */
	void currentUserProfileWillDieEventHandler();

	/**
	 * @see CUserProfileHandler::userProfileInitializedEventHandler
	 */
	void userProfileInitializedEventHandler();

	/**
	 * @see CUserProfileHandler::wengoAccountNotValidEventHandler
	 */
	void wengoAccountNotValidEventHandler(const WengoAccount & wengoAccount);

	/**
	 * @see CUserProfileHandler::defaultUserProfileExistsEventHandler
	 */
	void defaultUserProfileExistsEventHandler(const std::string & createdProfileName);

Q_SIGNALS:

	/**
	 * @see noCurrentUserProfileSetEventHandler
	 */
	void noCurrentUserProfileSetEventHandlerSignal();

	/**
	 * @see currentUserProfileWillDieEventHandler
	 */
	void currentUserProfileWillDieEventHandlerSignal();

	/**
	 * @see userProfileInitializedEventHandler
	 */
	void userProfileInitializedEventHandlerSignal();

	/**
	 * @see wengoAccountNotValidEventHandler
	 */
	void wengoAccountNotValidEventHandlerSignal(WengoAccount wengoAccount);

	/**
	 * @see defaultUserProfileExistsEventHandler
	 */
	void defaultUserProfileExistsEventHandlerSignal(QString createdProfileName);

public Q_SLOTS:

	/**
	 * Show the Profile chooser window.
	 */
	void showLoginWindow();

	/**
	 * Show the Profile chooser window with the given WengoAccount.
	 *
	 * Used by QtSubscribe2.
	 */
	void showLoginWindowWithWengoAccount(const WengoAccount & wengoAccount);

private Q_SLOTS:

	/**
	 * @see noCurrentUserProfileSetEventHandler
	 */
	void noCurrentUserProfileSetEventHandlerSlot();

	/**
	 * @see currentUserProfileWillDieEventHandler
	 */
	void currentUserProfileWillDieEventHandlerSlot();

	/**
	 * @see userProfileInitializedEventHandler
	 */
	void userProfileInitializedEventHandlerSlot();

	/**
	 * @see wengoAccountNotValidEventHandler
	 */
	void wengoAccountNotValidEventHandlerSlot(WengoAccount wengoAccount);

	/**
	 * @see defaultUserProfileExistsEventHandler
	 */
	void defaultUserProfileExistsEventHandlerSlot(QString createdProfileName);

private:

	void initThreadSafe();

	void updatePresentation();

	void updatePresentationThreadSafe();

	/** Link to control layer. */
	CUserProfileHandler & _cUserProfileHandler;

	/** Link to QtWengoPhone to create needed objects. */
	QtWengoPhone & _qtWengoPhone;

	/** Login Windows. */
	QtLogin * _qtLogin;

};

#endif //OWQTUSERPROFILEHANDLER_H
