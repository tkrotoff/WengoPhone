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

#ifndef QTJABBERSETTINGS_H
#define QTJABBERSETTINGS_H

#include "QtIMAccountPlugin.h"

class IMAccount;
class UserProfile;
class QWidget;
class QLineEdit;
class QCheckBox;

class QtJabberSettings : public QtIMAccountPlugin {
	Q_OBJECT
public:

	QtJabberSettings(UserProfile & userProfile, IMAccount * imAccount, QWidget * parent);

	QWidget * getWidget() const {
		return _IMSettingsWidget;
	}

public Q_SLOTS:

	void save();

private:

	void init();

	QWidget * _IMSettingsWidget;

	QLineEdit * _loginLineEdit;

	QLineEdit * _passwordLineEdit;

	QCheckBox * _useTLSCheckBox;

	QCheckBox * _requireTLSCheckBox;

	QCheckBox * _forceOldSSLCheckBox;

	QCheckBox * _allowPlainTextAuthenticationCheckBox;

	QLineEdit * _connectServerLineEdit;

	QLineEdit * _portLineEdit;
};

#endif	//QTJABBERSETTINGS_H
