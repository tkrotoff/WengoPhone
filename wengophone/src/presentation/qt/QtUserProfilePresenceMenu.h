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

#ifndef OWQTUSERPROFILEPRESENCEMENU_H
#define OWQTUSERPROFILEPRESENCEMENU_H

#include <QtGui/QMenu>

#include <imwrapper/EnumPresenceState.h>

/**
 * IM presence menu (online, away...) for the user.
 *
 * This is code factorization.
 *
 * @author Tanguy Krotoff
 */
class QtUserProfilePresenceMenu : public QMenu {
	Q_OBJECT
public:

	/**
	 * Creates the IM presence menu.
	 *
	 * @param myPresenceState if user is online, away...
	 * @param connected if user is connected or disconnected
	 * @param title menu title
	 * @param parent parent widget
	 */
	QtUserProfilePresenceMenu(EnumPresenceState::PresenceState myPresenceState, bool connected,
			const QString & title, QWidget * parent);

Q_SIGNALS:

	void onlineClicked();

	void doNotDisturbClicked();

	void invisibleClicked();

	void awayClicked();

	void disconnectClicked();

private Q_SLOTS:

	void onlineClicked(bool checked);

	void doNotDisturbClicked(bool checked);

	void invisibleClicked(bool checked);

	void awayClicked(bool checked);

	void disconnectClicked(bool checked);
};

#endif	//OWQTUSERPROFILEPRESENCEMENU_H
