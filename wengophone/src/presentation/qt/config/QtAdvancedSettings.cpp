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

#include "QtAdvancedSettings.h"

#include "ui_AdvancedSettings.h"

#include "QtAdvancedConfig.h"

#include <QtGui/QtGui>

QtAdvancedSettings::QtAdvancedSettings(QWidget * parent)
	: QObject(parent) {

	_advancedSettingsWidget = new QWidget(NULL);

	_ui = new Ui::AdvancedSettings();
	_ui->setupUi(_advancedSettingsWidget);

	connect(_ui->advancedConfigWindowButton, SIGNAL(clicked()), SLOT(showAdvancedConfigWindow()));

	readConfig();
}

QtAdvancedSettings::~QtAdvancedSettings() {
	delete _ui;
}

QString QtAdvancedSettings::getName() const {
	return tr("Advanced");
}

void QtAdvancedSettings::readConfig() {
}

void QtAdvancedSettings::saveConfig() {
}

void QtAdvancedSettings::showAdvancedConfigWindow() {
	static QtAdvancedConfig * configWindow = new QtAdvancedConfig(_advancedSettingsWidget);

	configWindow->populate();
	configWindow->show();
}
