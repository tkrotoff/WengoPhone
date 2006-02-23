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

#ifndef WENBOXPLUGIN_H
#define WENBOXPLUGIN_H

#include <wenbox/Wenbox.h>

class WengoPhone;

/**
 * Handles the Wenbox.
 *
 * Named WenboxPlugin rather than Wenbox since a class named Wenbox already exists.
 * Wenbox is a USB phone device loaded at runtime via a .dll
 *
 * @see IWenbox
 * @ingroup model
 * @author Tanguy Krotoff
 */
class WenboxPlugin {
public:

	/**
	 * @see IWenbox::keyPressedEvent
	 */
	Event<void (IWenbox & sender, IWenbox::Key key)> keyPressedEvent;

	WenboxPlugin(const WengoPhone & wengoPhone);

	~WenboxPlugin();

	/**
	 * @see IWenbox::setState()
	 */
	void setState(Wenbox::PhoneCallState state, const std::string & phoneNumber = String::null);

private:

	void keyPressedEventHandler(IWenbox & sender, IWenbox::Key key);

	std::string getWenboxAudioDeviceName() const;

	bool switchCurrentAudioDeviceToWenbox() const;

	Wenbox * _wenbox;

	const WengoPhone & _wengoPhone;
};

#endif	//WENBOXPLUGIN_H
