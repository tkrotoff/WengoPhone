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

#ifndef OWQTTOOLBAR_H
#define OWQTTOOLBAR_H

#include <QtCore/QObject>

#include <util/NonCopyable.h>

class QtWengoPhone;
class CWengoPhone;

class QWidget;
namespace Ui { class WengoPhoneWindow; }

/**
 * Tool bar and menu actions from the main window.
 *
 * @author Tanguy Krotoff
 */
class QtToolBar : public QObject, NonCopyable {
	Q_OBJECT
public:

	QtToolBar(QtWengoPhone & qtWengoPhone, Ui::WengoPhoneWindow * qtWengoPhoneUi, QWidget * parent);

	~QtToolBar();

	QWidget * getWidget() const;

public Q_SLOTS:

	/**
	 * Expands the volume config panel from the main window.
	 */
	void expandVolumePanel();

	void showWengoAccount();

	void editMyProfile();

	void addContact();

	void showConfig();

	void showWengoForum();

	void showAbout();

	void sendSms();

	void showWengoFAQ();

	void showWengoServices();

	void showHideContactGroups();

	void showIMAccountSettings();

	void showHideOfflineContacts();

	void createConferenceCall();

	void clearHistoryOutgoingCalls();

	void clearHistoryIncomingCalls();

	void clearHistoryMissedCalls();

	void clearHistoryChatSessions();

	void clearHistorySms();

	void clearHistoryAll();

	void searchWengoContact();

	void logOff();

	void acceptCall();

	void holdResumeCall();

	void hangUpCall();

	void showChatWindow();

	void showFileTransferWindow();

private:

	/**
	 * Ugly code to remove.
	 */
	int findFirstCallTab();

	QtWengoPhone & _qtWengoPhone;

	CWengoPhone & _cWengoPhone;

	Ui::WengoPhoneWindow * _ui;
};

#endif	//OWQTTOOLBAR_H
