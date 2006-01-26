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

#ifndef WENBOX_H
#define WENBOX_H

#include <IWenbox.h>

/**
 * Plugin interface for USB phone devices.
 *
 * Wenbox is a USB phone device loaded at runtime via a .dll
 * Historically named Wenbox since the first USB device handled by WengoPhone
 * was named Wenbox.
 *
 * Wenbox aimed to handle different USB phones via runtime dll loading.
 * Each USB device should implement the interface IWenbox.
 *
 * @author Tanguy Krotoff
 */
class Wenbox : public IWenbox {
public:

	Wenbox();

	~Wenbox();

	bool open();

	bool isOpen() const {
		return _open;
	}

	bool close();

	std::string getDeviceName();

	StringList getAudioDeviceNameList() const;

	bool setDefaultMode(Mode mode);

	bool switchMode(Mode mode);

	bool setLCDMessage(const std::string & message);

	bool setRingingTone(int tone);

	bool setState(PhoneCallState state, const std::string & phoneNumber = String::null);

private:

	IWenbox * _wenboxPrivate;

	bool _open;
};

#endif	//WENBOX_H
