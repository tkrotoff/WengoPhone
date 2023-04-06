/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "PhoneComboBox.h"

#include <qobject.h>

class ContactList;
class ContactListWidget;
class CallHistoryWidget;
class WidgetStackControl;
class SearchWindow;
class StatusBar;
class AboutWindow;
class ConfigWindow;
class Contact;
class PresenceStatus;
class PresencePopupMenu;
class HomePageWidget;
class QMainWindow;
class QWidgetStack;
class QComboBox;
class QAction;
class QPushButton;
class QSlider;
class QLabel;
class QToolBar;
class SearchPageWidget;
class QMenuBar;
class QPopupMenu;
class QCheckBox;

/**
 * Main window of the softphone.
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class MainWindow : public QObject {
	Q_OBJECT
public:

	MainWindow();

	~MainWindow();

	/**
	 * Gets the low-level gui widget of this class.
	 *
	 * @return low-level widget
	 */
	QMainWindow * getWidget() const {
		return _mainWindow;
	}

	StatusBar * getStatusBar() const {
		return _statusBar;
	}

	QComboBox * getPhoneComboBox() const {
		return _phoneComboBox->getWidget();
	}

	CallHistoryWidget & getCallHistoryWidget() const;

	QSlider * getInputSoundSlider() const {
		return _inputSoundSlider;
	}

	QSlider * getOutputSoundSlider() const {
		return _outputSoundSlider;
	}

	HomePageWidget * getHomePageWidget(){
		return _homePageWidget;
	}

	ConfigWindow & getConfigWindow() const;

	ContactListWidget * getContactListWidget(){
		return _contactListWidget;
	}

	AboutWindow * getAboutWindow() {
		return _aboutWindow;
	}

	void raiseHomePage();

	void raiseContactList();

	void raiseHistory();

	PresencePopupMenu * getPresencePopupMenu() {
		return _presencePopupMenu;
	}
	
	QCheckBox * getEnableVideoCheckBox() {
		return _enableVideoCheckBox;
	}
	
	static const QString ONNETCALL_TAG;
	static const QString CELLPHONECALL_TAG;
	static const QString HOMECALL_TAG;
	static const QString WORKCALL_TAG;
	static const QString CHAT_TAG;
	static const QString SMS_TAG;

public slots:

	void showAsActiveWindow();

	/**
	 * The dialpad button has been pressed.
	 *
	 * The SessionWindow has to be showed.
	 */
	void showDialpad();

	/**
	 * Show the session window with the SMS page
	 */
	void showSMSSessionWindow();

	/**
	 * Show the about dialog
	 */
	void showAbout();

	/**
	 * Show the search contacts dialog
	 */
	void showSearchContactDialog();

	/**
	 * Show the add contact window
	 */
	void showAddContact();

	/**
	 * Show the configuration window
	 */
	void showConfigurationWindow();

	PresencePopupMenu * getPresencePopupMenu() const {
		return _presencePopupMenu;
	}

	/**
	 * The presence status of the Contact has been updated.
	 *
	 * @param presenceStatus new presence status of the user
	 */
	void presenceStatusChanged(const PresenceStatus & presenceStatus);

	/**
	 * Open the account creation web page in the system browser.
	 */
	void showAccountCreation();

	/**
	 * Open the credit account web page in the system browser.
	 */
	void showCreditAccount();

	/**
	 * Open the selfcare web page in the system browser.
	 */
	void showMyWengoAccount();

	/**
	 * Open the forum web page in the system browser.
	 */
	void showForum();

	/**
	 * Open the help web page in the system browser.
	 */
	void showHelp();

	void PCtoPCmode();

private slots:

	/**
	 * Exits from the Softphone.
	 */
	void exit();

	void close();

	void switchAudioPanel();

	void buildActionMenu();

	void actionMenuSlot(int id);

	void contactListActivated();

	void homePageActivated();

	void callHistoryActivated();

	void goToAudioConfig();

	void goToVideoConfig();

private:

	MainWindow(const MainWindow &);
	MainWindow & operator=(const MainWindow &);

	void acessAuthWebSite(QString page);

	void acessWebSite(QString url);

	void unRegister();

	void sendSms(Contact & c);

	void startChat(Contact & c);

	void cellPhoneCall(Contact & c);

	void homePhoneCall(Contact & c);

	void workPhoneCall(Contact & c);

	void onNetCall(Contact & c);

	/**
	 * Low-level widget of this class.
	 */
	QMainWindow * _mainWindow;

	/**
	 * Stack to switch between ContactListWidget and CallHistoryWidget.
	 */
	QWidgetStack * _stack;

	QPushButton * _callButton;

	/**
	 * Button used by the user to select its presence (online, offline...).
	 */
	QPushButton * _myPresenceStatusButton;

	/**
	 * A direct handle to the menu bar (to insert the customized action menu)
	 */
	QMenuBar * _menuBar;

	/**
	 * A direct handle to the action menu
	 */
	QPopupMenu * _actionMenu;

	PhoneComboBox * _phoneComboBox;
	
	QAction * _dialpadAction;
	
	QAction * _enableVideoAction;
	
	QAction * _smartDirAction;
	
	QSlider * _inputSoundSlider;
	
	QSlider * _outputSoundSlider;
	
	HomePageWidget * _homePageWidget;
	
	AboutWindow * _aboutWindow;
	
	SearchPageWidget * _searchPageWidget;
	
	QLabel * _audioSettingsLabel;
	QLabel * _videoSettingsLabel;
	QCheckBox * _enableVideoCheckBox;
	/**
	 * Only for memory leak purpose.
	 */
	ContactListWidget * _contactListWidget;
	CallHistoryWidget * _callHistoryWidget;
	WidgetStackControl * _stackControl;
	StatusBar * _statusBar;
	SearchWindow * _searchWindow;
	ConfigWindow * _configWindow;
	PresencePopupMenu * _presencePopupMenu;
	
 	QLabel * _audioSwitchLabel;
	QWidgetStack * _audioPanel;
	bool _showAudioPanel;
};

#endif	//MAINWINDOW_H
