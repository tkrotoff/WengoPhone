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

#include <qtutil/Object.h>
#include <qtutil/WidgetFactory.h>
#include <qtutil/Widget.h>

#include <util/Logger.h>

#include <QtGui>

QtWengoConfigDialog::QtWengoConfigDialog(CWengoPhone & cWengoPhone, QWidget * parent)
	: QObject(parent) {

	_configDialog = qobject_cast<QDialog *>(WidgetFactory::create(":/forms/config/WengoConfigDialog.ui", parent));

	_notificationWidget = new QtNotificationSettings();
	_generalSettingsWidget = new QtGeneralSettings();
	_accountSettingsWidget = new QtAccountSettings(cWengoPhone, _configDialog);
	_privacySettingsWidget = new QtPrivacySettings();
	_audioSettingsWidget = new QtAudioSettings();
	_videoSettingsWidget = new QtVideoSettings();
	_advancedSettingsWidget = new QtAdvancedSettings();
	_callForwardWidget = new QtCallForwardSettings();
	_languagesWidget = new QtLanguagesSettings(_configDialog);

	//treeWidget
	_treeWidget = Object::findChild<QTreeWidget *>(_configDialog, "treeWidget");
	connect(_treeWidget, SIGNAL(itemSelectionChanged()), SLOT(itemActivated()));
	//Hides the header of the tree view
	_treeWidget->header()->hide();

	//stackedWidget
	_stackedWidget = Object::findChild<QStackedWidget *>(_configDialog, "stackedWidget");
	_stackedWidget->addWidget(_notificationWidget);
	_stackedWidget->addWidget(_generalSettingsWidget);
	_stackedWidget->addWidget(_accountSettingsWidget->getWidget());
	_stackedWidget->addWidget(_privacySettingsWidget);
	_stackedWidget->addWidget(_audioSettingsWidget);
	_stackedWidget->addWidget(_videoSettingsWidget);
	_stackedWidget->addWidget(_callForwardWidget);
	_stackedWidget->addWidget(_advancedSettingsWidget);
	_stackedWidget->addWidget(_languagesWidget->getWidget());
	_stackedWidget->setCurrentWidget(_generalSettingsWidget);

	//saveButton
	QPushButton * saveButton = Object::findChild<QPushButton *>(_configDialog, "saveButton");
	connect(saveButton, SIGNAL(clicked()), SLOT(save()));
}

void QtWengoConfigDialog::itemActivated() {
	const QList<QTreeWidgetItem *> itemList = _treeWidget->selectedItems();
	QString itemText = itemList[0]->text(0);

	if (itemText == tr("General")) {
		_stackedWidget->setCurrentWidget(_generalSettingsWidget);
		_videoSettingsWidget->widgetHidden();
	}

	else if (itemText == tr("Languages")) {
		_stackedWidget->setCurrentWidget(_languagesWidget->getWidget());
		_videoSettingsWidget->widgetHidden();
	}

	else if (itemText == tr("Notifications & Sounds")) {
		_stackedWidget->setCurrentWidget(_notificationWidget);
		_videoSettingsWidget->widgetHidden();
	}

	else if (itemText == tr("Accounts")) {
		_stackedWidget->setCurrentWidget(_accountSettingsWidget->getWidget());
		_videoSettingsWidget->widgetHidden();
	}

	else if (itemText == tr("Privacy")) {
		_stackedWidget->setCurrentWidget(_privacySettingsWidget);
		_videoSettingsWidget->widgetHidden();
	}

	else if (itemText == tr("Audio")) {
		_stackedWidget->setCurrentWidget(_audioSettingsWidget);
		_videoSettingsWidget->widgetHidden();
	}

	else if (itemText == tr("Video")) {
		_stackedWidget->setCurrentWidget(_videoSettingsWidget);
	}

	else if (itemText == tr("Advanced")) {
		_stackedWidget->setCurrentWidget(_advancedSettingsWidget);
		_videoSettingsWidget->widgetHidden();
	}

	else if (itemText == tr("Call Forward")) {
		_stackedWidget->setCurrentWidget(_callForwardWidget);
		_videoSettingsWidget->widgetHidden();
	}

	else {
		LOG_FATAL("unknown item text=" + itemText.toStdString());
	}
}

void QtWengoConfigDialog::show() {
	_configDialog->exec();
}

void QtWengoConfigDialog::save() {
	_generalSettingsWidget->saveData();
	_languagesWidget->saveData();
	_notificationWidget->saveData();
	_privacySettingsWidget->saveData();
	_audioSettingsWidget->saveData();
	_callForwardWidget->saveData();
	_videoSettingsWidget->saveData();
	_videoSettingsWidget->widgetHidden();
}
