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

#ifndef OWQTWENGOPHONE_H
#define OWQTWENGOPHONE_H

#include <presentation/PWengoPhone.h>

#include <model/network/NetworkProxyDiscovery.h>

#include <imwrapper/EnumPresenceState.h>

#include <qtutil/QObjectThreadSafe.h>

#include <util/Trackable.h>

#include <QMainWindow>
#include <QVariant>
#include <QAction>
#include <string>

class CWengoPhone;
class IMContact;
class PPhoneLine;
class PhoneCall;
class PresenceHandler;
class QtBrowser;
class QtCallBar;
class QtContactCallListWidget;
class QtContactList;
class QtHistoryWidget;
class QtIdle;
class QtLanguage;
class QtPhoneCall;
class QtProfileBar;
class QtSms;
class QtStatusBar;
class QtSubscribe;
class QtToaster;
class QtWengoStyleLabel;
class QtWsDirectory;
class QtSystray;
class UserProfile;

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
class QtWengoPhone : public QObjectThreadSafe, public PWengoPhone, public Trackable {
	Q_OBJECT
public:

	static const std::string URL_WENGO_MINI_HOME;

	static const std::string LOCAL_WEB_DIR;

	QtWengoPhone(CWengoPhone & cWengoPhone);

	virtual ~QtWengoPhone();

	void addPhoneCall(QtPhoneCall * qtPhoneCall);

	void addToConference(QtPhoneCall * qtPhoneCall);

	void setContactList(QtContactList * qtContactList);

	QtContactList * getContactList() { return _contactList;}

	void setHistory(QtHistoryWidget * qtHistoryWidget);

	/**
	 * Removes the history widget inside the history tab.
	 *
	 * Thread-safe, can be called from another thread.
	 */
	void removeHistory();

	void setPhoneCall(QtContactCallListWidget * qtContactCallListWidget);

	void setSms(QtSms * qtSms);

	void setWsDirectory(QtWsDirectory * qtWsDirectory);

	QtSms * getSms() const;

	QtWsDirectory * getQtWsDirectory() const;

	void setSubscribe(QtSubscribe * qtSubscribe);

	QtSubscribe * getSubscribe() const;

	void updatePresentation();

	void dialpad(const std::string & tone, const std::string & soundFile);

	void connectionStatusEventHandler(int totalSteps, int currentStep, const std::string & infoMsg);

	QWidget * getWidget() const {
		return _wengoPhoneWindow;
	}

	QtStatusBar & getStatusBar() const;

	QtSystray & getSystray() const;

	QtWengoStyleLabel * getCallButton() const {
		return _callButton;
	}

	QComboBox * getPhoneComboBox() const {
		return _phoneComboBox;
	}

	QtLanguage * getQtLanguage() {
		return _qtLanguage;
	}

	QtBrowser * getQtBrowser() {
		return _browser;
	}

	void setChatWindow(QWidget * chatWindow);

	void showHistory();

	CWengoPhone & getCWengoPhone() { return _cWengoPhone; }

	QtProfileBar * getProfileBar() { return _qtProfileBar; }

Q_SIGNALS:

	void connectionStatusEventHandlerSignal(int totalSteps, int currentStep, QString infoMsg);

	void signalTimeoutEventReached();

	void removeHistorySignal();

public Q_SLOTS:

	void currentUserProfileWillDieEventHandlerSlot();

	void userProfileInitializedEventHandlerSlot();

	void editMyProfile();

private Q_SLOTS:

	void removeHistorySlot();

	void connectionStatusEventHandlerSlot(int totalSteps, int currentStep, QString infoMsg);

	void callButtonClicked();

	void hangupButtonClicked();

	void enableCallButton();

	void showWengoAccount();

	void exitApplication();

	void addContact();

	void showConfig();

	void showForum();

	void showHelp();

	void showAbout();

	void showContactList();

	void showChatWindow();

	void sendSms();

	void showCreateConferenceCall();

	void showAccountSettings();

	void showFaq();

	void showCallOut();

	void showSms();

	void showSearchContactWindows();

	void showVoiceMail();

	void showHideOffLineContacts();

	void expandConfigPanel();

	void eraseHistoryOutgoingCalls();

	void eraseHistoryIncomingCalls();

	void eraseHistoryMissedCalls();

	void eraseHistoryChatSessions();

	void eraseHistorySms();

	void eraseHistory();

	void phoneComboBoxClicked();

	void addToConference(QString phoneNumber, PhoneCall * targetCall);

	void addToConference(PhoneCall * sourceCall, PhoneCall * targetCall);

	void logoff();

	void tabSelectionChanged(int index);

	void acceptCall();

	void resumeCall();

	void hangupCall();

	void hideMainWindow();

	void slotSystrayMenuCallBlank(bool checked);

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

	void updatePresentationThreadSafe();

	void proxyNeedsAuthenticationEventHandler(NetworkProxyDiscovery & sender, NetworkProxy networkProxy);

	void proxyNeedsAuthenticationEventHandlerThreadSafe(NetworkProxy networkProxy);

	void wrongProxyAuthenticationEventHandler(NetworkProxyDiscovery & sender, NetworkProxy networkProxy);

	void urlClickedEventHandler(std::string url);

	void makeCallErrorEventHandler();

	void makeCallErrorEventHandlerThreadSafe();

	int findFirstCallTab();

	QMenu * createStatusMenu();

	void timeoutEventHandler();

	QMenu * createCallMenu();

	/** Direct link to the control. */
	CWengoPhone & _cWengoPhone;

	Ui::WengoPhoneWindow * _ui;

	QMainWindow * _wengoPhoneWindow;

	QtWengoStyleLabel * _callButton;

	QtWengoStyleLabel * _hangUpButton;

	QComboBox * _phoneComboBox;

	QtSystray * _qtSystray;

	QtSms * _qtSms;

	QtWsDirectory * _qtWsDirectory;

	QtSubscribe * _qtSubscribe;

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
};

#endif	//OWQTWENGOPHONE_H
