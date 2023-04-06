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

#ifndef SOFTPHONE_H
#define SOFTPHONE_H

#include <singleton/Singleton.h>

#include <qobject.h>
#include <qstring.h>
#include <qmap.h>
#include <qdir.h>
#include <qstringlist.h>

class MainWindow;
class StatusBar;
class CallHistory;
class ContactList;
class WenboxPlugin;
class User;
class Systray;
class QApplication;
class QProcess;
class QTranslator;
class QWidget;

/**
 * Global constants and variables for the application.
 *
 * Design Pattern Singleton
 *
 * The instance of QApplication is needed to set the language,
 * that is why this class was created as a singleton.
 *
 * @author Tanguy Krotoff
 */
class Softphone : public Singleton<Softphone> {
	friend class Singleton<Softphone>;
public:

	/**
	 * Build number of the softphone.
	 *
	 * Q_ULLONG = unsigned long long
	 * 64bits from 0 to 18446744073709551615
	 * thus perfect for a date like:
	 * YYYYMMDDHHMMSS
	 * 20041215105700
	 */
	static const Q_ULLONG BUILD_ID;

	/**
	 * Commercial name for a release/buildid like 1.2 or 0.88.
	 *
	 * This is simply a more human readable way to deal with the buildid.
	 * Should be a number (double).
	 */
	static const QString BUILD_ID_STRING;

	/**
	 * Extension of the VCard format (*.vcf).
	 */
	static const QString CONTACT_FILE_EXTENSION;

	static const QString URL_CREATE_WENGO_ACCOUNT;

	static const QString URL_VIEW_WENGO_ACCOUNT;

	static const QString URL_HELP_WENGO;

	static const QString URL_FORUM_WENGO;

	static const QString WENGO_SERVER_HOSTNAME;

	static const QString WENGO_SERVER_HOSTNAME_SSL;

	static const QString WENGO_DOWNLOAD_UPDATE_PATH;

	static const QString WENGO_LOGIN_PATH;

	static const QString WENGO_SMS_PATH;

	static const QString LANGUAGE_FILE_EXTENSION;

	static const QString URL_WENGO_ACCOUNT_AUTH;

	static const QString URL_SMART_DIRECTORY;

	~Softphone();

	void setWengoDir(const QString & wengoDir) {
		_wengoDir = wengoDir;
	}

	const QString getApplicationDirPath() const {
		return _applicationDirPath;
	}

	/**
	 * Root directory where everything is saved.
	 *
	 * Under Unix: $HOME/.wengo/ where $HOME = /home/$USER/
	 * Under Windows 2k and XP: $HOME/wengo/ where $HOME = /Documents and Settings/$USER/
	 * Under Windows 98: $HOME/wengo/ where $HOME = C:/
	 */
	const QString getWengoDir() const {
		return _wengoDir;
	}

	const QString getLanguagesDir() const {
		return _languagesDir;
	}

	const QString getDefaultUserFile() const {
		return getWengoDir() + "user.config";
	}

	/**
	 * Configuration file (XML format).
	 */
	const QString getGeneralConfigFile() const {
		return getWengoDir() + "wengo.config";
	}

	/**
	 * Directory where is saved the ContactList.
	 */
	const QString getContactListDir() const {
		return getUserDir() + "contacts" + QDir::separator();
	}

	/**
	 * Gets the Authentication file, only available in debug mode.
	 *
	 * @return Authertication filename
	 * @see Authentication
	 */
	const QString getAuthFile() const {
		return getUserDir() + "auth.config";
	}

	/**
	 * Gets the signature file for SMS.
	 *
	 * @return SMS signature filename
	 * @see Sms
	 */
	const QString getSignatureFile() const {
		return getUserDir() + "signature.txt";
	}

	/**
	 * Gets the file where is saved CallHistory.
	 *
	 * @return CallHistory filename
	 * @see CallHistory
	 */
	const QString getCallHistoryFile() const {
		return getUserDir() + "callhistory" + QDir::separator() + "callhistory.xml";
	}

	void setQApplication(QApplication & application);

	QApplication & getQApplication() const {
		return *_application;
	}

	/**
	 * Gets the current active window (the one with the focus)
	 * or the MainWindow.
	 *
	 * @return the active window
	 */
	QWidget * getActiveWindow() const;

	void setMainWindow(MainWindow & mainWindow);

	MainWindow & getMainWindow() const;

	void setSystray(Systray & systray);

	Systray & getSystray() const;

	StatusBar & getStatusBar() const;

	QTranslator & getTranslator() const {
		return *_translator;
	}

	/**
	 * If the softphone is enabled or not: if the user can make a call.
	 *
	 * @return true if the user can make a call, false otherwise.
	 */
	bool isEnabled() const;

	/**
	 * If the user is logged or not.
	 *
	 * Logged does not mean that the user is authenticated,
	 * just that he entered a login inside the LoginWindow.
	 *
	 * @return true if the user is logged, false otherwise.
	 */
	bool isUserLogged() const;

	/**
	 * Sets the User (his login/password).
	 *
	 * @param user User of the softphone
	 */
	void setUser(const User & user);

	/**
	 * Gets the User (login/password) of the softphone.
	 *
	 * @return User of the softphone or NULL 
	 */
	const User & getUser() const;

	void setUserDir(const QString & userLogin);

	void loadConfig();

	void loadAuth();

	const User * loadUser();

	void loadContactList();

	void loadCallHistory();

	void loadPlugins();

	void exit(bool disconnect = true);

private:

	Softphone();

	Softphone(const Softphone &);
	Softphone & operator=(const Softphone &);

	const QString getUserDir() const;

	void disposePlugins();

	QApplication * _application;

	QTranslator * _translator;

	bool _isUserLogged;

	QStringList _languageList;

	MainWindow * _mainWindow;

	QString _applicationDirPath;

	QString _userDir;

	QString _wengoDir;

	QString _languagesDir;

	User * _user;

	Systray * _systray;

	WenboxPlugin * _wenbox;
};

/**
 * Wait for phApi to finish its unregister process
 * when the user quits WengoPhone, otherwise WengoPhone
 * is not unregister on the SIP server.
 *
 * This is quite an ugly trick.
 *
 * @author Tanguy Krotoff
 */
class WaitingForUnregisterSignal : public QObject {
	Q_OBJECT;
public:

	WaitingForUnregisterSignal() {
	}

public slots:

	void exit();

private:
};

#endif	//SOFTPHONE_H
