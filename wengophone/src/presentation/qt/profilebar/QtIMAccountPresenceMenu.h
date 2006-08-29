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

#ifndef OWQTIMACCOUNTPRESENCEMENU_H
#define OWQTIMACCOUNTPRESENCEMENU_H

#include <presentation/qt/QtUserProfilePresenceMenu.h>

class CUserProfile;
class IMAccount;

/**
 * Menu for IMAccounts.
 *
 * This menu displays entries to set the PresenceState of an IMAccount.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class QtIMAccountPresenceMenu : public QtUserProfilePresenceMenu {
	Q_OBJECT
public:

	/**
	 * @param userProfile the UserProfile to interact with
	 * @param imAccount the IMAccount to change the PresenceState of
	 * @param parent the parent QWidget
	 */
	QtIMAccountPresenceMenu(CUserProfile & cUserProfile, IMAccount & imAccount, QWidget * parent);

	~QtIMAccountPresenceMenu();

private Q_SLOTS:

	void onlineClicked();

	void doNotDisturbClicked();

	void invisibleClicked();

	void awayClicked();

	void disconnectClicked();

	void connectClicked();

private:

	IMAccount & _imAccount;

	CUserProfile & _cUserProfile;
};

#endif	//OWQTIMACCOUNTPRESENCEMENU_H
