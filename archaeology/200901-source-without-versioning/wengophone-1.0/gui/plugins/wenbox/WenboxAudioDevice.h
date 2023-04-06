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

#ifndef WENBOXAUDIODEVICE_H
#define WENBOXAUDIODEVICE_H

class StringList;
class QString;
class QStringList;

/**
 * Tools for the Wenbox audio device.
 *
 * @author Tanguy Krotoff
 */
class WenboxAudioDevice {
public:

	/**
	 * Gets the audio device name associated with the Wenbox.
	 *
	 * @return Wenbox audio device name
	 */
	static QString getWenboxAudioDeviceName();

	/**
	 * Change the default audio device to the previous one.
	 *
	 * Because the Wenbox becomes the default audio device
	 * when you first plug it under Windows.
	 *
	 * @return true if change was done, false otherwise
	 */
	static bool rollbackDefaultAudioDevice();

	/**
	 * Checks if the Wenbox is the current WengoPhone playback audio device.
	 *
	 * @return true if the Wenbox is the current audio device, false otherwise
	 */
	static bool isWenboxAudioDeviceUsed();

	/**
	 * Changes the current WengoPhone audio device for the Wenbox.
	 *
	 * @return true if success, false otherwise
	 */
	static bool switchCurrentAudioDeviceToWenbox();

private:

	WenboxAudioDevice();
	WenboxAudioDevice(const WenboxAudioDevice &);
	WenboxAudioDevice & operator=(const WenboxAudioDevice &);
	~WenboxAudioDevice();

	/**
	 * Looks for the Wenbox audio device from the list of audio devices.
	 *
	 * @param audioDeviceList list of all the audio devices on the computer
	 * @return Wenbox audio device name
	 */
	static QString getWenboxAudioDeviceName(const StringList & audioDeviceList);

	/**
	 * Gets the list of audio device related to the Wenbox.
	 *
	 * @return list of the possible names for the Wenbox audio device
	 */
	static QStringList getWenboxAudioDeviceList();
};

#endif	//WENBOXAUDIODEVICE_H
