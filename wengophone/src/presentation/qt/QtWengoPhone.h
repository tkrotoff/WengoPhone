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

#include <cutil/global.h>
#include <util/Trackable.h>

#include <QtGui/QMainWindow>
#include <QtCore/QVariant>
#include <QtGui/QAction>

#include <string>

class CWengoPhone;
class ContactInfo;
class IMContact;
class PPhoneLine;
class PhoneCall;
class PresenceHandler;
class QtCallBar;
class QtContactCallListWidget;
class QtContactList;
class QtFileTransfer;
class QtHistoryWidget;
class QtIdle;
class QtLanguage;
class QtPhoneCall;
class QtProfileBar;
class QtSms;
class QtStatusBar;
class QtToolBar;
class QtSubscribe;
class QtSystray;
class QtToaster;
class QtWsDirectory;
class QtBrowserWidget;
class QtDialpad;
class UserProfile;
#if (defined OS_WINDOWS) && (defined QT_COMMERCIAL)
class QtWebDirectory;
#endif

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

	QtWengoPhone(CWengoPhone & cWengoPhone);

	~QtWengoPhone();

	void addPhoneCall(QtPhoneCall * qtPhoneCall);

	void addToConference(QtPhoneCall * qtPhoneCall);

	void updatePresentation();

	void dialpad(const std::string & tone);

	void connectionStatusEventHandler(int totalSteps, int currentStep, const std::string & infoMsg);

	QWidget * getWidget() const;

	void setQtDialpad(QtDialpad * qtDialpad);

	void setQtContactList(QtContactList * qtContactList);
	QtContactList * getQtContactList() const;

	void setQtHistoryWidget(QtHistoryWidget * qtHistoryWidget);

	void setQtSms(QtSms * qtSms);
	QtSms * getQtSms() const;

	void setQtWsDirectory(QtWsDirectory * qtWsDirectory);
	QtWsDirectory * getQtWsDirectory() const;

	void setQtSubscribe(QtSubscribe * qtSubscribe);
	QtSubscribe * getQtSubscribe() const;

	QtStatusBar & getQtStatusBar() const;

	QtFileTransfer * getFileTransfer() const;

	QtToolBar & getQtToolBar() const;

	QtCallBar & getQtCallBar() const;

	QtSystray & getQtSystray() const;

	QtLanguage & getQtLanguage() const;

	QtProfileBar * getQtProfileBar() const;

	QtBrowserWidget & getQtBrowserWidget() const;

	CWengoPhone & getCWengoPhone() const;

#if (defined OS_WINDOWS) && (defined QT_COMMERCIAL)
	QtWebDirectory * getQtWebDirectory() const;
#endif

	void setChatWindow(QWidget * chatWindow);
	QWidget * getChatWindow() const;

	void showHistory();

	void ensureVisible(bool doMinimizeStuff = true);

	void showAddContact(ContactInfo contactInfo);

public Q_SLOTS:

	void currentUserProfileWillDieEventHandlerSlot();

	void userProfileInitializedEventHandlerSlot();

	void hangUpButtonClicked();

	void prepareToExitApplication();

private Q_SLOTS:

	void callButtonClicked();

	void enableCallButton();

	void phoneComboBoxClicked();

	void addToConference(QString phoneNumber, PhoneCall * targetCall);

	void addToConference(PhoneCall * sourceCall, PhoneCall * targetCall);

	void closeWindow();

	void languageChanged();

Q_SIGNALS:

	/**
	 * Current user profile has been deleted.
	 *
	 * Graphical components should be re-initialized:
	 * QtContactList, QtHistoryWidget, QtProfileBar are removed.
	 * QtSystray, QtBrowserWidget have to be re-initialized.
	 */
	void userProfileDeleted();

private:

	void initThreadSafe();

	/**
	 * Initializes pickup and hangup buttons inside the main window.
	 *
	 * This is called by QtPhoneCall to re-initialize the buttons
	 * since QtPhoneCall modifies the behaviour of this buttons.
	 */
	void initCallButtons();

	void updatePresentationThreadSafe();

	void proxyNeedsAuthenticationEventHandler(NetworkProxyDiscovery & sender, NetworkProxy networkProxy);

	void proxyNeedsAuthenticationEventHandlerThreadSafe(NetworkProxy networkProxy);

	void wrongProxyAuthenticationEventHandler(NetworkProxyDiscovery & sender, NetworkProxy networkProxy);

	void makeCallErrorEventHandler();

	void makeCallErrorEventHandlerThreadSafe();

	void exitEvent();

	/** Direct link to the control. */
	CWengoPhone & _cWengoPhone;

	Ui::WengoPhoneWindow * _ui;

	QMainWindow * _wengoPhoneWindow;

	QtSystray * _qtSystray;

	QtSms * _qtSms;

	QtWsDirectory * _qtWsDirectory;

	QtSubscribe * _qtSubscribe;

	QtContactList * _qtContactList;

	QtHistoryWidget * _qtHistoryWidget;

	QtContactCallListWidget * _qtContactCallListWidget;

	QtProfileBar * _qtProfileBar;

	QtIdle * _qtIdle;

	QtLanguage * _qtLanguage;

	QtCallBar * _qtCallBar;

	QtToolBar * _qtToolBar;

	QtStatusBar * _qtStatusBar;

	QtFileTransfer * _qtFileTransfer;

	QtBrowserWidget * _qtBrowserWidget;

	QWidget * _chatWindow;

	QWidget * _activeTabBeforeCall;

#if (defined OS_WINDOWS) && (defined QT_COMMERCIAL)
	QtWebDirectory * _qtWebDirectory;
#endif
};

#endif	//OWQTWENGOPHONE_H
