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

#ifndef SOUNDMIXEREXCEPTION_H
#define SOUNDMIXEREXCEPTION_H

#include <util/exception/Exception.h>

/**
 * A problem occured in while manipulating an audio mixer device under Windows.
 *
 * @author Tanguy Krotoff
 */
class SoundMixerException : public Exception {
public:

	/**
	 * Constructs a SoundMixerException object.
	 *
	 * @param errorNumber MMRESULT from win32 sound API
	 * @param soundComponent sound component that generated the error
	 */
	SoundMixerException(const std::string & soundComponent, unsigned int errorNumber);

	virtual ~SoundMixerException() {
	}

	/**
	 * Gets the string corresponding to the win32 sound API error.
	 *
	 * @return the explanation for the a specific win32 sound API error
	 */
	virtual std::string what() const;

private:

	/**
	 * win32 sound API error code MMRESULT.
	 */
	unsigned int _errorNumber;

	/**
	 * Name of the sound component that has generated the error.
	 */
	std::string _soundComponent;
};

#endif	//SOUNDMIXEREXCEPTION_H
