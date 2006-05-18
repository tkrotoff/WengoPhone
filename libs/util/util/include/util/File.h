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

#include <util/NonCopyable.h>
#include <util/StringList.h>

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

	File(const File & file);

	virtual ~File() {
	}

	/**
	 * Gets the file extension.
	 *
	 * @return the file extension or NULL if there's no extension
	 */
	std::string File::getExtension();

	/**
	 * Gets the path to the file.
	 *
	 * The path does not contain the filename, it stops at the last /
	 *
	 * @return path to the file
	 */
	std::string getPath();

	/**
	 * Gets the full path to the file.
	 *
	 * @return path to the file
	 */
	std::string getFullPath();

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

	/**
	 * Creates directories recursively if the path does not exist.
	 *
	 * If path exists, nothing happends.
	 *
	 * @param path the path to create
	 */
	static void createPath(const std::string & path);

	/**
	 * Creates a temporary file.
	 *
	 * @return the temporary file
	 */
	static File createTemporaryFile();

	/**
	 * @return true if the given path exists.
	 */
	static bool exists(const std::string & path);

	const static std::string directorySeparator;

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

	FileReader(const FileReader & fileReader);

	FileReader(const File & file);

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

	FileWriter(const FileWriter & fileWriter);

	FileWriter(const File & file);

	~FileWriter();

	bool open();

	void write(const std::string & data);

	void close();

private:

	std::ofstream _file;
};

#endif	//FILE_H
