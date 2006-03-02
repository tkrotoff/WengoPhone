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

#include "QtAdvancedConfig.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <Object.h>
#include <WidgetFactory.h>

#include <QtGui>

static const int PREFERENCE_NAME_COLUMN = 0;
static const int STATUS_COLUMN = 1;
static const int TYPE_COLUMN = 2;
static const int VALUE_COLUMN = 3;

QtAdvancedConfig::QtAdvancedConfig(QWidget * parent) : QObject() {
	_advancedConfigWindow = WidgetFactory::create(":/forms/AdvancedConfigWindow.ui", parent);

	QTableWidget * tableWidget = Object::findChild<QLabel *>(_advancedConfigWindow, "tableWidget");

	StringList keys = config.getAllKeys();
	for (int i = 0; i < keys; i++) {
		tableWidget->insertRow(i);

		tableWidget->
	}
}
