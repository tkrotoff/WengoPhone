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

#ifndef OWEDITACCOUNT_H
#define OWEDITACCOUNT_H

#include <QtGui/QDialog>

namespace Ui { class LoginPasswordDialog; }

/**
 *
 * @author Philippe Bernery
 */
class EditAccount : public QDialog {
	Q_OBJECT
public:

	EditAccount(QWidget * parent);

	~EditAccount();

	void setLogin(const QString & login);
	QString login() const;

	void setPassword(const QString & password);
	QString password() const;

private:

	Ui::LoginPasswordDialog * _ui;

};

#endif	//OWEDITACCOUNT_H
