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

#include <List.h>

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
	 * Converts this String to std::string.
	 *
	 * @return the converted String to std::string
	 */
	operator const std::string&() {
		return *this;
	}

	/**
	 * Converts this string to an int.
	 *
	 * @return the string converted to an int or 0 if failed to convert
	 */
	int toInteger() const;

	/**
	 * Converts this string to a boolean.
	 *
	 * Detects strings: "true", "TRUE", "yes", "YES", "1"
	 *                  "false", "FALSE", "no", "NO", "0"
	 * Be very carefull when using this method, maybe it should throw an Exception.
	 *
	 * @return the string converted to a boolean (return false if failed to convert)
	 */
	bool toBoolean() const;

	/**
	 * Converts all of the characters in this string to lower case.
	 *
	 * Example:
	 * <pre>
	 * String myString("WengO");
	 * str = myString.toLowerCase();	//str == "wengo"
	 * </pre>
	 *
	 * @return the string converted to lowercase
	 */
	std::string toLowerCase() const;

	/**
	 * Converts all of the characters in this string to upper case.
	 *
	 * Example:
	 * <pre>
	 * String myString("WengO");
	 * str = myString.toUpperCase();	//str == "WENGO"
	 * </pre>
	 *
	 * @return the string converted to uppercase
	 */
	std::string toUpperCase() const;

	/**
	 * @param str the string to test
	 * @return true if String begins with str
	 */
	bool beginsWith(const String & str) const;

	/**
	 * Gets the number of occurences of the string str inside this string.
	 *
	 * @param str string to find
	 * @param caseSensitive the search is case sensitive; otherwise the search is case insensitive
	 * @return true if this list contains the specified element
	 */
	bool contains(const std::string & str, bool caseSensitive = true) const;

	/**
	 * Replaces every occurence of the string before with the string after.
	 *
	 * @param before occurence to find
	 * @param after the string that will replace the string before
	 * @param caseSensitive the search is case sensitive; otherwise the search is case insensitive
	 */
	void replace(const std::string & before, const std::string & after, bool caseSensitive = true);

	/**
	 * Appends a string onto the end of this string.
	 *
	 * @param str string to append
	 * @return the new string
	 */
	std::string & append(const std::string & str);

	/**
	 * Removes every occurrence of str in the string.
	 *
	 * @param str to remove in the string
	 */
	void remove(const std::string & str);

	/**
	 * Gets a string from a number.
	 *
	 * @param number number to convert into a string
	 * @return number converted to a string
	 */
	static std::string fromNumber(int number);

	/**
	 * URL-encodes a string.
	 *
	 * @param str the string to encode
	 * @return a string with all non-alphanumeric characters replaced by their
	 *         URL-encoded equivalent.
	 */
	static std::string encodeUrl(const std::string & str);

	/**
	 * URL-decodes a string.
	 *
	 * @param str the URL-encoded string to decode
	 * @return a string with all URL-encoded sequences replaced by their
	 *         ASCII equivalent
	 */
	static std::string decodeUrl(const std::string & str);
};


/**
 * std::list<std::string> wrapper/helper.
 *
 * Inspired from the class QStringList from the Qt library.
 *
 * For converting a StringList object to a QStringList,
 * check libqtutil.
 *
 * @see QStringList
 * @see std::list
 * @author Tanguy Krotoff
 */
class StringList : public List<std::string> {
public:

	/**
	 * Converts this StringList to std::list<std::string>.
	 *
	 * @return the converted StringList to std::list<std::string>
	 */
	operator std::list<std::string>();

	  /**
	   * Convert a std::list<std::string> to a StringList using a
	   * copy-constructor.
	   * @return a new StringList.
	   */
	  StringList(const std::list<std::string>&);
	  StringList() {}
	/**
	 * Permits to use StringList as an array.
	 *
	 * @see List::operator[]
	 * @param i index inside the array
	 * @return the std::string that corresponds to the index i inside the StringList
	 *         or String::null if index i is bigger than length()
	 */
	std::string operator[](unsigned i) const;

	/**
	 * Gets the number of occurrences of a string contained inside the StringList.
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
	 * @return number of occurences of the value str inside the StringList (0 if no occurence)
	 */
	unsigned contains(const std::string & str, bool caseSensitive = true) const;

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

	enum SortingOrder {
		Ascendant,
		Descendant
	};

	/**
	 * Sort the string list alphabetically
	 *
	 * @param order sorting order
	 */
	void sort(SortingOrder order = Ascendant);

private:

	class StringCompareDescendant {
	public:
		bool operator()(const String & s1, const String & s2) {
			return s2 < s1;
		}
	};
};

#endif	//STRINGLIST_H
