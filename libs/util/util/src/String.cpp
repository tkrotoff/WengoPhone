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

#include <util/String.h>

#include <util/StringList.h>
#include <util/Logger.h>

#include <cctype>
#include <algorithm>
#include <sstream>
using namespace std;

const char * String::null = "";
const std::string String::EOL = "\n";

int String::toInteger() const {
	int tmp;
	stringstream ss(c_str());
	ss >> tmp;
	return tmp;
}

bool String::toBoolean() const {
	String tmp(c_str());
	tmp = tmp.toLowerCase();
	if (tmp == "true" || tmp == "yes" || tmp == "1") {
		return true;
	}

	return false;
}

std::string String::toUpperCase() const {
	string tmp(c_str());
	transform(tmp.begin(), tmp.end(), tmp.begin(), (int(*)(int)) toupper);
	return tmp;
}

std::string String::toLowerCase() const {
	string tmp(c_str());
	transform(tmp.begin(), tmp.end(), tmp.begin(), (int(*)(int)) tolower);
	return tmp;
}

bool String::beginsWith(const String & str) const {
	return (this->find(str) == 0);
}

bool String::contains(const std::string & str, bool caseSensitive) const {
	string tmp(c_str());
	String str2(str);

	if (!caseSensitive) {
		//Converts tmp + str2 to lower case
		tmp = toLowerCase();
		str2 = str2.toLowerCase();
	}

	if (tmp.find(str2, 0) != string::npos) {
		return true;
	}
	return false;
}

bool String::contains(char ch, bool caseSensitive) const {
	std::string str;
	str += ch;
	return contains(str, caseSensitive);
}

void String::replace(const std::string & before, const std::string & after, bool caseSensitive) {
	//Copy this + before to tmp + before2
	string tmp(c_str());
	String before2(before);

	if (!caseSensitive) {
		//Converts tmp + before2 to lower case
		tmp = toLowerCase();
		before2 = before2.toLowerCase();
	}

	//Searches on tmp + before2 rather than this + before
	string::size_type pos = 0;
	while ((pos = tmp.find(before2, pos)) != string::npos) {
		//Replaces on this + tmp
		string::replace(pos, before2.length(), after);
		tmp.replace(pos, before2.length(), after);
		pos = pos + after.length();
	}
}

std::string & String::append(const std::string & str) {
	return insert(size(), str);
}

void String::remove(const std::string & str) {
	replace(str, String::null);
}

std::string String::fromNumber(int number) {
	stringstream ss;
	ss << number;
	return ss.str();
}

std::string String::fromBoolean(bool boolean) {
	stringstream ss;
	ss << boolean;
	return ss.str();
}

std::string String::fromLong(long number) {
	stringstream ss;
	ss << number;
	return ss.str();
}

std::string String::fromLongLong(long long number) {
	stringstream ss;
	ss << number;
	return ss.str();
}

std::string String::fromUnsignedLongLong(unsigned long long number) {
	stringstream ss;
	ss << number;
	return ss.str();
}

static unsigned char hex_to_int(unsigned char ch) {
	if (ch >= 'A' && ch <= 'F') {
		return ch - 'A' + 10;
	}
	if (ch >= 'a' && ch <= 'f') {
		return ch - 'a' + 10;
	}
	if (ch >= '0' && ch <= '9') {
		return ch - '0';
	}
	return 0;
}

/* Taken from Qt3 QUrl::decode() */
std::string String::decodeUrl(const std::string & url) {
	string newUrl;

	if (url.empty()) {
		return newUrl;
	}

	int oldlen = url.length();
	int i = 0;
	while (i < oldlen) {
		unsigned char ch = url[i++];
		if (ch == '%' && i <= oldlen - 2) {
			ch = hex_to_int(url[i]) * 16 + hex_to_int(url[i + 1]);
			i += 2;
		}
		newUrl += ch;
	}

	return newUrl;
}

/* Taken from Qt3 QUrl::encode() */
std::string String::encodeUrl(const std::string & url) {
	string newUrl;

	if (url.empty()) {
		return newUrl;
	}

	static const String special("+<>#@\"&%$:,;?={}|^~[]\'`\\ \n\t\r");

	int oldlen = url.length();
	for (int i = 0; i < oldlen; ++i) {
		unsigned char inCh = url[i];

		if (inCh >= 128 || special.contains(inCh)) {
			newUrl += '%';

			unsigned short c = inCh / 16;
			c += c > 9 ? 'A' - 10 : '0';
			newUrl += c;

			c = inCh % 16;
			c += c > 9 ? 'A' - 10 : '0';
			newUrl += c;
		} else {
			newUrl += inCh;
		}
	}

	return newUrl;
}

StringList String::split(const std::string & separator) const {
	string str(c_str());

	//Skip separator at beginning.
	string::size_type lastPos = str.find_first_not_of(separator, 0);

	//Find first "non-separator".
	string::size_type pos = str.find_first_of(separator, lastPos);

	StringList tokens;
	while (string::npos != pos || string::npos != lastPos) {

		//Found a token, add it to the vector.
		tokens += str.substr(lastPos, pos - lastPos);

		//Skip delimiters. Note the "not_of"
		lastPos = str.find_first_not_of(separator, pos);

		//Find next "non-delimiter"
		pos = str.find_first_of(separator, lastPos);
	}
	return tokens;
}

std::string String::trim() {
	string newstr;

	string::size_type pos1 = find_first_not_of(" \t");
	string::size_type pos2 = find_last_not_of(" \t");
	newstr = substr(pos1 == string::npos ? 0 : pos1,
		pos2 == string::npos ? length() - 1 : pos2 - pos1 + 1);

	return newstr;
}
