/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include "QtLanguage.h"

#include <model/config/LanguageList.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/Logger.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>

static const char * LANGUAGE_FILE_EXTENSION = "qm";
static const char * LANG_DIR = "lang";

QtLanguage::QtLanguage(QObject * parent)
	: QObjectThreadSafe(parent) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.valueChangedEvent += boost::bind(&QtLanguage::configChangedEventHandler, this, _1, _2);

	_qtTranslator = new QTranslator(QCoreApplication::instance());
	QCoreApplication::installTranslator(_qtTranslator);

	_appTranslator = new QTranslator(QCoreApplication::instance());
	QCoreApplication::installTranslator(_appTranslator);

	loadLanguageFromConfig();
}

QtLanguage::~QtLanguage() {
}

void QtLanguage::configChangedEventHandler(Settings & sender, const std::string & key) {
	typedef PostEvent2<void (Settings & sender, const std::string &), Settings &, std::string> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtLanguage::configChangedEventHandlerThreadSafe, this, _1, _2), sender, key);
	postEvent(event);
}

void QtLanguage::configChangedEventHandlerThreadSafe(Settings & sender, const std::string & key) {
	if (key == Config::LANGUAGE_KEY) {
		loadLanguageFromConfig();
	}
}

QString QtLanguage::getLocaleFileName() {
	QString localeName;
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	if (config.getLanguage() == Config::LANGUAGE_AUTODETECT_KEYVALUE) {
		localeName = QLocale::system().name();
		if (localeName == "C") {
			// No locale defined, just get out
			return QString::null;
		}
	} else {
		localeName = QString::fromStdString(config.getLanguage());
	}
	return localeName;
}

QString QtLanguage::getQtLocaleFileName() {
	QString localeName = QtLanguage::getLocaleFileName();
	if (!localeName.isEmpty()) {
		return "qt_" + localeName;
	} else {
		return QString::null;
	}
}

QString QtLanguage::getWengoPhoneLocaleFileName() {
	QString localeName = QtLanguage::getLocaleFileName();
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QString langDir = QString::fromStdString(config.getResourcesDir()) + LANG_DIR;
	if (!localeName.isEmpty()) {
		return langDir + QDir::separator() + "qtwengophone_" + localeName;
	} else {
		return QString::null;
	}
}

void QtLanguage::loadLanguageFromConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QString qtTranslationName = getQtLocaleFileName();

	QString langDir = QString::fromStdString(config.getResourcesDir()) + LANG_DIR;
	bool qtTranslationLoaded = _qtTranslator->load(qtTranslationName, langDir);

#ifdef QT_TRANSLATION_DIR
	if (!qtTranslationLoaded) {
		qtTranslationLoaded = _qtTranslator->load(qtTranslationName, QT_TRANSLATION_DIR);
	}
#endif
	if (!qtTranslationLoaded) {
		LOG_INFO("no Qt translation available for locale '" + getLocaleFileName().toStdString() + "'");
	}

	LOG_DEBUG(getWengoPhoneLocaleFileName().toStdString());
	if (!_appTranslator->load(getWengoPhoneLocaleFileName())) {
		LOG_INFO("no application translation available for locale '" + getLocaleFileName().toStdString() + "'");
	}
}

static QString getLocaleFromFileName(const QString & fileName) {
	// qtwengophone_en.qm -> en
	QRegExp rx("qtwengophone_([^.]+)");
	rx.indexIn(fileName);
	QStringList list = rx.capturedTexts();
	if (list.size() == 2) {
		return list[1];
	} else {
		return QString();
	}
}

QStringList QtLanguage::getAvailableLanguages() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	QStringList languageList;
	QDir dir(QString::fromStdString(config.getResourcesDir()) + LANG_DIR);

	Q_FOREACH(QFileInfo fileInfo, dir.entryInfoList()) {

		if (fileInfo.isFile() &&
			fileInfo.size() &&
			(fileInfo.completeSuffix().toStdString() == LANGUAGE_FILE_EXTENSION)) {

			QString localeName = getLocaleFromFileName(fileInfo.fileName());
			if (localeName.isEmpty()) {
				// This can happen if the translation dir contains other files
				// (like Qt translations)
				continue;
			}
			std::string language = LanguageList::getLanguageName(localeName.toStdString());
			languageList += QString::fromUtf8(language.c_str());
		}
	}

	return languageList;
}
