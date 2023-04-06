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

#ifndef STRINGLIST_H
#define STRINGLIST_H

#include <string>
#include <list>

/**
 * std::string wrapper/helper.
 *
 * Inspired from the class QString from the Qt library.
 *
 * @see QString
 * @see std::string
 * @see java.lang.String
 * @author Tanguy Krotoff
 */
class String : public std::string {
public:

	/**
	 * Null string.
	 *
	 * Rather than to code something like this:
	 * if (myString == "")
	 * it's better to write:
	 * if (myString.empty())
	 * return "" -> return String::null
	 */
	static const String null;


	String() : std::string() { }

	String(const char * str) : std::string(str) { }

	String(const std::string & str) : std::string(str) { }

	/**
	 * Converts a String to std::string.
	 *
	 * @return the converted String to std::string
	 */
	operator const std::string&() {
		return *this;
	}

	/**
	 * Converts this String to an int.
	 *
	 * @return the String converted to an int or 0 if failed to convert
	 */
	int toInteger() const;

	/**
	 * Converts all of the characters in this String to lower case.
	 *
	 * Example:
	 * <pre>
	 * String myString("WengO");
	 * str = myString.toLowerCase();	//str == "wengo"
	 * </pre>
	 *
	 * @return the String converted to lowercase
	 */
	String toLowerCase() const;

	/**
	 * Converts all of the characters in this String to upper case.
	 *
	 * Example:
	 * <pre>
	 * String myString("WengO");
	 * str = myString.toUpperCase();	//str == "WENGO"
	 * </pre>
	 *
	 * @return the String converted to uppercase
	 */
	String toUpperCase() const;

	/**
	 * Replaces every occurence of the String before with the String after.
	 *
	 * @param before occurence to find
	 * @param after the String that will replace the String before
	 * @param caseSensitive the search is case sensitive; otherwise the search is case insensitive
	 */
	void replace(const std::string & before, const std::string & after, bool caseSensitive = true);

	/**
	 * Removes every occurrence of str in the String.
	 *
	 * @param str to remove in the String
	 */
	void remove(const std::string & str);

	/**
	 * Gets a String from a number.
	 *
	 * @param number number to convert into a String
	 * @return number converted to a String
	 */
	static std::string fromNumber(int number);
};


/**
 * std::list<std::string> wrapper/helper.
 *
 * Inspired from the class QStringList from the Qt library.
 *
 * How to convert from StringList to QStringList:
 * <pre>
 * StringList myList;
 * QStringList tmp;
 * for (StringList::const_iterator it = myList.begin();
 *          it != myList.end(); ++it) {
 *     tmp += *it;
 * }
 * </pre>
 *
 * @see QStringList
 * @see std::list
 * @author Tanguy Krotoff
 */
class StringList : public std::list<std::string> {
public:

	/**
	 * Permits to use StringList as an array.
	 *
	 * <pre>
	 * for (unsigned int i = 0; i  stringList.size(); i++) {
	 *     String tmp(stringList[i]);
	 *     if (!tmp.isNull()) {
	 *          std::cout << tmp << std::endl;
	 *     }
	 * }
	 * </pre>
	 *
	 * Warning: slow operation.
	 *
	 * @param i index inside the array
	 * @return the std::string that corresponds to the index i inside the StringList
	 *         or String::null if index i is bigger than length()
	 */
	String operator[](unsigned int i) const;

	/**
	 * Appends a String to the StringList.
	 *
	 * Example:
	 * <pre>
	 * StringList strList;
	 * strList += "wengo";	//strList[0]
	 * strList += "is";	//strList[1]
	 * strList += "good";	//strList[2]
	 * </pre>
	 *
	 * @param str the String to append to the StringList
	 */
	void operator+=(const std::string & str);

	/**
	 * Gets the number of occurrences of a String contained inside the StringList.
	 *
	 * Example:
	 * <pre>
	 * StringList strList;
	 * strList += "wengo";
	 * strList += "is";
	 * strList += "good";
	 * strList += "wengo";
	 * unsigned int i = strList.contains("wengo");	//i = 2
	 * unsigned int j = strList.contains("Wengo");	//j = 0
	 * unsigned int k = strList.contains("Wengo", false);	//k = 2
	 * </pre>
	 *
	 * @param str value to look up inside the StringList
	 * @param caseSensitive the search is case sensitive; otherwise the search is case insensitive
	 * @return number of occurences of the value str inside the StringList
	 *         (0 if no occurence)
	 */
	unsigned int contains(const std::string & str, bool caseSensitive = true) const;

	/**
	 * Tokenizes a string with a delimiter of your choice.
	 *
	 * Example:
	 * <pre>
	 * StringList tokens = StringList::split("four", "");	//tokens = "four"
	 * tokens = StringList::split("four roses");	//tokens = "four", "roses"
	 * </pre>
	 *
	 * @param str string to tokenize/split
	 * @param separator string delimiter (default is a whitespace)
	 * @return tokens, strings created by splitting the input string
	 */
	static StringList split(const std::string & str, const std::string & separator = " ");
};

#endif	//STRINGLIST_H
