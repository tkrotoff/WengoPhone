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

#ifndef OWQTYAHOOSETTINGS_H
#define OWQTYAHOOSETTINGS_H

#include "QtIMAccountPlugin.h"

#include <string>

class IMAccount;
class UserProfile;

class QWidget;
namespace Ui { class YahooSettings; }

/**
 * Yahoo! settings for the user.
 *
 * @author Tanguy Krotoff
 */
class QtYahooSettings : public QtIMAccountPlugin {
	Q_OBJECT
public:

	QtYahooSettings(UserProfile & userProfile, IMAccount * imAccount, QDialog * parent);

	~QtYahooSettings();

	QWidget * getWidget() const {
		return _IMSettingsWidget;
	}

	virtual bool isValid() const;

	virtual void save();

public Q_SLOTS:

	void forgotPasswordButtonClicked();

	void createAccountButtonClicked();

private:

	void init();

	Ui::YahooSettings * _ui;

	QWidget * _IMSettingsWidget;
};

#endif	//OWQTYAHOOSETTINGS_H