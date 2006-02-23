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

#ifndef PHONECALLSTATE_H
#define PHONECALLSTATE_H

#include <sipwrapper/EnumPhoneCallState.h>

#include <string>

class PhoneCall;
class Sound;

/**
 * Represents the state of a PhoneCall.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class PhoneCallState {
public:

	PhoneCallState();

	virtual ~PhoneCallState() {
	}

	virtual void execute(PhoneCall & phoneCall) = 0;

	/**
	 * Gets the status code corresponding to this PhoneCall state.
	 *
	 * @return status code of this state
	 */
	virtual int getCode() const = 0;

	/**
	 * Gets the string representation of this PhoneCall state.
	 *
	 * @return string representation of this state
	 */
	virtual std::string toString() const = 0;

protected:

	static std::string getRingerAudioDeviceName();

	static void stopSoundRingin();

	static std::string getSoundRinginFile();

	static std::string getSoundCallClosedFile();

	static Sound * _soundRingin;
};

#endif	//PHONECALLSTATE_H
