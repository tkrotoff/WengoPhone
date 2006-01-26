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

#ifndef IWENBOX_H
#define IWENBOX_H

#include <dllexport.h>

#include <NonCopyable.h>
#include <Event.h>
#include <StringList.h>

#ifdef WENBOX_DLL
	#ifdef BUILDING_DLL
		#define API DLLEXPORT
	#else
		#define API DLLIMPORT
	#endif
#else
	#define API
#endif

/**
 * @see Wenbox
 * @author Tanguy Krotoff
 */
class IWenbox : NonCopyable {
public:

	enum Key {
		Key0,
		Key1,
		Key2,
		Key3,
		Key4,
		Key5,
		Key6,
		Key7,
		Key8,
		Key9,

		/** * button. */
		KeyStar,

		/** # button. */
		KeySharp,

		/** Pick up button. */
		KeyPickUp,

		/** Hang up button. */
		KeyHangUp
	};

	/**
	 * A key has been pressed from the USB phone device.
	 *
	 * @param sender this class
	 * @param key key pressed by the user
	 */
	Event<void (IWenbox & sender, Key key)> keyPressedEvent;

	virtual ~IWenbox() { }

	/**
	 * Opens the USB device.
	 *
	 * @return true if manage to open the device
	 */
	virtual bool open() = 0;

	/**
	 * Closes the USB device.
	 *
	 * @return true if manage to close the device
	 */
	virtual bool close() = 0;

	/**
	 * Gets the USB device name,
	 *
	 * Example: "Yealink-USB-P1K", "Yealink-USB-B2K"...
	 *
	 * @return USB device name
	 */
	virtual std::string getDeviceName() = 0;

	/**
	 * Gets the audio device name list related to the USB device.
	 *
	 * The list should be ordered from the most specific name to the
	 * most generic (e.g USB).
	 *
	 * @return list of the possible names for the audio device
	 */
	virtual StringList getAudioDeviceNameList() const = 0;

	enum Mode {
		/** PSTN mode (standard phone line). */
		ModePSTN,

		/** USB mode. */
		ModeUSB
	};

	/**
	 * Sets the default mode: USB or PSTN.
	 *
	 * This method is only available for USB devices
	 * that support PSTN line.
	 *
	 * @param mode new mode
	 * @return true if manage to change the mode
	 */
	virtual bool setDefaultMode(Mode mode) = 0;

	/**
	 * Changes the current mode.
	 *
	 * This method is only available for USB devices
	 * that support PSTN line.
	 *
	 * @param mode new mode
	 * @return true if manage to change the mode
	 */
	virtual bool switchMode(Mode mode) = 0;

	/**
	 * Changes the LCD message on the USB device.
	 *
	 * This method is only available for USB devices
	 * that have a LCD screen.
	 *
	 * @param message message to show on the LCD screen
	 * @return true if manage to change the LCD screen message
	 */
	virtual bool setLCDMessage(const std::string & message) = 0;

	/**
	 * Changes the ringing tone.
	 *
	 * @param tone new ringing tone
	 * @return true if manage to change the ringing tone
	 */
	virtual bool setRingingTone(int tone) = 0;

	enum PhoneCallState {
		/** Incoming phone call. */
		CallIncoming,

		/** Conversation state. */
		CallTalking,

		/** An error occured. */
		CallError,

		/** Outgoing call. */
		CallOutgoing,

		/** Phone call closed (call rejected or call hang up). */
		CallClosed
	};

	/**
	 * Changes the USB device state.
	 *
	 * @param state new state
	 * @param phoneNumber only used when in state CallIncoming
	 * @return true if manage to change the state
	 */
	virtual bool setState(PhoneCallState state, const std::string & phoneNumber = String::null) = 0;
};

/**
 * Gets the IWenbox instance.
 *
 * Used for loading the DLL at runtime.
 * It's easier to load at runtime a C function rather than a C++ method.
 */
extern "C" API IWenbox * getInstance();

#endif	//IWENBOX_H
