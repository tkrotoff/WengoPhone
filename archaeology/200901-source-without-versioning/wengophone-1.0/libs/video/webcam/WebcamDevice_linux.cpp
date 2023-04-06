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

#include "WebcamDevice.h"
#include <linux/videodev.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;


#define MAX_DEVICES 10

WebcamDevice::WebcamDevice() {
}

WebcamDevice::~WebcamDevice() {
}

QStringList WebcamDevice::getWebcamList(bool pci_device) {
	QStringList devs;
	devs += "/dev/video0";
	return devs;

	QString devName;
	int handle;
	struct video_capability tempCaps;

	for (int devC = 0; devC <= MAX_DEVICES; devC += 1) {
		devName = "/dev/video" + devC;
		cout << devName << "\n";
		handle = open(devName, O_RDWR);
		if (handle <= 0)
			continue;
	        
		ioctl(handle, VIDIOCGCAP, &tempCaps);
		devs += tempCaps.name;
	}

	return devs;
}

QString WebcamDevice::getDefaultDevice() {
	return "/dev/video0";
}
