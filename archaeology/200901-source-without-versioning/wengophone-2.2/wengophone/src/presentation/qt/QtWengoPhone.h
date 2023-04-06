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

#ifndef OWQTWENGOPHONE_H
#define OWQTWENGOPHONE_H

#include <presentation/PWengoPhone.h>

#include <model/phoneline/EnumMakeCallError.h>
#include <model/network/NetworkProxyDiscovery.h>

#include <imwrapper/EnumPresenceState.h>

#include <qtutil/QObjectThreadSafe.h>

#include <cutil/global.h>
#include <util/Trackable.h>

#include <QtGui/QMainWindow>
#include <QtCore/QVariant>
#include <QtGui/QAction>

#include <string>
#include <memory>

class CWengoPhone;
class Config;
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
class QtSystray;
class QtToaster;
class QtBrowserWidget;
class QtDialpad;
class UserProfile;
class QtPhoneComboBox;
class QtIMAccountMonitor;

#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
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

	QtStatusBar & getQtStatusBar() const;

	QtFileTransfer * getFileTransfer() const;

	QtToolBar & getQtToolBar() const;

	QtCallBar & getQtCallBar() const;

	QtSystray & getQtSystray() const;

	QtLanguage & getQtLanguage() const;

	QtProfileBar * getQtProfileBar() const;

	CWengoPhone & getCWengoPhone() const;

#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
	QtWebDirectory * getQtWebDirectory() const;
#endif

	void setChatWindow(QWidget * chatWindow);
	QWidget * getChatWindow() const;

	void showHistory();

	void ensureVisible();

	void showAddContact(ContactInfo contactInfo);

	virtual void bringMainWindowToFront();

	void installQtBrowserWidget();

	void uninstallQtBrowserWidget();

	QtBrowserWidget * getQtBrowserWidget() const;

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

	void closeWindow();

	void languageChanged();

	void setActiveTabBeforeCallCurrent();

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
	void initUi();

	void initThreadSafe();

	/**
	 * Set the geometry of the mainwindow (position and size).
	 *
	 * Load last size and position. If the mainwindow is not visible use default values.
	 */
	void mainWindowGeometry(Config & config);

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

	void makeCallErrorEventHandler(EnumMakeCallError::MakeCallError);

	void makeCallErrorEventHandlerThreadSafe(EnumMakeCallError::MakeCallError);

	void exitEvent();

#ifdef OS_MACOSX
	void fixMacOSXMenus();
#endif

	void loadStyleSheets();

	/** Direct link to the control. */
	CWengoPhone & _cWengoPhone;

	Ui::WengoPhoneWindow * _ui;

	QMainWindow * _wengoPhoneWindow;

	QtSystray * _qtSystray;

	QtSms * _qtSms;

	QtContactList * _qtContactList;

	QtHistoryWidget * _qtHistoryWidget;

	QtContactCallListWidget * _qtContactCallListWidget;

	QtIdle * _qtIdle;

	QtLanguage * _qtLanguage;

	QtToolBar * _qtToolBar;

	QtStatusBar * _qtStatusBar;

	QtFileTransfer * _qtFileTransfer;

	QtBrowserWidget * _qtBrowserWidget;

	int _qtBrowserWidgetTabIndex;

	QWidget * _chatWindow;

	QWidget * _activeTabBeforeCall;

	std::auto_ptr<QtIMAccountMonitor> _qtIMAccountMonitor;

#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
	QtWebDirectory * _qtWebDirectory;
#endif
};

#endif	//OWQTWENGOPHONE_H
