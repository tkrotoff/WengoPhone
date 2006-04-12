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

#include <vcard/VCardParser.h>

#include <util/Logger.h>

using namespace std;

VCardParser::VCardParser(const string & filename) 
: _filename(filename) {
}

VCardParser::~VCardParser() {
}

bool VCardParser::parse() {
	_fileStream.open(_filename);
	if (!_fileStream) {
		LOG_ERROR("cannot open the file " + _filename);
		return false;
	}

	string lastLine = getLastLine();
	while (!lastLine.empty()) {
		string tag = getTag();

		tagFoundEvent(tag, lastLine);

		lastLine = getLastLine();
	}

	_fileStream.close();

	return true;
}

vector<string> VCardParser::getType(const string & tagLine) {

}

vector<string> VCardParser::getValue(const string & tagLine) {

}

string VCardParser::getLastTagLine() {
	string result;
	stringstream strStream;

	// Get a line
	_fileStream.get(strStream);
	result = strStream.str();
	////

	// Looking if the next lines are folded lines
	pos_type lastPos = _fileStream.tellg();
	_fileStream.get(strStream);

	while (beginWithLinearWhiteSpace(strStream.str()) {
		result += stripLinearWhiteSpace(strStream.str());

		lastPos = _fileStream.tellg();
		_fileStream.get(strStream);
	}

	_fileStream.seekg(lastPos);
	////

	return result;
}

string VCardParser::getTag(const string & tagLine) {

}

bool VCardParser::beginWithLinearWhiteSpace(const std::string & line) {
	
}

string VCardParser::stripLinearWhiteSpace(const string & line) {

}
