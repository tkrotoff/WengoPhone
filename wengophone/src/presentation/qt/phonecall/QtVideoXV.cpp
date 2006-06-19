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

#include "QtVideoXV.h"

#include <cutil/global.h>

#include <webcam/WebcamDriver.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

#include <QtGui>

#include <util/Logger.h>

#include "XVWindow.h"

XVWindow* _localWindow;
XVWindow* _remoteWindow;

QtVideoXV::QtVideoXV(QWidget * parent, int rem_width, int rem_height, int loc_width, int loc_height) {
	_videoWindow = WidgetFactory::create(":/forms/phonecall/VideoWindow.ui", parent);

	//frame
	_frame = Object::findChild<QFrame *>(_videoWindow, "frame");
	_frameWindowFlags = _frame->windowFlags();

	//flipButton
	QPushButton * flipButton = Object::findChild<QPushButton *>(_frame, "flipButton");
	connect(flipButton, SIGNAL(clicked()), SLOT(flipWebcam()));

	//fullScreenButton
	_fullScreenButton = Object::findChild<QPushButton *>(_frame, "fullScreenButton");
	connect(_fullScreenButton, SIGNAL(clicked()), SLOT(fullScreenButtonClicked()));

	_encrustLocalWebcam = true;

	_remoteWindow = new XVWindow();
	if(_remoteWindow->init(XOpenDisplay(NULL), DefaultRootWindow(
		   XOpenDisplay(NULL)), 0, 0, rem_width * 2, rem_height * 2, rem_width, rem_height)) {
			_localWindow = new XVWindow();
			LOG_DEBUG("Remote window initialization: success");

			if(_encrustLocalWebcam && 
				_localWindow->init(XOpenDisplay(NULL), _remoteWindow->getWindow(),0,0,loc_width, loc_height,loc_width, loc_height)) {

				_remoteWindow->registerSlave(_localWindow);
				_localWindow->registerMaster(_remoteWindow);
				LOG_DEBUG("Local window initialization: success");

			} else {
				delete _localWindow;
				_localWindow = NULL;

				//TODO: in this case to have the local video image
				//we must compute the final image

				LOG_DEBUG("Local window initialization: failed");
			}

	} else {
		delete _remoteWindow;
		_remoteWindow = NULL;
		_localWindow = NULL;
		LOG_DEBUG("Remote window initialization: failed");
	}
}

QtVideoXV::~QtVideoXV() {
	if(_remoteWindow) {
		_remoteWindow->registerSlave(NULL);
		delete _remoteWindow;
	}
	if(_localWindow) {
		_localWindow->registerMaster(NULL);
		delete _localWindow;
	}
}

bool QtVideoXV::isFullScreen() {
	return _remoteWindow->isFullScreen();
}

QSize QtVideoXV::getFrameSize() {
	return _frame->frameRect().size();
}

QWidget* QtVideoXV::getWidget()  {
	return _videoWindow;
}

void QtVideoXV::showImage(piximage* remoteVideoFrame, piximage* localVideoFrame) {
	if (_remoteWindow) {
		_remoteWindow->putFrame(remoteVideoFrame);
		if (_localWindow) {
			_localWindow->putFrame(localVideoFrame);
		}
	}
}

void QtVideoXV::flipWebcam() {
	static bool flip = true;

	IWebcamDriver * driver = WebcamDriver::getInstance();
	driver->flipHorizontally(flip);

	flip = !flip;
}

void QtVideoXV::fullScreenButtonClicked() {
	_remoteWindow->toggleFullscreen();
}

void QtVideoXV::unFullScreen() {
	if( isFullScreen() ) {
		_remoteWindow->toggleFullscreen();
	}
}

bool QtVideoXV::isInitialized() {
	return (_remoteWindow != NULL);
}
