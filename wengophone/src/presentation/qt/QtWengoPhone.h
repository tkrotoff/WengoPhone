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

#ifndef QTWENGOPHONE_H
#define QTWENGOPHONE_H

#include <presentation/PWengoPhone.h>
#include <model/account/SipAccount.h>

#include <QObjectThreadSafe.h>

#include <QMainWindow>

class SipAccount;
class CWengoPhone;
class PPhoneLine;
class QtPhoneCall;
class QtContactList;
class QtSms;

class QPushButton;
class QComboBox;
class QTabWidget;
class QLayout;

/**
 * Qt Presentation component for WengoPhone.
 *
 * @author Tanguy Krotoff
 */
class QtWengoPhone : public QObjectThreadSafe, public PWengoPhone {
	Q_OBJECT
public:

	QtWengoPhone(CWengoPhone & cWengoPhone);

	void addPhoneLine(PPhoneLine * pPhoneLine);

	void addPhoneCall(QtPhoneCall * qtPhoneCall);

	void setContactList(QtContactList * qtContactList);

	void setSms(QtSms * qtSms);

	void updatePresentation();

	void dialpad(const std::string & tone, const std::string & soundFile);

	QWidget * getWidget() const {
		return _wengoPhoneWindow;
	}

	QPushButton * getCallButton() const {
		return _callButton;
	}

	QComboBox * getPhoneComboBox() const {
		return _phoneComboBox;
	}

private Q_SLOTS:

	void callButtonClicked();

	void enableCallButton();

	void showWengoAccount();

	void openWengoAccount();

	void exitApplication();

	void addContact();
	
	void actionSetLogin();

	void showConfig();

	void showForum();

	void showHelp();

	void showAbout();

	void showHome();

	void showContactList();

	void showCallHistory();

	void sendSms();

private:

	void initThreadSafe();

	/**
	 * Initializes pickup and hangup buttons inside the main window.
	 *
	 * This is called by QtPhoneCall to re-initialize the buttons
	 * since QtPhoneCall modifies the behaviour of this buttons.
	 */
	void initButtons();

	void addPhoneLineThreadSafe(PPhoneLine * pPhoneLine);

	void updatePresentationThreadSafe();

	void noAccountAvailableEventHandler(WengoPhone & sender);

	void noAccountAvailableEventHandlerThreadSafe(WengoPhone & sender);

	void loginStateChangedEventHandler(SipAccount & sipAccount, SipAccount::LoginState state);

	void loginStateChangedEventHandlerThreadSafe(SipAccount & sender, SipAccount::LoginState state);

	void wrongProxyAuthenticationEventHandler(SipAccount & sender,
		const std::string & proxyAddress, unsigned proxyPort,
		const std::string & proxyLogin, const std::string & proxyPassword);

	void proxyNeedsAuthenticationEventHandler(SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort);

	void proxyNeedsAuthenticationEventHandlerThreadSafe(SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort);

	void showLoginWindow();

	static QLayout * createLayout(QWidget * parent);


	/** Direct link to the control. */
	CWengoPhone & _cWengoPhone;

	QMainWindow * _wengoPhoneWindow;

	QPushButton * _callButton;

	QComboBox * _phoneComboBox;

	QTabWidget * _tabWidget;

	QtSms * _qtSms;
};

#endif	//QTWENGOPHONE_H
