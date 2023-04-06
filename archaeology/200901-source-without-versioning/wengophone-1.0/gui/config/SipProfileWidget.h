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

#ifndef SIPPROFILEWIDGET_H
#define SIPPROFILEWIDGET_H

#include <observer/Observer.h>

#include <qobject.h>

class Authentication;
class QPushButton; 
class QLineEdit;
class QCheckBox;
class QWidget;

/**
 * GUI Widget used to save the SIP profile and register onto the server.
 *
 * @author Philippe Kajmar
 * @author Tanguy Krotoff
 */
class SipProfileWidget : public QObject, public Observer {
	Q_OBJECT
public:

	SipProfileWidget(QWidget *parent);

	/**
	 * Gets the low-level widget.
	 *
	 * @return the low-level widget for this gui component
	 */
	QWidget * getWidget() const {
		return _sipProfileWidget;
	}

	/// inherits method from class Observer
	void update(const Subject &, const Event & event);

	/// save changes
	void save();

private slots:

	/**
	 * Register button is clicked, save changes.
	 * Cannot be renamed to register() since it is a C keyword.
	 */
	void doRegister();

	/**
	 * Unregister button is clicked, start stack SIP without registering.
	 */
	void unregister();

private:

	/// register
	void registering();

	/// graphical component
	QWidget * _sipProfileWidget;

	/// register button
	QPushButton * _doRegisterButton;

	/// unregister button
	QPushButton * _unregisterButton;

	/// username Line Edit
	QLineEdit * _usernameEdit;

	/// user id Line Edit
	QLineEdit * _userIdEdit;

	/// password Line Edit
	QLineEdit * _passwordEdit;

	/// realm Line Edit
	QLineEdit * _realmEdit;

	/// server edit
	QLineEdit * _serverEdit;

	/// proxy edit
	QLineEdit * _proxyEdit;

	/// port edit
	QLineEdit * _portEdit;

	/// check box designed to notice
	/// if authentication will be done at start
	QCheckBox * _autoCheckBox;

	/// check box designed to notice if
	/// registration is done via the proxy
	QCheckBox * _forceCheckBox;

	/// subject
	Authentication & _auth;
};

#endif	//SIPPROFILEWIDGET_H
