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

#ifndef FILE_H
#define FILE_H

#include <NonCopyable.h>
#include <StringList.h>

#include <string>
#include <fstream>

/**
 * File interface.
 *
 * An abstract representation of file and directory pathnames.
 *
 * TODO use FileNotFoundException?
 *
 * @see QFile
 * @see java.io.*
 * @author Tanguy Krotoff
 */
class File : NonCopyable {
public:

	File(const std::string & filename);

	virtual ~File() {
	}

	/**
	 * Gets the path to the file.
	 *
	 * @return path to the file
	 */
	std::string getPath();

	/**
	 * Gets directory list.
	 *
	 * @return a list of directories contained in 'this' directory
	 */
	StringList getDirectoryList() const;

	/**
	 * Gets file list.
	 *
	 * TODO Not implemented yet.
	 *
	 * @return a list of files contained in 'this' directory
	 */
	StringList getFileList() const;

	/**
	 * Gets the directory that contains the application executable.
	 *
	 * The executable path returned always finished by "/" or "\".
	 *
	 * @param application executable path
	 */
	static std::string getApplicationDirPath();

	/**
	 * Gets pathName with the '/' separators converted to separators that are appropriate for the underlying operating system.
	 *
	 * On Windows, convertPathSeparators("c:/winnt/system32") returns "c:\winnt\system32".
	 *
	 * @param path path to convert
	 * @return path converted
	 */
	static std::string convertPathSeparators(const std::string & path);

	/**
	 * Gets the native directory separator: "/" under Unix (including Mac OS X) and "\" under Windows.
	 *
	 * @return native system path separator
	 */
	static std::string getPathSeparator();

protected:

	std::string _filename;
};


/**
 * Reads from a file.
 *
 * @author Tanguy Krotoff
 */
class FileReader : public File {
public:

	FileReader(const std::string & filename);

	~FileReader();

	bool open();

	std::string read();

	void close();

private:

	std::ifstream _file;
};


/**
 * Writes to a file.
 *
 * @author Tanguy Krotoff
 */
class FileWriter : public File {
public:

	FileWriter(const std::string & filename);

	~FileWriter();

	bool open();

	void write(const std::string & data);

	void close();

private:

	std::ofstream _file;
};

#endif	//FILE_H
