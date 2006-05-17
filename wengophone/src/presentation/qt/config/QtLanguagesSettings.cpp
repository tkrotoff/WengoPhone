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

#include "QtLanguagesSettings.h"

#include "ui_LanguagesSettings.h"

#include <presentation/qt/QtLanguage.h>

#include <model/config/LanguageList.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <QtGui>

QtLanguagesSettings::QtLanguagesSettings(QWidget * parent)
	: QObject(parent) {

	_languagesSettingsWidget = new QWidget(NULL);

	_ui = new Ui::LanguagesSettings();
	_ui->setupUi(_languagesSettingsWidget);

	_ui->listWidget->addItems(QtLanguage::getAvailableLanguages());

	readConfig();
}

QtLanguagesSettings::~QtLanguagesSettings() {
	delete _ui;
}

QString QtLanguagesSettings::getName() const {
	return tr("Language");
}

void QtLanguagesSettings::saveConfig() {
	QListWidgetItem * item = _ui->listWidget->currentItem();

	std::string language;
	if (item) {
		language = item->text().toUtf8().constData();
	}

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::LANGUAGE_KEY, LanguageList::getISO639Code(language));
}

void QtLanguagesSettings::readConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string language = LanguageList::getLanguageName(config.getLanguage());

	QList<QListWidgetItem *> list = _ui->listWidget->findItems(QString::fromUtf8(language.c_str()), Qt::MatchExactly);
	if (!list.isEmpty()) {
		_ui->listWidget->setCurrentItem(list[0]);
	}
}
