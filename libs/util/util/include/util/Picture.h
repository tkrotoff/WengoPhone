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

#ifndef PICTURE_H
#define PICTURE_H

#include <string>

/**
 * Represents a picture.
 *
 * TODO:
 *	- Detect the picture format from header.
 *	- Return the format of the loaded picture and its size
 *
 * @author Philippe Bernery
 */
class Picture {
public:

	/**
	 * Constructs an empty Picture.
	 */
	Picture();

	Picture(const Picture & picture);

	/**
	 * Constructs a Picture from data in memory.
	 * These data must include the header available in the file from where
	 * it has been extracted. The construtor will then guess the picture format
	 * from this header.
	 *
	 * @param data Picture data
	 */
	static Picture pictureFromData(const std::string & data);

	/**
	 * Constructs a Picture from a file.
	 *
	 * The data are loaded and copied in memory.
	 *
	 * @param data Picture data
	 */
	static Picture pictureFromFile(const std::string & filename);

	/**
	 * Gets the data with format header.
	 *
	 * @return the data
	 */
	std::string getData() const;

	/**
	 * Gets the name of the file from which the Picture has been created.
	 * This does not return the complete path of the file.
	 * @return the filename
	 */
	std::string getFilename() const;

	/**
	 * Set the filename of the Picture.
	 * Only the last part of the past is saved.
	 */
	void setFilename(const std::string & filename);

private:

	std::string _pictureData;

	std::string _filename;

};

#endif	//PICTURE_H
