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

#ifndef OWQTLANGUAGE_H
#define OWQTLANGUAGE_H

#include <qtutil/QObjectThreadSafe.h>

#include <util/Trackable.h>

class Settings;

class QString;
class QStringList;

/**
 * Changes the language of the application.
 *
 * @see QTranslator
 * @author Tanguy Krotoff
 */
class QtLanguage : public QObjectThreadSafe, public Trackable {
	Q_OBJECT
public:

	QtLanguage(QObject * parent);

	~QtLanguage();

	static QStringList getAvailableLanguages();

private:

	void initThreadSafe() { }

	void configChangedEventHandler(Settings & sender, const std::string & key);

	void configChangedEventHandlerThreadSafe(Settings & sender, const std::string & key);

	static std::string getISO639CodeFromFileName(const QString & fileName);

	static QString getFileNameFromISO639Code(const std::string & iso639Code);
};

#endif	//OWQTLANGUAGE_H
