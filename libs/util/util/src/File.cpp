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

#include <stddef.h>
#include <sys/types.h>
#ifdef CC_MSVC
	#include <direct.h>
#endif

#include <sys/stat.h>

#if defined(OS_WINDOWS)
	#ifndef S_ISDIR
		#define S_ISDIR(x) ((x) & _S_IFDIR)
	#endif
	#ifndef S_ISREG
		#define S_ISREG(x) ((x) & _S_IFREG)
	#endif
#endif

#include <dirent.h>

#include <string>
#include <iostream>
using namespace std;

#ifdef OS_WINDOWS
	#include <windows.h>
#endif

File::File(const std::string & filename)
	: _filename(filename) {
}

std::string File::getPath() {
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
			if (stat(dir.c_str(), &statinfo) == 0) {
				if (S_ISDIR(statinfo.st_mode)) {
					//ep->d_name is a directory
					dirList += dir;
				}
			}
		}
	}

	closedir(dp);

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
			if (stat(file.c_str(), &statinfo) == 0) {
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
#endif	//OS_WINDOWS
}

void File::createPath(const std::string & path) {
	string::size_type index = path.find(File::getPathSeparator(), 0);
	while (index != string::npos) {
#ifdef CC_MSVC
		mkdir(path.substr(0, index).c_str());
#else
		mkdir(path.substr(0, index).c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
#endif
		index = path.find(File::getPathSeparator(), index + 1);
	}
}

FileReader::FileReader(const std::string & filename)
	: File(filename) {
}

FileReader::~FileReader() {
	close();
}

bool FileReader::open() {
	_file.open(_filename.c_str());
	return _file.is_open();
}

std::string FileReader::read() {
	static const unsigned int BUFFER_SIZE = 2000;

	if (!_file.is_open()) {
		//Tries to open the file if not already done
		open();
	}

	std::string data;
	if (_file.is_open()) {
		char tmp[BUFFER_SIZE];
		while (!_file.eof()) {
			_file.getline(tmp, BUFFER_SIZE);
			data += tmp;
		}
	}

	return data;
}

void FileReader::close() {
	_file.close();
}


FileWriter::FileWriter(const std::string & filename)
	: File(filename) {
}

FileWriter::~FileWriter() {
	close();
}

bool FileWriter::open() {
	_file.open(_filename.c_str());
	return _file.is_open();
}

void FileWriter::write(const std::string & data) {
	if (!_file.is_open()) {
		//Tries to open the file if not already done
		open();
	}

	if (_file.is_open() && !data.empty()) {
		_file << data;
	}
}

void FileWriter::close() {
	_file.close();
}
