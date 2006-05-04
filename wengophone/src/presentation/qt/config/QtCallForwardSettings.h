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

#ifndef QTCALLFORWARDSETTINGS_H
#define QTCALLFORWARDSETTINGS_H

#include "QtISettings.h"

class CWengoPhone;
class QWidget;

class QString;
namespace Ui { class CallForwardSettings; }

/**
 * Call forward configuration panel.
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class QtCallForwardSettings : public QtISettings {
	Q_OBJECT
public:

	QtCallForwardSettings(CWengoPhone & cWengoPhone, QWidget * parent);

	virtual ~QtCallForwardSettings();

	QWidget * getWidget() const {
		return _callForwardSettingsWidget;
	}

	QString getName() const;

	void saveConfig();

private:

	void readConfig();

	Ui::CallForwardSettings * _ui;

	QWidget * _callForwardSettingsWidget;

	CWengoPhone & _cWengoPhone;
};

#endif	//QTCALLFORWARDSETTINGS_H
