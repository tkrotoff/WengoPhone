/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWIRECEIVEFILESESSION_H
#define OWIRECEIVEFILESESSION_H

#include <coipmanager/filesessionmanager/IFileSession.h>

/**
 * Interface for receive file session implementations.
 *
 * @author Philippe Bernery
 */
class IReceiveFileSession : public IFileSession {
public:

	COIPMANAGER_API IReceiveFileSession();

	COIPMANAGER_API IReceiveFileSession(const IReceiveFileSession & iReceiveFileSession);

	COIPMANAGER_API virtual ~IReceiveFileSession();

	/**
	 * Sets the path for saving the File.
	 */
	COIPMANAGER_API void setFilePath(const std::string & path);

	/**
	 * Gets the path to the File to be saved.
	 */
	COIPMANAGER_API std::string getFilePath() const;

	/**
	 * Gets the name of the file to receive.
	 */
	COIPMANAGER_API std::string getFileName() const;

	/**
	 * Gets the size of the file to receive.
	 */
	COIPMANAGER_API unsigned getFileSize() const;

protected:

	/** Path to save place of the file. */
	std::string _filePath;

	/** Name of the file to receive. */
	std::string _fileName;

	/** Size of the file to receive. */
	unsigned _fileSize;
};

#endif	//OWIRECEIVEFILESESSION_H
