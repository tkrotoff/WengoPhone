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

#include "LanguageList.h"

#include "Languages.h"

#include <boost/regex.hpp>

std::string LanguageList::getLanguageName(const std::string & iso639Code) {
	return getResultRegex(iso639Code, ".*\\t(.*)\\t");
}

std::string LanguageList::getEnglishLanguageName(const std::string & iso639Code) {
	return getResultRegex(iso639Code, ".*\\t.*\\t(.*)$");
}

std::string LanguageList::getResultRegex(const std::string & iso639Code, const std::string & regex) {
	static const char * pointer;
	static int length = sizeof(LANGUAGES) / sizeof(pointer);

	for (int i = 0; i < length; i++) {
		std::string line(LANGUAGES[i]);

		//en... -> en
		boost::regex e("^" + iso639Code + "\\t");
		if (boost::regex_match(line, e)) {
			//en	English... -> English

			boost::regex eLangName(regex);
			boost::match_results<std::string::const_iterator> what;
			boost::regex_search(line, what, eLangName);
			std::string tmp(what[1].first, what[1].second);
			return tmp;
		}
	}
	return String::null;
}
