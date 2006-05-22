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

#include <webcam/V4LWebcamDriver.h>

#include <pixertool/v4l-pixertool.h>

#include <util/File.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <iostream>

using namespace std;

V4LWebcamDriver::V4LWebcamDriver(WebcamDriver *driver, int flags)
: IWebcamDriver(flags) {
	_webcamDriver = driver;

	_fhandle = 0;
}

V4LWebcamDriver::~V4LWebcamDriver() {
	cleanup();
}

void V4LWebcamDriver::cleanup() {
	if (_fhandle > 0) {
		close(_fhandle);
	}
	_fhandle = 0;
	_isOpened = false;
	_terminate = false;
	_fps = 15;
}


void V4LWebcamDriver::terminate() {
	_terminate = true;
}

StringList V4LWebcamDriver::getDeviceList() {
	StringList deviceList;
	DevNameArray devName = getDevices();

	DevNameArray::const_iterator i = devName.begin();

	while (i != devName.end()) {
		deviceList+=((*i).second);
		i++;
	}

	return deviceList;
}

string V4LWebcamDriver::getDefaultDevice() {
	string defaultDeviceName;
	DevNameArray devName = getDevices();

	defaultDeviceName = devName["video0"];

	return defaultDeviceName;
}

webcamerrorcode V4LWebcamDriver::setDevice(const std::string & deviceName) {
	//TODO: test if a webcam is already opened

	std::string device = "/dev/" + deviceName.substr(deviceName.size() - 6, deviceName.size() - 1);

#if 0
	//Looking for the device path
	//FIXME: the current device naming scheme allows different devices to have
	//	the same name. This can produce some unwanted behavior (such as selecting
	//	the wrong device)
	string device;
	DevNameArray devName = getDevices();
	DevNameArray::const_iterator i = devName.begin();
	while (i != devName.end()) {
		if ((*i).second == deviceName) {
			device = "/dev/" + (*i).first;
			break;
		}
		i++;
	}
#endif
	
	_fhandle = open(device.c_str(), O_RDWR);
	if (_fhandle <= 0) {
		return WEBCAM_NOK;
	}

	fcntl(_fhandle, O_NONBLOCK);

	_isOpened = true;

	readCaps();

	return WEBCAM_OK;
}

bool V4LWebcamDriver::isOpened() const {
	return _isOpened;
}

void V4LWebcamDriver::startCapture() {
	start();
}

void V4LWebcamDriver::pauseCapture() {
	//TODO: implement pause
}

void V4LWebcamDriver::stopCapture() {
	terminate();
}

webcamerrorcode V4LWebcamDriver::setPalette(pixosi palette) {
	int depth;
	int v4l_palette = pix_v4l_from_pix_osi(palette);
	
	switch (palette) {
	case PIX_OSI_YUV420P:
		depth = 12;
		break;

	case PIX_OSI_YUV422:
		depth = 16;
		break;

	case PIX_OSI_YUV422P:
		depth = 16;
		break;

	case PIX_OSI_RGB32:
		depth = 32;
		break;

	case PIX_OSI_RGB24:
		depth = 24;
		break;

	default:
		depth = 0;
		break;
	}

	_vPic.palette = v4l_palette;
	_vPic.depth = depth;

	ioctl(_fhandle, VIDIOCSPICT, &_vPic);

	readCaps();

	return (_vPic.palette == v4l_palette ? WEBCAM_OK : WEBCAM_NOK);
}

pixosi V4LWebcamDriver::getPalette() const {
	return pix_v4l_to_pix_osi(_vPic.palette);
}

webcamerrorcode V4LWebcamDriver::setFPS(unsigned fps) {
	_fps = fps;

	return WEBCAM_OK;
}

unsigned V4LWebcamDriver::getFPS() const {
	return _fps;
}

webcamerrorcode V4LWebcamDriver::setResolution(unsigned width, unsigned height) {
	memset(&_vWin, 0, sizeof(struct video_window));
	_vWin.width = width;
	_vWin.height = height;

	if (ioctl(_fhandle, VIDIOCSWIN, &_vWin) == -1) {
		return WEBCAM_NOK;
	}

	readCaps();

	return WEBCAM_OK;
}

unsigned V4LWebcamDriver::getWidth() const {
	return _vWin.width;
}

unsigned V4LWebcamDriver::getHeight() const {
	return _vWin.height;;
}

void V4LWebcamDriver::setBrightness(int brightness) {
}

int V4LWebcamDriver::getBrightness() const {
	return 0;
}

void V4LWebcamDriver::setContrast(int contrast) {
}

int V4LWebcamDriver::getContrast() const {
	return 0;
}

void V4LWebcamDriver::flipHorizontally(bool flip) {
	//TODO: add horizontal flip support
}

void V4LWebcamDriver::readCaps() {
	if (isOpened()) {
		ioctl(_fhandle, VIDIOCGCAP, &_vCaps);
		ioctl(_fhandle, VIDIOCGWIN, &_vWin);
		ioctl(_fhandle, VIDIOCGPICT, &_vPic);
	}
}

void V4LWebcamDriver::run() {
	int len, fsize;
	piximage *image;

	image = pix_alloc(getPalette(), getWidth(), getHeight());

	while (isOpened() && !_terminate) {
		msleep(1000 / _fps);

		fsize = pix_size(image->palette, image->width, image->height);
		len = read(_fhandle, image->data, pix_size(image->palette, image->width, image->height));

		if (len >= fsize) {
			if (!isOpened()) {
				break;
			}
			_webcamDriver->frameBufferAvailable(image);
		}
	}

	pix_free(image);
}

V4LWebcamDriver::DevNameArray V4LWebcamDriver::getDevices() {
	return getDevices2_6();
}

V4LWebcamDriver::DevNameArray V4LWebcamDriver::getDevices2_6() {
	const string dir = "/sys/class/video4linux";
	DevNameArray array;
	FileReader sysDir(dir);
	StringList list = sysDir.getDirectoryList();

	
	for (register unsigned i = 0 ; i < list.size() ; i++) {
		if (list[i][0] != '.') {
			ifstream nameFile((dir + "/" + list[i] + "/name").c_str());
			char buffer[512];
			nameFile.getline(buffer, 512);
			
			strncat(buffer, (" : " + list[i]).c_str(), list[i].size() + 3); 

			array[list[i]] = buffer;
		}
	}

	return array;
}

/* TODO: test this part
V4LWebcamDriver::DevNameArray V4LWebcamDriver::getDevices2_4() {
	const string dir = "/proc/video/dev";
	DevNameArray array;
	FileReader sysDir(dir);
	StringList list = sysDir.getDirectoryList();

	for (register unsigned i = 0 ; i < list.size() ; i++) {
		if (list[i][0] != '.') {
			ifstream nameFile((dir + list[i]).c_str());
			char buffer[512];
			nameFile.getline(buffer, 512);
			char *colonPos = strchr(buffer, ':');

			array[list[i]] = colonPos + 2;
		}
	}

	return array;
}
*/
