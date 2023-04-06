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

#include "Softphone.h"

#include "../buildid/buildid.h"

#include "StatusBar.h"
#include "systray/Systray.h"
#include "contact/ContactPopupMenu.h"
#include "util/WidgetStackControl.h"
#include "SessionWindow.h"
#include "callhistory/CallHistoryWidget.h"
#include "contact/ContactList.h"
#include "contact/ContactListWidget.h"
#include "contact/XmlContactReader.h"
#include "AudioCallManager.h"
#include "contact/StreetAddress.h"
#include "login/User.h"
#include "MainWindow.h"
#include "login/LoginWindow.h"
#include "database/Database.h"
#include "database/FileSource.h"
using namespace database;
#include "config/Config.h"
#include "config/Authentication.h"
#include "callhistory/CallHistory.h"
#include "connect/Connect.h"
#include "Plugin.h"
#include "plugins/wenbox/WenboxPlugin.h"

#include <qapplication.h>
#include <qobject.h>
#include <qprocess.h>
#include <qmainwindow.h>
#include <qdir.h>
#include <qwidgetfactory.h>
#include <qstatusbar.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qiconset.h>
#include <qlistview.h>
#include <qheader.h>
#include <qdialog.h>
#include <qlistview.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qwidgetstack.h>
#include <qvbox.h>
#include <qframe.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtimer.h>

#include <cassert>
#include <iostream>
#include <string>
using namespace std;

const Q_ULLONG Softphone::BUILD_ID = BUILDID;

const QString Softphone::BUILD_ID_STRING = BUILDID_STRING;

const QString Softphone::CONTACT_FILE_EXTENSION = "vcf";
const QString Softphone::LANGUAGE_FILE_EXTENSION = "qm";

const QString Softphone::WENGO_SERVER_HOSTNAME = "ws.wengo.fr";
const QString Softphone::WENGO_SERVER_HOSTNAME_SSL = "https://ws.wengo.fr";
const QString Softphone::WENGO_DOWNLOAD_UPDATE_PATH = "/softphone-version/version.php";
const QString Softphone::WENGO_LOGIN_PATH = "/softphone-sso/sso.php";
const QString Softphone::WENGO_SMS_PATH = "/sms/sendsms.php";
//const QString Softphone::URL_CREATE_WENGO_ACCOUNT = "http://www.wengo.fr/index.php?yawl[S]=wengo.public.homePage&yawl[K]=wengo.public_128.populateOffer";
const QString Softphone::URL_VIEW_WENGO_ACCOUNT = "http://www.wengo.fr/index.php?yawl[S]=wengo.public.homePage&yawl[K]=wengo.public.displayLogin";
const QString Softphone::URL_CREATE_WENGO_ACCOUNT = "https://www.wengo.fr/public/public.php?page=subscribe_wengos";
const QString Softphone::URL_HELP_WENGO = "http://www.wengo.fr/public/public.php?page=helpcenter";
const QString Softphone::URL_FORUM_WENGO = "http://www.wengo.fr/public/public.php?page=forum";
const QString Softphone::URL_WENGO_ACCOUNT_AUTH = "https://www.wengo.fr/auth/auth.php";
#ifdef WIN32
const QString Softphone::URL_SMART_DIRECTORY = "http://www.wengo.fr/public/public.php?page=smart_directory";
#else
const QString Softphone::URL_SMART_DIRECTORY = "http://www.wengo.fr/public/public.php?page=main_smart_directory";
#endif

Softphone::Softphone() {
	_isUserLogged = false;
	_user = NULL;
	_systray = NULL;
	_wenbox = NULL;

	//Default Wengo directory
#ifdef WIN32
	//Under Windows it's better to use "wengo" rather than ".wengo"
	_wengoDir = QDir::homeDirPath() + QDir::separator() + "wengo" + QDir::separator();
	//For USB key installation, save config file in install dir
	//_wengoDir = _application->applicationDirPath() + QDir::separator() + "wengo" + QDir::separator();
#else
	_wengoDir = QDir::homeDirPath() + QDir::separator() + ".wengo" + QDir::separator();
#endif	//WIN32
}

Softphone::~Softphone() {
	delete _translator;
}

void Softphone::setQApplication(QApplication & application) {
	_application = &application;
	_translator = new QTranslator();
	_application->installTranslator(_translator);

	_applicationDirPath = _application->applicationDirPath() + QDir::separator();
	_languagesDir = _applicationDirPath + "lang" + QDir::separator();

	//For USB key
	//_wengoDir = _applicationDirPath + "wengo" + QDir::separator();
}

QWidget * Softphone::getActiveWindow() const {
	return _application->activeWindow();
}

void Softphone::loadConfig() {
	FileSource * source = NULL;
	try {
		source = new FileSource(getGeneralConfigFile());

		Database & database = Database::getInstance();
		//Creates implicitly the unique Config object
		database.transformConfigFromXml(database.extract(*source));
	} catch (const FileNotFoundException & e) {
		cerr << e.what() << endl;
	}
	delete source;
}

void Softphone::loadAuth() {
	FileSource * source = NULL;
	try {
		source = new FileSource(getAuthFile());

		Database & database = Database::getInstance();
		//Creates implicitly the unique Config object
		database.transformAuthFromXml(database.extract(*source));
	} catch (const FileNotFoundException & e) {
		cerr << e.what() << endl;
	}
	delete source;
}

const User * Softphone::loadUser() {
	FileSource * source = NULL;
	try {
		source = new FileSource(getDefaultUserFile());

		Database & database = Database::getInstance();
		_user = new User(database.transformUserFromXml(database.extract(*source)));

	} catch (const FileNotFoundException & e) {
		cerr << e.what() << endl;
	}
	delete source;

	return _user;
}

void Softphone::loadContactList() {
	ContactList & contactList = ContactList::getInstance();
	contactList.reset();

	QDir dir(getContactListDir());
	const QFileInfoList * list = dir.entryInfoList();

	if (list == NULL) {
		//The directory is unreadable or does not exist
		return;
	}

	QFileInfoListIterator it(*list);
	QFileInfo * info;

	while ((info = it.current()) != 0) {
		if (info->isFile() && info->size()) {

			Contact * contact = NULL;
			if (info->extension(false) == Softphone::CONTACT_FILE_EXTENSION) {
				//VCard
				FileSource * sourceVCard = NULL;
				try {
					sourceVCard = new FileSource(info->absFilePath());

					Database & database = Database::getInstance();
					QString tmp = database.extract(*sourceVCard);
					if (!tmp.isEmpty()) {
						contact = &database.transformFromVCard(tmp);
					}

					contact->setFilename(info->absFilePath());

				} catch (const FileNotFoundException & e) {
					cerr << e.what() << endl;
				}
				delete sourceVCard;


				QString filePath = info->absFilePath();
				filePath.remove("." + Softphone::CONTACT_FILE_EXTENSION);
				filePath.remove(".old");
				filePath += ".xml";

				//XML
				FileSource * sourceXML = NULL;
				try {
					sourceXML = new FileSource(filePath);

					if (!contact) {
						contact = new Contact();
					}
					XmlContactReader(sourceXML->read(), *contact);

				} catch (const FileNotFoundException & e) {
					cerr << e.what() << endl;
				}
				delete sourceXML;

				if (contact) {
					contactList.addContact(*contact);
				}
			}
		}

		++it;
	}
}

void Softphone::loadCallHistory() {
	CallHistory & callHistory = CallHistory::getInstance();
	callHistory.reset();

	CallHistory::loadFromXml();
}

void Softphone::setMainWindow(MainWindow & mainWindow) {
	_mainWindow = &mainWindow;
}

MainWindow & Softphone::getMainWindow() const {
	assert(_mainWindow && "Call setMainWindow() first");
	return *_mainWindow;
}

void Softphone::setSystray(Systray & systray) {
	_systray = &systray;
}

Systray & Softphone::getSystray() const {
	assert(_systray && "Call setSystray() first");
	return *_systray;
}

StatusBar & Softphone::getStatusBar() const {
	assert(_mainWindow && "Call setMainWindow() first");
	assert(_mainWindow->getStatusBar() && "Call setMainWindow() first");
	return *_mainWindow->getStatusBar();
}

void Softphone::setUserDir(const QString & userLogin) {
	_userDir = getWengoDir() + QDir::separator() + userLogin + QDir::separator();
	_isUserLogged = true;
}

const QString Softphone::getUserDir() const {
	assert(!_userDir.isEmpty() && "Call setUserLogin() first");
	return _userDir;
}

void Softphone::setUser(const User & user) {
	_user = new User(user);
}

const User & Softphone::getUser() const {
	//assert(_user && "Call loadUser() first");
	static User nullUser;
	if(!_user) {
		return nullUser;
	} else {
		return *_user;
	}
}

bool Softphone::isUserLogged() const {
	return _isUserLogged;
}

bool Softphone::isEnabled() const {
	if (getStatusBar().isUpToDate() &&
		getStatusBar().isSipEnabled() &&
		getStatusBar().isAudioEnabled()) {

		return true;
	} else {
		return false;
	}
}

void Softphone::loadPlugins() {
#ifdef WIN32
	_wenbox = new WenboxPlugin();
	_wenbox->init(Softphone::getInstance());
	_wenbox->run();
#endif	//WIN32
}

void Softphone::disposePlugins() {
#ifdef WIN32
	if (_wenbox) {
		_wenbox->dispose();
	}
#endif	//WIN32
}

void Softphone::exit(bool disconnect) {
	disposePlugins();
	/*if (_systray) {
		//Hide the systray
		_systray->hide();
	}*/
	
	Connect::getInstance().disconnect();
	getQApplication().quit();
	/*
	WaitingForUnregisterSignal * waiting = new WaitingForUnregisterSignal();
	if (disconnect) {
		//Quits when we receive the signal unregister
		QObject::connect(&Connect::getInstance(), SIGNAL(registerDone()),
				waiting, SLOT(exit()));

		Connect::getInstance().disconnect();

		//Quits after 5 seconds if the signal unregister was not received
		QTimer::singleShot(5 * 1000, waiting, SLOT(exit()));
	} else {
		waiting->exit();
	}
    */
}

/* WaitingForUnregisterSignal */

void WaitingForUnregisterSignal::exit() {
	Softphone::getInstance().getQApplication().quit();
	//::exit(EXIT_SUCCESS);
}
