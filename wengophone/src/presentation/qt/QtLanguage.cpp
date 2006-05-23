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

#include "QtLanguage.h"

#include <model/config/LanguageList.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/Logger.h>

#include <QtCore>

static const char * LANGUAGE_FILE_EXTENSION = "qm";
static const char * LANG_DIR = "lang";

QtLanguage::QtLanguage(QObject * parent)
	: QObjectThreadSafe(parent) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.valueChangedEvent += boost::bind(&QtLanguage::configChangedEventHandler, this, _1, _2);

	configChangedEventHandler(config, Config::LANGUAGE_KEY);
}

void QtLanguage::updateTranslation() {
  Config & config = ConfigManager::getInstance().getCurrentConfig();
  configChangedEventHandler(config, Config::LANGUAGE_KEY);
}

QtLanguage::~QtLanguage() {
}

void QtLanguage::configChangedEventHandler(Settings & sender, const std::string & key) {
	typedef PostEvent2<void (Settings & sender, const std::string &), Settings &, std::string> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtLanguage::configChangedEventHandlerThreadSafe, this, _1, _2), sender, key);
	postEvent(event);
}

void QtLanguage::configChangedEventHandlerThreadSafe(Settings & sender, const std::string & key) {
	static QTranslator * translator = new QTranslator(QCoreApplication::instance());

	if (key == Config::LANGUAGE_KEY) {
		std::string iso639Code;
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		if (config.getLanguage() == Config::LANGUAGE_AUTODETECT_KEYVALUE) {
			//QLocale::system().name() = language_country
			//we want locale = language
			QStringList locale = QLocale::system().name().split("_");
			iso639Code = locale[0].toLower().toStdString();
			config.set(Config::LANGUAGE_KEY, iso639Code);
		} else {
			iso639Code = config.getLanguage();
		}
		QString fileName = getFileNameFromISO639Code(iso639Code);
		fileName = QString::fromStdString(config.getResourcesDir()) + LANG_DIR + QDir::separator() + fileName;
		translator->load(fileName);
		QCoreApplication::installTranslator(translator);
		LOG_DEBUG("language changed=" + fileName.toStdString());
		translationChangedSignal();
	}
}

std::string QtLanguage::getISO639CodeFromFileName(const QString & fileName) {
	std::string iso639Code;

	//wengophone_en.qm -> en
	QRegExp rx("_([^\\.]*)");
	rx.indexIn(fileName);
	QStringList list = rx.capturedTexts();

	QString langName(list[1]);
	if (!langName.isNull()) {
		iso639Code = langName.toStdString();
	}
	return iso639Code;
}

QString QtLanguage::getFileNameFromISO639Code(const std::string & iso639Code) {
	return "qtwengophone_" + QString::fromStdString(iso639Code) + "." + LANGUAGE_FILE_EXTENSION;
}

QStringList QtLanguage::getAvailableLanguages() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	QStringList languageList;
	QDir dir(QString::fromStdString(config.getResourcesDir()) + LANG_DIR);
	QFileInfoList list = dir.entryInfoList();

	if (!dir.exists() || list.isEmpty()) {
		//The directory is unreadable or does not exist
		return languageList;
	}

	for (int i = 0; i < list.size(); i++) {
		QFileInfo fileInfo = list.at(i);

		if (fileInfo.isFile() &&
			fileInfo.size() &&
			(fileInfo.completeSuffix().toStdString() == LANGUAGE_FILE_EXTENSION)) {

			QString fileName = fileInfo.fileName();
			std::string iso639Code = getISO639CodeFromFileName(fileName);

			//Adds the language to the list
			languageList += QString::fromUtf8(LanguageList::getLanguageName(iso639Code).c_str());
		}
	}

	return languageList;
}
