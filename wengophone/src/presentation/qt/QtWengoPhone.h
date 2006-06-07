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
#include <model/account/wengo/WengoAccount.h>

#include <imwrapper/EnumPresenceState.h>

#include <qtutil/QObjectThreadSafe.h>

#include <QMainWindow>
#include <QVariant>
#include <QAction>
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
class QtIdle;
class QtLanguage;
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

	QtWengoPhone(CWengoPhone & cWengoPhone, bool background = false);

	virtual ~QtWengoPhone();

	void addPhoneCall(QtPhoneCall * qtPhoneCall);

	void addToConference(QtPhoneCall * qtPhoneCall);

	void setContactList(QtContactList * qtContactList);

	QtContactList * getContactList() { return _contactList;}

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

	void connectionStatusEventHandler(int totalSteps, int currentStep, const std::string & infoMsg);

	void noCurrentUserProfileSetEventHandler();

	void currentUserProfileWillDieEventHandler();

	void userProfileInitializedEventHandler();

	void wengoAccountNotValidEventHandler(const WengoAccount & wengoAccount);

	QWidget * getWidget() const {
		return _wengoPhoneWindow;
	}

	QtStatusBar & getStatusBar() const;

	QtWengoStyleLabel * getCallButton() const {
		return _callButton;
	}

	QComboBox * getPhoneComboBox() const {
		return _phoneComboBox;
	}

	void setChatWindow(QWidget * chatWindow);

	void showHistory();

Q_SIGNALS:

	void connectionStatusEventHandlerSignal(int totalSteps, int currentStep, QString infoMsg);

	void noCurrentUserProfileSetEventHandlerSignal();

	void currentUserProfileWillDieEventHandlerSignal();

	void userProfileInitializedEventHandlerSignal();

	void wengoAccountNotValidEventHandlerSignal(WengoAccount wengoAccount);

	void signalTimeoutEventReached();

private Q_SLOTS:

	void connectionStatusEventHandlerSlot(int totalSteps, int currentStep, QString infoMsg);

	void noCurrentUserProfileSetEventHandlerSlot();

	void currentUserProfileWillDieEventHandlerSlot();

	void userProfileInitializedEventHandlerSlot();

	void wengoAccountNotValidEventHandlerSlot(WengoAccount wengoAccount);

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

	void showLoginWindow();

	void showContactList();

	void showChatWindow();

	void sendSms();

	void showCreateConferenceCall();

	void showAccountSettings();

	void showFaq();

	void showBuyOut();

	void showCallOut();

	void showSms();

	void showSearchContactWindows();

	void showVoiceMail();

	void showHideOffLineContacts();

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

	void addToConference(QString phoneNumber, PhoneCall * targetCall);

	void addToConference(PhoneCall * sourceCall, PhoneCall * targetCall);

	void logoff();

	void tabSelectionChanged(int index);

	void acceptCall();

	void resumeCall();

	void hangupCall();

	void hideMainWindow();

	void slotSystrayMenuCallWengo(QAction * action);

	void slotSystrayMenuCallMobile(QAction * action);

	void slotSystrayMenuCallLandLine(QAction * action);

	void slotSystrayMenuStartChat(QAction * action);

	void slotSystrayMenuSendSms(QAction * action);

	void slotSystrayMenuCallBlank(bool checked);

	void setTrayMenu();

	void slotTimeoutEventReachedThreadSafe();

	void slotTranslationChanged();

    void showHideGroups();

private:

	void initThreadSafe();

	/**
	 * Initializes pickup and hangup buttons inside the main window.
	 *
	 * This is called by QtPhoneCall to re-initialize the buttons
	 * since QtPhoneCall modifies the behaviour of this buttons.
	 */
	void initButtons();

	void updateCallMenu();

	void updatePresentationThreadSafe();

	void loginStateChangedEventHandler(SipAccount & sipAccount, SipAccount::LoginState state);

	void loginStateChangedEventHandlerThreadSafe(SipAccount & sender, SipAccount::LoginState state);

	void wrongProxyAuthenticationEventHandler(SipAccount & sender,
		const std::string & proxyAddress, unsigned proxyPort,
		const std::string & proxyLogin, const std::string & proxyPassword);

	void proxyNeedsAuthenticationEventHandler(SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort);

	void proxyNeedsAuthenticationEventHandlerThreadSafe(SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort);

	void authorizationRequestEventHandlerThreadSafe(PresenceHandler & sender, const IMContact & imContact,
		const std::string & message);

	void urlClickedEventHandler(std::string url);

	int findFirstCallTab();

	QMenu * createStatusMenu();

	void timeoutEventHandler();

	QMenu * createCallLandLineTrayMenu();
	QMenu * createCallMobileTrayMenu();
	QMenu * createCallWengoTrayMenu();
	QMenu * createCallMenu();

	/** Direct link to the control. */
	CWengoPhone & _cWengoPhone;

	Ui::WengoPhoneWindow * _ui;

	QMainWindow * _wengoPhoneWindow;

	QtWengoStyleLabel * _callButton;

	QtWengoStyleLabel * _hangUpButton;

	QComboBox * _phoneComboBox;

	TrayIcon * _trayIcon;

	QMenu * _trayMenu;

	QMenu * _callMenu;
	QMenu * _callLandLineMenu;
	QMenu * _callMobileMenu;
	QMenu * _callWengoMenu;

	QMenu * _startChatMenu;

	QMenu * _sendSmsMenu;

	QtSms * _qtSms;

	QtWsDirectory * _qtWsDirectory;

	QtSubscribe * _qtSubscribe;

	QtLogin * _qtLogin;

	QtBrowser * _browser;

	QtContactList * _contactList;

	QLayout * _contactListTabLayout;

	QtHistoryWidget * _qtHistoryWidget;

	QtContactCallListWidget * _contactCallListWidget;

	QtProfileBar * _qtProfileBar;

	QtIdle * _qtIdle;

	QtLanguage * _qtLanguage;

	QtCallBar * _qtCallBar;

	QWidget * _configPanelWidget;

	QtStatusBar * _statusBar;

	QWidget * _chatWindow;

	QWidget * _activeTabBeforeCall;

	static const std::string ANCHOR_CONTACTLIST;

	static const std::string ANCHOR_HISTORY;

	static const std::string ANCHOR_CONFIGURATION;

	static const std::string ANCHOR_DIALPAD;

	static const std::string ANCHOR_ADDCONTACT;

	static const std::string ANCHOR_SELFCARE;

	static const std::string ANCHOR_FORUM;

	static const std::string URL_WENGO_MINI_HOME;

	static const std::string LOCAL_WEB_DIR;

	bool _background;
};

#endif	//QTWENGOPHONE_H
