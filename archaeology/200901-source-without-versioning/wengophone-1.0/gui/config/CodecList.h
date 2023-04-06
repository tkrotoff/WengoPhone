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

#ifndef CODECLIST_H
#define CODECLIST_H

#include <qstring.h>

#include <vector>

class Codec;

/**
 * List of codecs supported by WengoPhone.
 *
 * @author Tanguy Krotoff
 */
class CodecList {
public:

	CodecList();

	~CodecList();

	/**
	 * Loads the default Codec list.
	 */
	void loadDefaultCodecList();

	/**
	 * Gets a Codec from the list given its name.
	 *
	 * @param codecName name of the code we want to get
	 * @return the Codec or NULL
	 */
	Codec * getCodec(const QString & codecName);

	/**
	 * Gets the number of Codec inside the CodecList.
	 *
	 * @return size of the collection/vector
	 */
	unsigned int size() const;

	/**
	 * Permits to use CodecList as an array.
	 *
	 * <code>
	 * CodecList CodecList;
	 * CodecList.addCodec(...);
	 * CodecList.addCodec(...);
	 * for (unsigned int i = 0; i  codecList.size(); i++) {
	 *     Codec & codec = codecList[i];
	 * }
	 * </code>
	 *
	 * @param i index inside the array
	 * @return the Codec that corresponds to the index i inside the CodecList
	 */
	const Codec & operator[] (unsigned int i) const;
	/**
	 * Appends the specified element to the end of the list.
	 *
	 * @param codec element to be appended to the CodecList
	 */
	void addCodec(const Codec & codec);



	/**
	 * @see operator[]
	 */
	Codec & operator[] (unsigned int i);

	QString toString();

private:

	/**
	 * Defines the vector of Codec.
	 */
	typedef std::vector<Codec> Codecs;

	/**
	 * List of Codec.
	 */
	Codecs _codecList;
};

#endif	//CODECLIST_H
