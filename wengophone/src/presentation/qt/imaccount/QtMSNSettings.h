/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#ifndef QTMSNSETTINGS_H
#define QTMSNSETTINGS_H

#include "QtIMAccountPlugin.h"

class IMAccount;
class UserProfile;
class QWidget;
class QLineEdit;

class QtMSNSettings : public QtIMAccountPlugin {
	Q_OBJECT
public:

	QWidget * getWidget() const {
		return _MSNSettingsWidget;
	}

public Q_SLOTS:

	void save();

private:

	void init();

	QWidget * _MSNSettingsWidget;

	QLineEdit * _loginLineEdit;

	QLineEdit * _passwordLineEdit;
};

#endif	//QTMSNSETTINGS_H
