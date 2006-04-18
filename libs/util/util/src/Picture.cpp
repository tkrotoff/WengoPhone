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

#include <util/Picture.h>

#include <util/File.h>

using namespace std;

Picture::Picture() {
}

Picture::Picture(const Picture & picture)
: _pictureData(picture._pictureData) {
}

Picture Picture::pictureFromData(const std::string & data) 
{
	Picture result;

	result._pictureData = data;

	return result;
}

Picture Picture::pictureFromFile(const std::string & filename) 
{
	string data;
	FileReader file(filename);
	if (file.open()) {
		data = file.read();
		file.close();
	}

	Picture result = pictureFromData(data);
	result.setFilename(filename);

	return result;
}

string Picture::getData() const {
	return _pictureData;
}

string Picture::getFilename() const {
	return _filename;
}

void Picture::setFilename(const string & filename) {
	string path = filename;
	path = File::convertPathSeparators(path);
	string::size_type pos = path.rfind(File::getPathSeparator());

	_filename = path.substr(pos + 1);
}
