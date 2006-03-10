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

#ifndef LANGUAGELIST_H
#define LANGUAGELIST_H

#include <NonCopyable.h>
#include <StringList.h>

/**
 * Parses Languages.h
 *
 * @see LANGUAGES
 * @author Tanguy Krotoff
 */
class LanguageList : NonCopyable {
public:

	/**
	 * Gets the language name given its ISO 639 Alpha-2 code.
	 *
	 * @param iso639Code ISO 639 Alpha-2 code (e.g en, fr ect...)
	 * @return language name (e.g français, english...) or empty string
	 */
	static std::string getLanguageName(const std::string & iso639Code);

	/**
	 * Gets the english language name given its ISO 639 Alpha-2 code.
	 *
	 * @param iso639Code ISO 639 Alpha-2 code (e.g en, fr ect...)
	 * @return english language name (e.g french, english...) or empty string
	 */
	static std::string getEnglishLanguageName(const std::string & iso639Code);

private:

	static std::string getResultRegex(const std::string & iso639Code, const std::string & regex);
};

#endif	//LANGUAGELIST_H
