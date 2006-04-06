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

#include <qtutil/QObjectThreadSafe.h>

#include <QMainWindow>
#include <QMenu>
#include <trayicon.h>

class SipAccount;
class UserProfile;
class CWengoPhone;
class PPhoneLine;
class QtPhoneCall;
class QtContactList;
class QtContactCallListWidget;
class QtSms;
class QtBrowser;
class QtHistoryWidget;

class QPushButton;
class QComboBox;
class QTabWidget;
class QLayout;
class QLabel;
class QFrame;
class QtToaster;

/**
 * Qt Presentation component for WengoPhone.
 *
 * @author Tanguy Krotoff
 */
class QtWengoPhone : public QObjectThreadSafe, public PWengoPhone {
	Q_OBJECT
public:

	QtWengoPhone(CWengoPhone & cWengoPhone);

	void addPhoneCall(QtPhoneCall * qtPhoneCall);

	void setContactList(QtContactList * qtContactList);

	void setHistory(QtHistoryWidget * qtHistoryWidget);

	void setPhoneCall(QtContactCallListWidget * qtContactCallListWidget);

	void setSms(QtSms * qtSms);

	QtSms * getSms() const;

	void updatePresentation();

	void dialpad(const std::string & tone, const std::string & soundFile);

	QWidget * getWidget() const {
		return _wengoPhoneWindow;
	}

	QLabel * getInternetConnectionStateLabel() const {
		return _internetConnectionStateLabel;
	}

	QLabel * getPhoneLineStateLabel() const {
		return _phoneLineStateLabel;
	}

	QPushButton * getCallButton() const {
		return _callButton;
	}

	QPushButton * getHangUpButton() const {
		return _hangUpButton;
	}

	QComboBox * getPhoneComboBox() const {
		return _phoneComboBox;
	}

private Q_SLOTS:

	void callButtonClicked();

	void enableCallButton();

	void showWengoAccount();

	void openWengoAccount();

	void editMyProfile();

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

	void showAdvancedConfig();

	void showCreateConferenceCall();

	void showAccountSettings();

	void showFaq();

	void showBuyOut();

	void showCallOut();

	void showSms();

	void showVoiceMail();

	void showHideOffLineContacts();

	void setTrayMenu();

	void sortContacts();

	void toasterClosed(QtToaster * toaster);

private:

	void initThreadSafe();

	/**
	 * Initializes pickup and hangup buttons inside the main window.
	 *
	 * This is called by QtPhoneCall to re-initialize the buttons
	 * since QtPhoneCall modifies the behaviour of this buttons.
	 */
	void initButtons();

	void updatePresentationThreadSafe();

	void noAccountAvailableEventHandler(UserProfile & sender);

	void noAccountAvailableEventHandlerThreadSafe(UserProfile & sender);

	void loginStateChangedEventHandler(SipAccount & sipAccount, SipAccount::LoginState state);

	void loginStateChangedEventHandlerThreadSafe(SipAccount & sender, SipAccount::LoginState state);

	void networkDiscoveryStateChangedEventHandler(SipAccount & sender, SipAccount::NetworkDiscoveryState state);

	void networkDiscoveryStateChangedEventHandlerThreadSafe(SipAccount & sender, SipAccount::NetworkDiscoveryState state);

	void wrongProxyAuthenticationEventHandler(SipAccount & sender,
		const std::string & proxyAddress, unsigned proxyPort,
		const std::string & proxyLogin, const std::string & proxyPassword);

	void proxyNeedsAuthenticationEventHandler(SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort);

	void proxyNeedsAuthenticationEventHandlerThreadSafe(SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort);

	void showLoginWindow();

	void urlClickedEventHandler(std::string url);

	static QLayout * createLayout(QWidget * parent);

	/** Direct link to the control. */
	CWengoPhone & _cWengoPhone;

	QMainWindow * _wengoPhoneWindow;

	QPushButton * _callButton;

	QPushButton * _hangUpButton;

	QComboBox * _phoneComboBox;

	QTabWidget * _tabWidget;

	TrayIcon * _trayIcon;

	QMenu * _trayMenu;

	QtSms * _qtSms;

	QtBrowser * _browser;

	QtContactList *_contactList;

	QtContactCallListWidget * _contactCallListWidget;

	QLabel * _phoneLineStateLabel;

	QLabel * _internetConnectionStateLabel;

	QFrame * _iconBar;

	QPushButton * _addContactButton;

	static const std::string ANCHOR_CONTACTLIST;

	static const std::string ANCHOR_HISTORY;

	static const std::string ANCHOR_CONFIGURATION;

	static const std::string ANCHOR_DIALPAD;

	static const std::string ANCHOR_ADDCONTACT;

	static const std::string ANCHOR_SELFCARE;

	static const std::string ANCHOR_FORUM;

	static const std::string URL_WENGO_MINI_HOME;

	static const std::string LOCAL_WEB_DIR;
};

#endif	//QTWENGOPHONE_H
