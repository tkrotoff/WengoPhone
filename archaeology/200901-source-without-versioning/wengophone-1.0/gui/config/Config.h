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

#ifndef CONFIG_H
#define CONFIG_H

#include <singleton/Singleton.h>
#include <observer/Subject.h>

#include <qobject.h>

class CodecList;
class QString;
class QPalette;

/**
 * Class that saves in memory the settings/configuration of the user.
 *
 * Design Pattern Singleton.
 * For saving the configuration of the user on the hard disk, we use
 * this class + Database. Does not use QSettings since it seems to be
 * buggy.
 *
 * @author Tanguy Krotoff
 */
class Config : public Subject, public Singleton<Config> {
	friend class Singleton<Config>;
public:

	~Config();

	/**
	 * DocType.
	 */
	static const QString DOC_TYPE;

	/**
	 * Tags.
	 */
	static const QString STYLE_TAG;

	static const QString LANGUAGE_TAG;

	/**
	 * Saves the configuration/settings of the application.
	 */
	void saveAsXml();

	/**
	 * Sets the style for the application.
	 *
	 * @param style style/theme
	 */
	void setStyle(const QString & style);

	/**
	 * Gets the current style.
	 *
	 * @return current style/theme
	 */
	QString getStyle() const {
		return _style;
	}

	/**
	 * Sets the current language.
	 *
	 * @param language current language/translation
	 */
	void setLanguage(const QString & language);

	/**
	 * Gets the current language.
	 *
	 * @return current language/translation
	 */
	QString getLanguage() const {
		return _language;
	}

	/**
	 * Gets the {audio,video}CodecList object.
	 *
	 * @return CodecList object
	 */
	CodecList & getAudioCodecList() const {
		return *_audioCodecList;
	}

	CodecList & getVideoCodecList() const {
		return *_videoCodecList;
	}
	

	/**
	 * Return the language code like "en", "fr", ...
	 * 
	 * @return the language code 
	 */
	QString getLanguageCode();
	
private:

	Config();
	Config(const Config &);
	Config & operator=(const Config &);

	/**
	 * Style/theme of the application.
	 */
	QString _style;

	/**
	 * Language/translation of the application.
	 */
	QString _language;

	/**
	 * CodecList.
	 */
	CodecList * _audioCodecList;
	CodecList * _videoCodecList;
};

#endif	//CONFIG_H
