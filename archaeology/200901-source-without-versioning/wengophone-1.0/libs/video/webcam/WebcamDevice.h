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

#ifndef WEBCAMDEVICE_H
#define WEBCAMDEVICE_H

#include <qstring.h>
#include <qstringlist.h>

/**
 * @author David Ferlier, Mathieu Stute
 */
 
#if defined(WIN32) && defined(DSHOW)
    #include <dshow.h>
    #include <atlbase.h>
    #include <qedit.h>
    #include <comutil.h>
#endif

class WebcamDevice {
public:

	WebcamDevice();
	~WebcamDevice();

	/**
	 * Gets the list of mixer devices.
	 *
	 * @return list of mixer devices
	 */
	static QStringList getWebcamList(bool pci_device = false);
	static QString getDefaultDevice();
	static bool deviceAvailable() {
		return (getWebcamList().count() > 0);
	}

private:

	WebcamDevice(const WebcamDevice &);
	WebcamDevice & operator=(const WebcamDevice &);
};

#endif	//WEBCAMDEVICE_H
