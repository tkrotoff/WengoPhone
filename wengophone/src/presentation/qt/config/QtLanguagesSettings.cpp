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

#include <model/config/LanguageList.h>

#include <QtGui>

QtLanguagesSettings::QtLanguagesSettings(QWidget * parent)
	: QObject(parent) {

	_languagesSettingsWidget = new QWidget(parent);

	_ui = new Ui::LanguagesSettings();
	_ui->setupUi(_languagesSettingsWidget);

	_ui->listWidget->addItem(QString::fromUtf8(LanguageList::getLanguageName("aa").c_str()));
	_ui->listWidget->addItem(QString::fromUtf8(LanguageList::getLanguageName("fr").c_str()));
	_ui->listWidget->addItem(QString::fromUtf8(LanguageList::getLanguageName("en").c_str()));
	_ui->listWidget->addItem(QString::fromUtf8(LanguageList::getLanguageName("es").c_str()));
}

QtLanguagesSettings::~QtLanguagesSettings() {
	delete _ui;
}

void QtLanguagesSettings::saveConfig() {
	/*
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::LANGUAGE_KEY, _ui->listWidget->currentItem()->text().toStdString());
	*/

	QString language = _ui->listWidget->currentItem()->text();

	QTranslator * translator = new QTranslator();
	translator->load(language);
	QApplication::installTranslator(translator);
}

void QtLanguagesSettings::readConfig() {
}
