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
	Picture(const std::string & data);

	/**
	 * Gets the data with format header.
	 *
	 * @return the data
	 */
	std::string getData() const;

private:

	std::string _pictureData;

};

#endif	//PICTURE_H
