/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "LanguageWidget.h"

#include "MyWidgetFactory.h"
#include "Softphone.h"
#include "Config.h"
#include "database/Database.h"
using namespace database;

#include <qlabel.h>
#include <qlistbox.h>
#include <qstylefactory.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qvaluelist.h>
#include <qtranslator.h>

LanguageWidget::LanguageWidget(QWidget * parent) : QObject(parent) {
	_languageWidget = MyWidgetFactory::create("LanguageWidgetForm.ui", this, parent);

	QListBox * languageListBox = (QListBox *) _languageWidget->child("languageListBox", "QListBox");
	languageListBox->insertStringList(getLangNameList());

	connect(languageListBox, SIGNAL(selectionChanged(QListBoxItem *)),
		this, SLOT(selectionChanged(QListBoxItem *)));
}

LanguageWidget::~LanguageWidget() {
	delete _languageWidget;
}

void LanguageWidget::selectionChanged(QListBoxItem * itemSelected) {
	//itemSelected->text() = "English"
	//_mapFileLanguage[itemSelected->text()] = "wengo_en.qm"
	Config::getInstance().setLanguage(_mapFileLanguage[itemSelected->text()]);
}

QStringList LanguageWidget::getLangNameList() {
	QStringList langList;

	QStringList tmp(getLangFileList());
	for (QStringList::Iterator it = tmp.begin(); it != tmp.end(); ++it) {
		QString fileName(*it);

		//wengo_en.qm -> en
		QRegExp rx("_([^\\.]*)");
		rx.search(fileName);
		QStringList list = rx.capturedTexts();

		//en -> English
		//fr -> Francais
		QString langName(_languageList.getLanguageName(list[1]));
		if (!langName.isNull()) {
			langList += langName;
			_mapFileLanguage[langName.ascii()] = fileName.ascii();
		}
	}

	return langList;
}

QStringList LanguageWidget::getLangFileList() {
	QStringList languageList;
	QDir dir(Softphone::getInstance().getLanguagesDir());
	const QFileInfoList * list = dir.entryInfoList();

	if (list == NULL) {
		//The directory is unreadable or does not exist
		return languageList;
	}

	QFileInfoListIterator it(*list);
	QFileInfo * info;

	while ((info = it.current()) != 0) {
		if (info->isFile() &&
			info->size() &&
			(info->extension() == Softphone::LANGUAGE_FILE_EXTENSION)) {

			//Adds the language to the list
			languageList += info->fileName();
		}

		++it;
	}

	return languageList;
}
