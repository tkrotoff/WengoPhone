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

#ifndef OWQTIMMENU_H
#define OWQTIMMENU_H

#include <QtGui/QMenu>

class CUserProfile;
class IMAccount;

/**
 * Menu for IMAccounts.
 *
 * This menu displays entries to set the PresenceState of an IMAccount.
 *
 * @author Philippe Bernery
 */
class QtIMMenu : public QMenu {
	Q_OBJECT
public:

	/**
	 * @param userProfile the UserProfile to interact with
	 * @param imAccount the IMAccount to change the PresenceState of
	 * @param parent the parent QWidget
	 */
	QtIMMenu(CUserProfile & cUserProfile, IMAccount & imAccount, QWidget * parent);

private Q_SLOTS:

	void onlineClicked(bool checked);

	void dndClicked(bool checked);

	void invisibleClicked(bool checked);

	void awayClicked(bool checked);

	void forwardClicked(bool checked);

	void disconnectClicked(bool checked);

	void connectClicked(bool checked);

private:

	IMAccount & _imAccount;

	CUserProfile & _cUserProfile;
};

#endif	//OWQTIMMENU_H
