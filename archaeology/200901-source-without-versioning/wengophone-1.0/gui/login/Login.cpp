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

#include "Login.h"
#include "Softphone.h"
#include "MainWindow.h"
#include "StatusBar.h"
#include "User.h"
#include "database/Database.h"
using namespace database;
#include "database/FileTarget.h"
#include "config/Authentication.h"
#include "config/AdvancedConfig.h"
#include "exception/FileNotFoundException.h"
#include "exception/FileNotFoundException.h"
#include "callhistory/CallHistory.h"
#include "connect/Connect.h"
#include "contact/ContactList.h"
#include "contact/Contact.h"
#include "AudioCallManager.h"

#include "softphone-config.h"

#include <timer/Timer.h>

#include <qstring.h>
#include <qurl.h>
#include <qmessagebox.h>
#include <qmainwindow.h>
#include <qtimer.h>
#include <qevent.h>
#include <qapplication.h>
#include <qaction.h>

#include <cassert>
#include <iostream>
using namespace std;

//#define PRINT_SSO

const int Login::LIMIT_NB_RETRY = 3;
const int Login::LOGINRECEIVED_TIMEOUT = 15000;
bool Login::_already = false;

/**
 * Custom event for login error.
 *
 * @author Mathieu Stute
 */
class LoginErrorEvent : public QCustomEvent {
public:
	static const int type = QEvent::User + 202;

	LoginErrorEvent() : QCustomEvent(type) {
	}
};

class LoginOkEvent : public QCustomEvent {
public:
	static const int type = QEvent::User + 203;

	LoginOkEvent() : QCustomEvent(type) {
	}
};

Login::Login(const User & user) {

	HttpRequestFactory * factory = new CurlHttpRequestFactory();
	HttpRequest::setFactory(factory);

	_user = user;
	_nbRetry = 0;
	_loginOk = false;

	Softphone & softphone = Softphone::getInstance();

	//Disconnect from the server
	Connect::getInstance().disconnect();

	//Change MainWindow title
	QMainWindow * mainWindow = softphone.getMainWindow().getWidget();

	//No translation for WengoPhone
	mainWindow->setCaption("WengoPhone - " + _user.getLogin());

	//Sets the user directory
	softphone.setUserDir(_user.getLogin());

	//Loads all the configuration files
	softphone.loadCallHistory();
	softphone.loadContactList();

	//FIXME
	//_contact inside SessionWindow is not NULL
	//after we reload the ContactList even if ContactList::eraseAll() does it.
	//Now it works and does not crash anymore.
	//this is really ugly.
	AudioCallManager::getInstance().reset();

	_errorMessageBox = new QMessageBox(
		tr("Login error"),
		tr("Incorrect login or password"),
		QMessageBox::Critical,
		QMessageBox::Ok,
		QMessageBox::NoButton,
		QMessageBox::NoButton,
		Softphone::getInstance().getActiveWindow());

	AdvancedConfig & conf = AdvancedConfig::getInstance();
	_sslProtocol = conf.useSSL();
	sendLogin();

	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), this, SLOT(loginTimerSlot()));
	_timer->start(LOGINRECEIVED_TIMEOUT);
}

Login::~Login() {
	delete _timer;
	delete _errorMessageBox;
}

void Login::loginTimerSlot() {
	cout << "Login Timer Slot " << endl;
	if (!_loginOk) {
		sendLogin();
	}
}

void Login::answerReceived(const std::string & answer, Error error) {
	cout << endl << "Login: anwser received" << endl;
	_timer->stop();
	if (error == NoError) {
		cerr << "Login: login received" << endl;
		_loginOk = true;
		//Gets the authentication informations from the login received from the server
		Authentication & auth = Database::getInstance().transformAuthFromXml(answer);
#ifdef PRINT_SSO
		cout << answer << endl;
#endif
		//Checks if the login/password are correct
		static const int STATUS_CODE_OK = 200;
		if (auth.getStatusCode() != STATUS_CODE_OK) {
			cerr << "Bad Login and/or password" << endl;
			QApplication::postEvent(this, new LoginErrorEvent());
		} else {
			QApplication::postEvent(this, new LoginOkEvent());
		}
	}

	else {
		cout << endl << "Login: Error " << _nbRetry << endl;
		_nbRetry++;
		if (_nbRetry > LIMIT_NB_RETRY) {
			_nbRetry = 0;
			//Tries without SSL
			_sslProtocol = false;
			cerr << "Login: switch to no SSL" << endl;
		}
		sendLogin();
	}
}

void Login::saveAsXml() {
	using namespace database;

	Database & database = Database::getInstance();
	QString xml = database.transformUserToXml(_user);

	FileTarget * target = NULL;
	try {
		target = new FileTarget(Softphone::getInstance().getDefaultUserFile());
	} catch (const FileNotFoundException & e) {
		cerr << e.what() << endl;
		delete target;
		return;
	}

	database.load(xml, *target);
	delete target;
}

void Login::customEvent(QCustomEvent * event) {

	if (_already) {
		return;
	}
	_already = true;
	
	switch (event->type()) {
	case LoginOkEvent::type: {
		Connect & connect = Connect::getInstance();
		connect.connect();
		break;
	}

	case LoginErrorEvent::type: {
		cerr << "Login: customEvent Login error\n";
		Connect & connect = Connect::getInstance();
		connect.disconnect();
		Softphone::getInstance().getStatusBar().setSipStatus(false);
		_errorMessageBox->exec();
		MainWindow & mainWindow = Softphone::getInstance().getMainWindow();
		QAction * changeProfileAction = (QAction *) mainWindow.getWidget()->child("changeProfileAction", "QAction");
		changeProfileAction->activate(); 
		break;
	}

	default:
		assert(NULL && "Wrong event type");
		break;
	}
}

void Login::sendLogin() {
	QString login = _user.getLogin();
	QUrl::encode(login);
	QString password = _user.getPassword();
	QUrl::encode(password);
	QString data = "login=" + login + "&password=" + password + "&wl="+WL_TAG;

	if (_sslProtocol) {
		cout << "Login: SSL enabled" << endl;
		sendRequest(true, Softphone::WENGO_SERVER_HOSTNAME, 443, Softphone::WENGO_LOGIN_PATH, data, true);
	} else {
		cout << "Login: SSL disabled" << endl;
		sendRequest(false, Softphone::WENGO_SERVER_HOSTNAME, 80, Softphone::WENGO_LOGIN_PATH, data, true);
	}
}
