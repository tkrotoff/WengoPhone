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

#ifndef OWLOGINWIDGET_H
#define OWLOGINWIDGET_H

#include <coipmanager/connectmanager/EnumConnectionError.h>

#include <coipmanager_base/EnumConnectionState.h>
#include <coipmanager_base/userprofile/UserProfile.h>

#include <QtGui/QMainWindow>

#include <stack>
#include <string>

class TCoIpManager;

namespace Ui { class LoginWidget; }

/**
 * Test program for Call service of CoIpManager.
 *
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class LoginWidget : public QMainWindow {
	Q_OBJECT
public:

	LoginWidget();

	~LoginWidget();

private Q_SLOTS:

	void tCoIpManagerInitialized();

	void loginButtonClicked();

	void realmLineEditingFinished();

	void setAccountType(int accountType);

	void userProfileSetSlot(UserProfile userProfile);

private:

	std::string addAccount();

	std::string addSipAccount();

	std::stack<QWidget *> _childWidgetStack;

	Ui::LoginWidget *_ui;

	TCoIpManager *_tCoIpManager;

};

#endif //OWLOGINWIDGET_H
