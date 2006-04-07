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

#include <util/Logger.h>

#include <qtutil/Object.h>
#include <qtutil/WidgetFactory.h>

#include <QtGui>

static const int KEY_NAME_COLUMN = 0;
static const int STATUS_COLUMN = 1;
static const int TYPE_COLUMN = 2;
static const int VALUE_COLUMN = 3;
static const int DEFAULT_VALUE_COLUMN = 4;
static const int SAVE_KEY_COLUMN = 5;

static const QString TYPE_STRING = "string";
static const QString TYPE_STRINGLIST = "stringlist";
static const QString TYPE_BOOLEAN = "boolean";
static const QString TYPE_INTEGER = "integer";

QtAdvancedConfig::QtAdvancedConfig(QWidget * parent) : QObject() {
	_advancedConfigWindow = WidgetFactory::create(":/forms/config/AdvancedConfigWindow.ui", parent);

	_tableWidget = Object::findChild<QTableWidget *>(_advancedConfigWindow, "tableWidget");

	QPushButton * saveButton = Object::findChild<QPushButton *>(_advancedConfigWindow, "saveButton");
	connect(saveButton, SIGNAL(clicked()), SLOT(saveConfig()));
}

void QtAdvancedConfig::populate() {
	//_tableWidget->clear();

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	StringList keys = config.getAllKeys();
	for (unsigned i = 0; i < keys.size(); i++) {
		if (i >= _tableWidget->rowCount()) {
			_tableWidget->insertRow(i);
		}

		std::string key = keys[i];
		QTableWidgetItem * itemKey = new QTableWidgetItem(QString::fromStdString(key));
		_tableWidget->setItem(i, KEY_NAME_COLUMN, itemKey);

		boost::any value = config.getAny(key);
		if (!value.empty()) {
			setItem(value, config.keyValueToBeSaved(key), i, VALUE_COLUMN);
		}

		boost::any defaultValue = config.getDefaultValue(key);
		if (!defaultValue.empty()) {
			setItem(defaultValue, config.keyValueToBeSaved(key), i, DEFAULT_VALUE_COLUMN);
		}

	}
}

void QtAdvancedConfig::setItem(boost::any value, bool saveKeyValue, int row, int column) {
	QTableWidgetItem * itemValue = NULL;
	QTableWidgetItem * itemType = NULL;
	if (Settings::isBoolean(value)) {
		itemType = new QTableWidgetItem(TYPE_BOOLEAN);
		bool tmp = boost::any_cast<bool>(value);
		if (tmp) {
			itemValue = new QTableWidgetItem("true");
		} else {
			itemValue = new QTableWidgetItem("false");
		}
	} else if (Settings::isInteger(value)) {
		itemType = new QTableWidgetItem(TYPE_INTEGER);
		int tmp = boost::any_cast<int>(value);
		itemValue = new QTableWidgetItem(QString::fromStdString(String::fromNumber(tmp)));
	} else if (Settings::isString(value)) {
		itemType = new QTableWidgetItem(TYPE_STRING);
		std::string tmp = boost::any_cast<std::string>(value);
		itemValue = new QTableWidgetItem(QString::fromStdString(tmp));
	} else if (Settings::isStringList(value)) {
		itemType = new QTableWidgetItem(TYPE_STRINGLIST);
		StringList tmp = boost::any_cast<StringList>(value);
		//itemValue = new QTableWidgetItem(QString::fromStdString(tmp));
	} else {
		LOG_FATAL("unknown type");
	}

	_tableWidget->setItem(row, column, itemValue);
	_tableWidget->setItem(row, TYPE_COLUMN, itemType);

	QTableWidgetItem * itemSaveKeyValue = NULL;
	if (saveKeyValue) {
		itemSaveKeyValue = new QTableWidgetItem("true");
	} else {
		itemSaveKeyValue = new QTableWidgetItem("false");
	}
	_tableWidget->setItem(row, SAVE_KEY_COLUMN, itemSaveKeyValue);
}

void QtAdvancedConfig::saveConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	for (int row = 0; row < _tableWidget->rowCount(); row++) {

		QTableWidgetItem * itemKey = _tableWidget->item(row, KEY_NAME_COLUMN);
		if (!itemKey) {
			return;
		}
		std::string key = itemKey->text().toStdString();

		QTableWidgetItem * itemValue = _tableWidget->item(row, VALUE_COLUMN);
		if (!itemValue) {
			return;
		}

		QTableWidgetItem * itemType = _tableWidget->item(row, TYPE_COLUMN);
		if (!itemType) {
			return;
		}

		if (itemType->text() == TYPE_BOOLEAN) {
			String tmp = itemValue->text().toStdString();
			config.set(key, tmp.toBoolean());
		} else if (itemType->text() == TYPE_INTEGER) {
			String tmp = itemValue->text().toStdString();
			config.set(key, tmp.toInteger());
		} else if (itemType->text() == TYPE_STRING) {
			std::string tmp = itemValue->text().toStdString();
			config.set(key, tmp);
		} else if (itemType->text() == TYPE_STRINGLIST) {
			//std::string tmp = itemValue->text().toStdString();
			//config.set(key, tmp);
		} else {
			LOG_FATAL("unknown type");
		}
	}
}
