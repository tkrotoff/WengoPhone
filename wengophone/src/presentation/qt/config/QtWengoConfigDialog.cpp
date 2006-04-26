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

#include "QtWengoConfigDialog.h"

#include "ui_WengoConfigDialog.h"

#include "QtNotificationSettings.h"
#include "QtGeneralSettings.h"
#include "QtAccountSettings.h"
#include "QtPrivacySettings.h"
#include "QtAudioSettings.h"
#include "QtVideoSettings.h"
#include "QtAdvancedSettings.h"
#include "QtCallForwardSettings.h"
#include "QtLanguagesSettings.h"

#include <util/Logger.h>

#include <QtGui>

QtWengoConfigDialog::QtWengoConfigDialog(CWengoPhone & cWengoPhone, QWidget * parent)
	: QObject(parent) {

	_configDialog = new QDialog(parent);

	_ui = new Ui::WengoConfigDialog();
	_ui->setupUi(_configDialog);

	_generalSettingsWidget = new QtGeneralSettings(_configDialog);
	_notificationSettingsWidget = new QtNotificationSettings(_configDialog);
	_accountSettingsWidget = new QtAccountSettings(cWengoPhone, _configDialog);
	_privacySettingsWidget = new QtPrivacySettings(_configDialog);
	_audioSettingsWidget = new QtAudioSettings(_configDialog);
	_videoSettingsWidget = new QtVideoSettings(_configDialog);
	_advancedSettingsWidget = new QtAdvancedSettings(_configDialog);
	_callForwardSettingsWidget = new QtCallForwardSettings(_configDialog);
	_languagesSettingsWidget = new QtLanguagesSettings(_configDialog);

	//treeWidget
	connect(_ui->treeWidget, SIGNAL(itemSelectionChanged()), SLOT(itemActivated()));
	//Hides the header of the tree view
	_ui->treeWidget->header()->hide();

	//stackedWidget
	_ui->stackedWidget->addWidget(_generalSettingsWidget->getWidget());
	_ui->stackedWidget->addWidget(_notificationSettingsWidget->getWidget());
	_ui->stackedWidget->addWidget(_accountSettingsWidget->getWidget());
	_ui->stackedWidget->addWidget(_privacySettingsWidget->getWidget());
	_ui->stackedWidget->addWidget(_audioSettingsWidget->getWidget());
	_ui->stackedWidget->addWidget(_videoSettingsWidget->getWidget());
	_ui->stackedWidget->addWidget(_callForwardSettingsWidget->getWidget());
	_ui->stackedWidget->addWidget(_advancedSettingsWidget->getWidget());
	_ui->stackedWidget->addWidget(_languagesSettingsWidget->getWidget());
	_ui->stackedWidget->setCurrentWidget(_generalSettingsWidget->getWidget());

	//saveButton
	connect(_ui->saveButton, SIGNAL(clicked()), SLOT(save()));
}

QtWengoConfigDialog::~QtWengoConfigDialog() {
	delete _ui;
}

void QtWengoConfigDialog::itemActivated() {
	const QList<QTreeWidgetItem *> itemList = _ui->treeWidget->selectedItems();
	QString itemText = itemList[0]->text(0);

	if (itemText == tr("General")) {
		_ui->stackedWidget->setCurrentWidget(_generalSettingsWidget->getWidget());
		_videoSettingsWidget->hide();
	}

	else if (itemText == tr("Languages")) {
		_ui->stackedWidget->setCurrentWidget(_languagesSettingsWidget->getWidget());
		_videoSettingsWidget->hide();
	}

	else if (itemText == tr("Notifications & Sounds")) {
		_ui->stackedWidget->setCurrentWidget(_notificationSettingsWidget->getWidget());
		_videoSettingsWidget->hide();
	}

	else if (itemText == tr("Accounts")) {
		_ui->stackedWidget->setCurrentWidget(_accountSettingsWidget->getWidget());
		_videoSettingsWidget->hide();
	}

	else if (itemText == tr("Privacy")) {
		_ui->stackedWidget->setCurrentWidget(_privacySettingsWidget->getWidget());
		_videoSettingsWidget->hide();
	}

	else if (itemText == tr("Audio")) {
		_ui->stackedWidget->setCurrentWidget(_audioSettingsWidget->getWidget());
		_videoSettingsWidget->hide();
	}

	else if (itemText == tr("Video")) {
		_ui->stackedWidget->setCurrentWidget(_videoSettingsWidget->getWidget());
	}

	else if (itemText == tr("Advanced")) {
		_ui->stackedWidget->setCurrentWidget(_advancedSettingsWidget->getWidget());
		_videoSettingsWidget->hide();
	}

	else if (itemText == tr("Call Forward")) {
		_ui->stackedWidget->setCurrentWidget(_callForwardSettingsWidget->getWidget());
		_videoSettingsWidget->hide();
	}

	else {
		LOG_FATAL("unknown item text=" + itemText.toStdString());
	}
}

void QtWengoConfigDialog::show() {
	_configDialog->exec();
}

void QtWengoConfigDialog::save() {
	_generalSettingsWidget->saveConfig();
	_languagesSettingsWidget->saveConfig();
	_notificationSettingsWidget->saveConfig();
	_privacySettingsWidget->saveConfig();
	_audioSettingsWidget->saveConfig();
	_callForwardSettingsWidget->saveConfig();
	_videoSettingsWidget->saveConfig();
	_videoSettingsWidget->hide();
}
