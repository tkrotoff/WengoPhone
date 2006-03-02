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

#include "QtVideo.h"

#include <PaintEventFilter.h>
#include <WidgetFactory.h>
#include <Object.h>
#include <global.h>
#include <WebcamDriver.h>

#include <QtGui>

#ifdef OS_WINDOWS
	#include <windows.h>
#endif

#include <iostream>
using namespace std;

QtVideo::QtVideo(QWidget * parent) {
	_videoWindow = WidgetFactory::create(":/forms/phonecall/VideoWindow.ui", parent);

	//frame
	_frame = Object::findChild<QFrame *>(_videoWindow, "frame");
	_frameWindowFlags = _frame->windowFlags();

	PaintEventFilter * paintFilter = new PaintEventFilter(this, SLOT(paintEvent()));
	_frame->installEventFilter(paintFilter);

	//flipButton
	QPushButton * flipButton = Object::findChild<QPushButton *>(_frame, "flipButton");
	connect(flipButton, SIGNAL(clicked()), SLOT(flipWebcam()));

	//fullScreenButton
	_fullScreenButton = Object::findChild<QPushButton *>(_frame, "fullScreenButton");
	connect(_fullScreenButton, SIGNAL(clicked()), SLOT(fullScreenButtonClicked()));
}

void QtVideo::showImage(const QImage & image) {
	_image = image;
	_frame->update();
}

void QtVideo::paintEvent() {
	if (!_image.isNull()) {
		QPainter painter(_frame);
		painter.drawImage(0, 0, _image.scaled(_frame->frameRect().size()));
	}
}

void QtVideo::flipWebcam() {
	static bool flip = true;

	IWebcamDriver * driver = WebcamDriver::getInstance();
	driver->flipHorizontally(flip);

	flip = !flip;
}

void QtVideo::fullScreenButtonClicked() {
	static bool fullScreen = true;

	_frame->setFocus();

	if (fullScreen) {
		this->fullScreen();
	} else {
		this->unFullScreen();
	}

	fullScreen = !fullScreen;
}

void QtVideo::fullScreen() {
	_fullScreenButton->setIcon(QIcon(":/pics/video_unfullscreen.png"));

	QLayout * layout = _videoWindow->layout();
	layout->removeWidget(_frame);

	_frame->setParent(NULL);

#ifdef OS_WINDOWS
	//TODO put inside LibUtil

	int nModeExist;
	int nModeSwitch;
	DEVMODE devMode;

	//Tries to switch to desktop resolution 640x480
	//Uses less processor time if the desktop resolution is lower
	devMode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(NULL, 0, &devMode);
	for (int i = 1; ;i++) {
		nModeExist = EnumDisplaySettings(NULL, i, &devMode);
		if (nModeExist == 0) {
			//End of modes, bail out.
			break;
		}
		if ((devMode.dmBitsPerPel == 32) && (devMode.dmPelsWidth == 640) && (devMode.dmPelsHeight == 480)) {
			nModeSwitch = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
			break;
		}
	}
#endif

	_frame->showFullScreen();
}

void QtVideo::unFullScreen() {
	_fullScreenButton->setIcon(QIcon(":/pics/video_fullscreen.png"));
	_frame->setParent(_videoWindow);
	QLayout * layout = _videoWindow->layout();
	layout->addWidget(_frame);
	//FIXME bug inside Qt-4.1.0???
	//_frame->setWindowFlags((Qt::WindowFlags) (_frameWindowFlags & ~Qt::Window));
	_frame->setWindowFlags((Qt::WindowFlags) 0);
	_frame->setWindowFlags(Qt::SubWindow);

#ifdef OS_WINDOWS
	//Restores previous resolution
	ChangeDisplaySettings(NULL, 0);
#endif

	_frame->show();
}
