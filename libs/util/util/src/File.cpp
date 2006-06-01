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

#include <util/File.h>

#include <util/Logger.h>

#include <cutil/global.h>

#include <string>
#include <iostream>
using namespace std;

#include <stdio.h>
#include <stddef.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(OS_WINDOWS)
	#ifndef S_ISDIR
		#define S_ISDIR(x) ((x) & _S_IFDIR)
	#endif
	#ifndef S_ISREG
		#define S_ISREG(x) ((x) & _S_IFREG)
	#endif
#endif

#ifdef OS_WINDOWS
	#include <windows.h>
#endif
#ifdef CC_MSVC
	#include <direct.h>
#endif

File::File(const std::string & filename)
	: _filename(filename) {
}

File::File(const File & file)
	: _filename(file._filename) {
}

std::string File::getExtension() const {
	int posLastElm = _filename.find_last_of(getPathSeparator());

	if (posLastElm == -1 || posLastElm == _filename.length()) {
		return String::null;
	}

	string last = _filename.substr(++posLastElm, _filename.length() - posLastElm);
	int posExt = last.find_last_of('.');

	if (posExt == -1 || posExt == last.length()) {
		return String::null;
	} else {
		return last.substr(++posExt, last.length() - posExt);
	}
}

std::string File::getPath() const {
	String path = _filename;
	path = convertPathSeparators(path);

	string::size_type pos = path.rfind(getPathSeparator());

	if (pos == string::npos || pos == path.length() - 1) {
		return path;
	} else {
		path = path.substr(0, pos);
		return path;
	}
}

std::string File::getFullPath() const {
	return _filename;
}

StringList File::getDirectoryList() const {
	//Same code as File::getFileList()

	StringList dirList;

	DIR * dp = opendir(_filename.c_str());
	if (dp) {
		struct dirent * ep = NULL;
		while ((ep = readdir(dp))) {
			String dir(ep->d_name);

			if (dir == "." || dir == "..") {
				continue;
			}

			struct stat statinfo;
			std::string absPath = _filename + getPathSeparator() + dir;
			if (stat(absPath.c_str(), &statinfo) == 0) {
				if (S_ISDIR(statinfo.st_mode)) {
					//ep->d_name is a directory
					dirList += dir;
				}
			}
		}

		closedir(dp);
	}

	return dirList;
}

StringList File::getFileList() const {
	//Same code as File::getDirectoryList()

	StringList fileList;

	DIR * dp = opendir(_filename.c_str());
	if (dp) {
		struct dirent * ep = NULL;
		while ((ep = readdir(dp))) {
			String file(ep->d_name);

			struct stat statinfo;
			std::string absPath = _filename + file;
			if (stat(absPath.c_str(), &statinfo) == 0) {
				if (S_ISREG(statinfo.st_mode)) {
					//ep->d_name is a file
					fileList += file;
				}
			}
		}
	}

	closedir(dp);

	return fileList;
}

std::string File::convertPathSeparators(const std::string & path) {
	String tmp = path;
	tmp.replace("\\", getPathSeparator());
	tmp.replace("/", getPathSeparator());
	return tmp;
}

std::string File::getPathSeparator() {
#ifdef OS_WINDOWS
	static const std::string PATH_SEPARATOR = "\\";
	return PATH_SEPARATOR;
#else
	static const std::string PATH_SEPARATOR = "/";
	return PATH_SEPARATOR;
#endif
}

void File::createPath(const std::string & path) {
	string::size_type index = path.find(File::getPathSeparator(), 0);
	while (index != string::npos) {
#if defined CC_MSVC || defined CC_MINGW
		mkdir(path.substr(0, index).c_str());
#else
		mkdir(path.substr(0, index).c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
#endif
		index = path.find(File::getPathSeparator(), index + 1);
	}
}

File File::createTemporaryFile() {
	return File(tempnam(NULL, NULL));
}

bool File::exists(const std::string & path) {
	struct stat statinfo;

	if (stat(path.c_str(), &statinfo) == 0) {
		return true;
	} else {
		return false;
	}
}

FileReader::FileReader(const std::string & filename)
	: File(filename) {
}

FileReader::FileReader(const File & file)
	: File(file) {
}

FileReader::FileReader(const FileReader & fileReader)
	: File(fileReader) {
}

FileReader::~FileReader() {
	close();
}

bool FileReader::open() {
	_file.open(_filename.c_str(), ios::binary);
	return isOpen();
}

bool FileReader::isOpen() const {
	return _file.is_open();
}

std::string FileReader::read() {
	static const unsigned int BUFFER_SIZE = 2000;

	if (!isOpen()) {
		LOG_FATAL("you must check the file is open");
	}

	std::string data;
	char tmp[BUFFER_SIZE];
	while (!_file.eof()) {
		_file.read(tmp, BUFFER_SIZE);
		data.append(tmp, _file.gcount());
	}

	return data;
}

void FileReader::close() {
	LOG_DEBUG("file=" + _filename + " loaded");
	_file.close();
}


FileWriter::FileWriter(const std::string & filename)
	: File(filename) {
	_fileOpen = false;
}

FileWriter::FileWriter(const File & file)
	: File(file) {
}

FileWriter::FileWriter(const FileWriter & fileWriter)
	: File(fileWriter) {
}

FileWriter::~FileWriter() {
	close();
}

bool FileWriter::open() {
	_file.open(_filename.c_str(), ios::binary);
	_fileOpen = true;
	return isOpen();
}

bool FileWriter::isOpen() const {
	return _fileOpen;
}

void FileWriter::write(const std::string & data) {
	//See http://www.cplusplus.com/doc/tutorial/files.html

	if (!isOpen()) {
		open();
	}

	if (!data.empty()) {
		_file.write(data.c_str(), data.size());
	}
}

void FileWriter::close() {
	LOG_DEBUG("file=" + _filename + " saved");
	_file.close();
}
