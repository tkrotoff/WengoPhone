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

#ifndef QTSETLOGIN_H
#define QTSETLOGIN_H

#include <QObject>

#include <string>

class QWidget;
class QDialog;

/**
 * Qt login window for other protocols.
 *
 * @author Kavous Bojnourdi
 */
 
 // Only for testing, will be removed later 
 
class QtSetLogin : public QObject {
	Q_OBJECT
public:

	QtSetLogin(QWidget * parent);

	QDialog * getWidget() const {
		return _loginWindow;
	}

	std::string getLogin() const;

	std::string getPassword() const;

	std::string getProtocol() const;

public Q_SLOTS:

	int exec();

private:

	QDialog * _loginWindow;
};

#endif	//QTSETLOGIN_H
