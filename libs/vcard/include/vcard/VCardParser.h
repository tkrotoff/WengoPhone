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

#ifndef VCARDPARSER_H
#define VCARDPARSER_H

#include <util/Event.h>

#include <string>
#include <vector>
#include <ifstream>

/**
 * Parse a VCard.
 *
 * Usage:
 *    VCardParser parser(file);
 *    parser.tagFoundEvent += boost::bind(&MyClass::tagFoundEventHandler, this,
 *        _1, _2);
 *    parser.parse;
 *
 * @author Philippe BERNERY
 */
class VCardParser {
public:

	/**
	 * Emitted when a tag has been found.
	 *
	 * @param tag the found tag
	 * @param tagLine the entire line where the tag has been. This line can then
	 * be processed with static methods contained in this class.
	 */
	Event< void (VCardParser & sender, const std::string & tag,
		const std::string & tagLine) > tagFoundEvent;

	/**
	 * Creates a VCardParser with a path to the file to parse.
	 *
	 * @param filename the file to parse
	 */
	VCardParser(const std::string & filename);

	~VCardParser();

	/**
	 * Launches the file parsing.
	 *
	 * Each time a new tag is found the event "tagFoundEvent" is emitted.
	 *
	 * @return false if an error occured while parsing the file.
	 */
	bool parse();

	/**
	 * Returns the type of a tag (e.g.: 'TEL;TYPE=home;TYPE=cell' will return
	 * a vector with ('home', 'cell').
	 * 
	 * @param tagLine the tag line to analyse
	 * @return a vector of string containing the found types
	 */
	static std::vector<std::string> getType(const std::string & tagLine);

	/**
	 * Returns the value of the tag line.
	 * (e.g: 'ADR:;;0 rue de Kerfontaniou;Lorient;Bretagne;56100;France' will
	 * return a vector with
	 * ('', '', '0 rue de Kerfontaniou', 'Lorient', 'Bretagne', '56100', 'France')
	 *
	 * @param tagLine the tag line to analyse
	 * @return a vector og string containing the found values
	 */
	static std::vector<std::string> getValue(const std::string & tagLine);

private:

	/**
	 * Retrieve the last tag line even if it is folded.
	 */
	std::string getLastTagLine();

	/**
	 * Retrieve the tag.
	 *
	 * @param tagLine the tag line to retrieve the tag from
	 * @return the tag
	 */
	static std::string getTag(const std::string & tagLine);

	/**
	 * Returns true if the given string begins with at least one linear white
	 * space.
	 *
	 * @param line the line to test
	 * @return true if there is at least one LWSP 	 
	 */
	bool beginWithLinearWhiteSpace(const std::string & line);

	/**
	 * Strips all LWSP in a line.
	 *
	 * @param line the line to strip
	 * @return the stripped line
	 */
	std::string stripLinearWhiteSpace(const std::string & line);

	/** The input file stream. */
	std::ifstream _fileStream;

	/** Name of the file to read. */
	std::string _filename;

};

#endif //VCARDPARSER_H
