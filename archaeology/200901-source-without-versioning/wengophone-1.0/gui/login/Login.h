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

#ifndef LOGIN_H
#define LOGIN_H

#include "User.h"
#include <qobject.h>

#include <http/HttpRequest.h>
#include <http/HttpRequestFactory.h>
#include <http/CurlHttpRequestFactory.h>

class QTimer;
class QMessageBox;

/**
 * Login procedure: sends a HTTP request to the server.
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class Login : public QObject, public HttpRequest {
	Q_OBJECT
public:

	/**
	 * Constructs a Login object.
	 *
	 * @param user user that wants to login
	 */
	Login(const User & user);

	~Login();

	void saveAsXml();

	virtual void answerReceived(const std::string & answer, Error error);

	void reinit() {
		_already = false;
	}

protected:

	void customEvent(QCustomEvent *ev);

private:

	Login(const Login &);
	Login & operator=(const Login &);

	void sendLogin();

	bool _loginOk;

	/**
	 * User that wants to login.
	 */
	User _user;

	/**
	 * Number of sent requests
	 */
	int _nbRetry;

	static const int LOGINRECEIVED_TIMEOUT;

	/**
	 * Number of limited retry if the login failed to be received.
	 *
	 * Equals 5 times.
	 */
	static const int LIMIT_NB_RETRY;

	/**
	 * Timer to check if the login was received from the server.
	 *
	 * Checks every LOGINRECEIVED_TIMEOUT seconds.
	 *
	 * @see loginReceived()
	 */
	QTimer * _timer;

	QMessageBox *_errorMessageBox;

	bool _sslProtocol;
	
	static bool _already;

private slots:

	void loginTimerSlot();
};

#endif	//LOGIN_H
