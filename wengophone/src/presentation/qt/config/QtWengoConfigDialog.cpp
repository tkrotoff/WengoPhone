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

	_settingsList += new QtGeneralSettings(_configDialog);
	_settingsList += new QtNotificationSettings(_configDialog);
	_settingsList += new QtAccountSettings(cWengoPhone, _configDialog);
	_settingsList += new QtPrivacySettings(_configDialog);
	_settingsList += new QtAudioSettings(cWengoPhone, _configDialog);
	_settingsList += new QtVideoSettings(cWengoPhone, _configDialog);
	_settingsList += new QtAdvancedSettings(_configDialog);
	_settingsList += new QtCallForwardSettings(cWengoPhone, _configDialog);
	_settingsList += new QtLanguagesSettings(_configDialog);

	//treeWidget
	connect(_ui->treeWidget, SIGNAL(itemSelectionChanged()), SLOT(itemActivated()));
	//Hides the header of the tree view
	_ui->treeWidget->header()->hide();

	//stackedWidget
	for (unsigned i = 0; i < _settingsList.size(); i++) {
		_ui->stackedWidget->addWidget(_settingsList[i]->getWidget());
	}
	_ui->stackedWidget->setCurrentWidget(_settingsList[0]->getWidget());

	//saveButton
	connect(_ui->saveButton, SIGNAL(clicked()), SLOT(save()));
}

QtWengoConfigDialog::~QtWengoConfigDialog() {
	delete _ui;
}

void QtWengoConfigDialog::itemActivated() {
	const QList<QTreeWidgetItem *> itemList = _ui->treeWidget->selectedItems();
	QString itemText = itemList[0]->text(0);
	unsigned i;

	for (i = 0; i < _settingsList.size(); i++) {
		QtISettings * settings = _settingsList[i];
		QString name = settings->getName();
		if (name == itemText) {
			_ui->stackedWidget->setCurrentWidget(settings->getWidget());
			break;
		}
	}

	if (i >= _settingsList.size()) {
		LOG_FATAL("unknown item text=" + itemText.toStdString());
	}
}

void QtWengoConfigDialog::show() {
	_configDialog->exec();
}

void QtWengoConfigDialog::save() {
	for (unsigned i = 0; i < _settingsList.size(); i++) {
		_settingsList[i]->saveConfig();
	}
}

void QtWengoConfigDialog::showGeneralPage() {
	QList<QTreeWidgetItem *> list = _ui->treeWidget->findItems(tr("General"), Qt::MatchExactly);
	if( !list.empty() ) {
		if( list[0] ) {
			_ui->treeWidget->setCurrentItem(list[0]);
		}
	}
}

void QtWengoConfigDialog::showLanguagePage() {
	QList<QTreeWidgetItem *> list = _ui->treeWidget->findItems(tr("Language"), Qt::MatchExactly);
	if( !list.empty() ) {
		if( list[0] ) {
			_ui->treeWidget->setCurrentItem(list[0]);
		}
	}
}

void QtWengoConfigDialog::showAccountsPage() {
	QList<QTreeWidgetItem *> list = _ui->treeWidget->findItems(tr("Accounts"), Qt::MatchExactly);
	if( !list.empty() ) {
		if( list[0] ) {
			_ui->treeWidget->setCurrentItem(list[0]);
		}
	}
}

void QtWengoConfigDialog::showPrivacyPage() {
	QList<QTreeWidgetItem *> list = _ui->treeWidget->findItems(tr("Privacy"), Qt::MatchExactly);
	if( !list.empty() ) {
		if( list[0] ) {
			_ui->treeWidget->setCurrentItem(list[0]);
		}
	}
}

void QtWengoConfigDialog::showAudioPage() {
	QList<QTreeWidgetItem *> list = _ui->treeWidget->findItems(tr("Audio"), Qt::MatchExactly);
	if( !list.empty() ) {
		if( list[0] ) {
			_ui->treeWidget->setCurrentItem(list[0]);
		}
	}
}

void QtWengoConfigDialog::showVideoPage() {
	QList<QTreeWidgetItem *> list = _ui->treeWidget->findItems(tr("Video"), Qt::MatchExactly);
	if( !list.empty() ) {
		if( list[0] ) {
			_ui->treeWidget->setCurrentItem(list[0]);
		}
	}
}

void QtWengoConfigDialog::showNotificationsPage() {
	QList<QTreeWidgetItem *> list = _ui->treeWidget->findItems(tr("Notifications & Sounds"), Qt::MatchExactly);
	if( !list.empty() ) {
		if( list[0] ) {
			_ui->treeWidget->setCurrentItem(list[0]);
		}
	}
}

void QtWengoConfigDialog::showCallForwardPage() {
	QList<QTreeWidgetItem *> list = _ui->treeWidget->findItems(tr("Call Forward"), Qt::MatchExactly);
	if( !list.empty() ) {
		if( list[0] ) {
			_ui->treeWidget->setCurrentItem(list[0]);
		}
	}
}

void QtWengoConfigDialog::showAdvancedPage() {
	QList<QTreeWidgetItem *> list = _ui->treeWidget->findItems(tr("Advanced"), Qt::MatchExactly);
	if( !list.empty() ) {
		if( list[0] ) {
			_ui->treeWidget->setCurrentItem(list[0]);
		}
	}
}
