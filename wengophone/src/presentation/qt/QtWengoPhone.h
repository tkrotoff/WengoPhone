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
#include <imwrapper/EnumPresenceState.h>

#include <qtutil/QObjectThreadSafe.h>

#include <QMainWindow>
#include <QVariant>
#include <string>

class PresenceHandler;
class IMContact;
class UserProfile;
class CWengoPhone;
class PPhoneLine;
class QtPhoneCall;
class QtProfileBar;
class QtContactList;
class QtContactCallListWidget;
class QtSms;
class QtSubscribe;
class QtBrowser;
class QtHistoryWidget;
class QtLogin;
class QtToaster;
class QtStatusBar;
class QtCallBar;
class QtProfileBar;
class QtWengoStyleLabel;
class QtWsDirectory;
class TrayIcon;

class QPushButton;
class QComboBox;
class QTabWidget;
class QLayout;
class QLabel;
class QFrame;
class QStackedWidget;
class QWidget;
class QMenu;
namespace Ui { class WengoPhoneWindow; }

/**
 * Qt Presentation component for WengoPhone.
 *
 * @author Tanguy Krotoff
 */
class QtWengoPhone : public QObjectThreadSafe, public PWengoPhone {
	Q_OBJECT
public:

	QtWengoPhone(CWengoPhone & cWengoPhone);

	void modelInitializedEvent();

	void addPhoneCall(QtPhoneCall * qtPhoneCall);

	void addToConference(QtPhoneCall * qtPhoneCall);

	void setContactList(QtContactList * qtContactList);

	void setHistory(QtHistoryWidget * qtHistoryWidget);

	void setPhoneCall(QtContactCallListWidget * qtContactCallListWidget);

	void setSms(QtSms * qtSms);

	void setWsDirectory(QtWsDirectory * qtWsDirectory);

	QtSms * getSms() const;

	QtWsDirectory * getQtWsDirectory() const;

	void setSubscribe(QtSubscribe * qtSubscribe);

	QtSubscribe * getSubscribe() const;

	QtLogin * getLogin() const;

	void updatePresentation();

	void dialpad(const std::string & tone, const std::string & soundFile);

	void authorizationRequestEventHandler(PresenceHandler & sender, const IMContact & imContact,
		const std::string & message);

	QWidget * getWidget() const {
		return _wengoPhoneWindow;
	}

	QtStatusBar & getStatusBar() const;

	QtWengoStyleLabel * getCallButton() const {
		return _callButton;
	}

/*	QtWengoStyleLabel * getHangUpButton() const {
		return _hangUpButton;
	}
*/
	QComboBox * getPhoneComboBox() const {
		return _phoneComboBox;
	}

Q_SIGNALS:

	void modelInitializedEventSignal();

private Q_SLOTS:

	void callButtonClicked();

	void hangupButtonClicked();

	void enableCallButton();

	void showWengoAccount();

	void openWengoAccount();

	void editMyProfile();

	void exitApplication();

	void addContact();

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

	void showSearchContactWindows();

	void showVoiceMail();

	void showHideOffLineContacts();

	void setTrayMenu();

	void sortContactsAlphabetically();

	void toasterClosed(QtToaster * toaster);

	void expandConfigPanel();

	void eraseHistoryOutgoingCalls();

	void eraseHistoryIncomingCalls();

	void eraseHistoryMissedCalls();

	void eraseHistoryChatSessions();

	void eraseHistorySms();

	void eraseHistory();

	void phoneComboBoxClicked();

	void setSystrayIcon(QVariant status);

	void sysTrayDoubleClicked(const QPoint& p);

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

	void wrongProxyAuthenticationEventHandler(SipAccount & sender,
		const std::string & proxyAddress, unsigned proxyPort,
		const std::string & proxyLogin, const std::string & proxyPassword);

	void proxyNeedsAuthenticationEventHandler(SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort);

	void proxyNeedsAuthenticationEventHandlerThreadSafe(SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort);

	void authorizationRequestEventHandlerThreadSafe(PresenceHandler & sender, const IMContact & imContact,
		const std::string & message);

	void showLoginWindow();

	void urlClickedEventHandler(std::string url);

	QMenu * createStatusMenu();

	/** Direct link to the control. */
	CWengoPhone & _cWengoPhone;

	Ui::WengoPhoneWindow * _ui;

	QMainWindow * _wengoPhoneWindow;

	QtWengoStyleLabel * _callButton;

	QtWengoStyleLabel * _hangUpButton;

	QComboBox * _phoneComboBox;

	TrayIcon * _trayIcon;

	QMenu * _trayMenu;

	QtSms * _qtSms;

	QtWsDirectory * _qtWsDirectory;

	QtSubscribe * _qtSubscribe;

	QtLogin * _qtLogin;

	QtBrowser * _browser;

	QtContactList * _contactList;

	QtHistoryWidget * _qtHistoryWidget;

	QtContactCallListWidget * _contactCallListWidget;

    QtProfileBar * _qtProfileBar;

	QtCallBar * _qtCallBar;

	QWidget * _configPanelWidget;

	QtStatusBar * _statusBar;

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
