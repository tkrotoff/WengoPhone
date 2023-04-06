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

#ifndef USER_H
#define USER_H

#include <qstring.h>

class User {
public:

	User() {
		_autoLogin = false;
		_password = "";
		_login = "";
	}

	User(const QString & login, const QString & password, bool autoLogin) {
		_login = login;
		_password = password;
		_autoLogin = autoLogin;
	}

	QString getLogin() const {
		return _login;
	}

	QString getPassword() const {
		return _password;
	}

	bool autoLogin() const {
		return _autoLogin;
	}

private:

	/*User();
	User(const User &);
	User & operator=(const User &);*/

	QString _login;

	QString _password;

	bool _autoLogin;
};

#endif	//USER_H
