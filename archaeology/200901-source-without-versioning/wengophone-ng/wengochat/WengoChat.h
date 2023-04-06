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

#ifndef OWWENGOCHAT_H
#define OWWENGOCHAT_H

#include <coipmanager_base/userprofile/UserProfile.h>

#include <coipmanager/connectmanager/EnumConnectionError.h>

#include <coipmanager_base/EnumConnectionState.h>

#include <QtGui/QMainWindow>

class IPostEvent;
class ManageAccount;
class TChatSession;
class TChatSessionManager;
class TCoIpManager;
class TUserProfileManager;

namespace Ui { class MainWindow; }

/**
 *
 * @author Philippe Bernery
 */
class WengoChat : public QMainWindow {
	Q_OBJECT
public:

	WengoChat();

	~WengoChat();

	void showStatusBarMessage(const std::string & message);

private Q_SLOTS:

	void tCoIpManagerInitialized();

	void manageAccount();

	void startChat();

	void newChatSessionCreatedSlot(TChatSession * tChatSession);

	void accountConnectionStateSlot(std::string accountId,
		EnumConnectionState::ConnectionState state);

	void accountConnectionErrorSlot(std::string accountId,
		EnumConnectionError::ConnectionError errorCode);

	void accountConnectionProgressSlot(std::string accountId,
		unsigned currentStep, unsigned totalSteps, std::string infoMessage);

	void userProfileSetSlot(UserProfile userProfile);

private:

	void initializeCoIpManager();

	void initializeLogTab();

	void addSession(TChatSession * session);

	Ui::MainWindow * _ui;

	ManageAccount * _manageAccount;

	TCoIpManager * _tCoIpManager;

	IPostEvent * _postEvent;

	std::string _configFilename;
};

#endif	//OWWENGOCHAT_H
