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

#include "QtAccountSettings.h"

#include "ui_AccountSettings.h"

#include <presentation/qt/imaccount/QtIMAccountManager.h>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>

#include <model/WengoPhone.h>
#include <model/profile/UserProfile.h>

#include <QtGui>

QtAccountSettings::QtAccountSettings(CWengoPhone & cWengoPhone, QWidget * parent)
	: QObject(parent) {

	_accountSettingsWidget = new QWidget(NULL);

	_ui = new Ui::AccountSettings();
	_ui->setupUi(_accountSettingsWidget);

	QtIMAccountManager * imAccountManager = new QtIMAccountManager(cWengoPhone.getCUserProfile()->getUserProfile(),
		cWengoPhone, false, NULL);
	int index = _ui->imAccountStackedWidget->addWidget(imAccountManager->getWidget());
	_ui->imAccountStackedWidget->setCurrentIndex(index);
}

QtAccountSettings::~QtAccountSettings() {
	delete _ui;
}

QString QtAccountSettings::getName() const {
	return tr("Accounts");
}
