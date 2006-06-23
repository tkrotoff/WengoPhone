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

#include "QtVideoQt.h"

#include <cutil/global.h>

#include <webcam/WebcamDriver.h>

#include <qtutil/PaintEventFilter.h>
#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

#include <util/Logger.h>

#include <QtGui>

#if defined OS_WINDOWS && defined CC_MSVC
#include <windows.h>
#endif

QtVideoQt::QtVideoQt(QWidget * parent) {
	_videoWindow = WidgetFactory::create(":/forms/phonecall/VideoWindow.ui", parent);

	//frame
	_frame = Object::findChild<QFrame *>(_videoWindow, "frame");
	_frameWindowFlags = _frame->windowFlags();

	_qtVideoQtEventManager = new QtVideoQtEventManager(_frame, this);

	PaintEventFilter * paintFilter = new PaintEventFilter(this, SLOT(paintEvent()));
	_frame->installEventFilter(paintFilter);

	//flipButton
	QPushButton * flipButton = Object::findChild<QPushButton *>(_frame, "flipButton");
	connect(flipButton, SIGNAL(clicked()), SLOT(flipWebcam()));
#ifdef OS_WINDOWS
	flipButton->hide();
#endif

	//fullScreenButton
	_fullScreenButton = Object::findChild<QPushButton *>(_frame, "fullScreenButton");
	connect(_fullScreenButton, SIGNAL(clicked()), SLOT(fullScreenButtonClicked()));

	_encrustLocalWebcam = true;
	_fullScreen = false;
}

QtVideoQt::~QtVideoQt() {
}

QSize QtVideoQt::getFrameSize() {
	return _frame->frameRect().size();
}

QWidget * QtVideoQt::getWidget() {
	return _videoWindow;
}

void QtVideoQt::showImage(piximage * remoteVideoFrame, piximage * localVideoFrame) {

	QSize size(640, 480); // will be the optimum interim resized image
	if (_videoWindow) {
		// screen target size
		//QSize frameSize = _videoWindow->frameSize();
		QSize frameSize = _frame->frameRect().size();
		
		if (frameSize.width() < size.width()) {
			size.setWidth(352);
			size.setHeight(288);
		}
	}

	piximage * resizedImage = pix_alloc(PIX_OSI_RGB32, size.width(), size.height());
	pix_convert(PIX_NO_FLAG, resizedImage, remoteVideoFrame);

	QImage * image = new QImage(resizedImage->width,
		resizedImage->height, QImage::Format_RGB32);

	memcpy(image->bits(), resizedImage->data, pix_size(resizedImage->palette, resizedImage->width, resizedImage->height));

	pix_free(resizedImage);

	//If we want to embed the local webcam picture, we do it here
	if (_encrustLocalWebcam) {
		const unsigned offset_x = 10;
		const unsigned offset_y = 10;
		const unsigned ratio = 5;
		const unsigned border_size = 1;
		const QBrush border_color = Qt::black;

		// we force the ratio of the remote frame on the webcam frame (ignoring the webcam's aspect ratio)
		unsigned width = (size.width() / ratio) & ~1;
		unsigned height = (size.height() / ratio) & ~1;
		unsigned posx = size.width() - width - offset_x;
		unsigned posy = size.height() - height - offset_y;

		piximage * resizedLocalImage = pix_alloc(PIX_OSI_RGB32, width, height);
		pix_convert(PIX_NO_FLAG, resizedLocalImage, localVideoFrame);

		QImage localImage(resizedLocalImage->width,
			resizedLocalImage->height, QImage::Format_RGB32);

		memcpy(localImage.bits(), resizedLocalImage->data,
			pix_size(resizedLocalImage->palette, resizedLocalImage->width, resizedLocalImage->height));

		pix_free(resizedLocalImage);

		QPainter painter;
		painter.begin(image);
		// draw a 1-pixel border around the local embedded frame
		painter.fillRect(posx - border_size, posy - border_size, width + 2 * border_size,
			height + 2 * border_size, border_color);
		// embed the image
		painter.drawImage(posx, posy, localImage);
		painter.end();
	}

	_image = *image;
	_frame->update();
	delete image;
}

void QtVideoQt::paintEvent() {
	if (!_image.isNull()) {
		QPainter painter(_frame);
		QSize frameSize = _frame->frameRect().size();
		QSize size = frameSize;
		int xpos = 0, ypos = 0;

#if defined(OS_MACOSX) || defined(OS_LINUX)
		if (frameSize.width() > 640) {
			size.setWidth(640);
		}
		if (frameSize.height() > 480) {
			size.setHeight(480);
		}

		painter.fillRect(QRect(QPoint(0, 0), frameSize), Qt::black);

		xpos = (frameSize.width() - size.width()) / 2;
		ypos = (frameSize.height() - size.height()) / 2;
#endif

		painter.drawImage(xpos, ypos, _image.scaled(size,
			Qt::IgnoreAspectRatio, Qt::FastTransformation));
	}
}

void QtVideoQt::flipWebcam() {
	static bool flip = true;

	IWebcamDriver * driver = WebcamDriver::getInstance();
	driver->flipHorizontally(flip);

	flip = !flip;
}

void QtVideoQt::fullScreenButtonClicked() {
	_frame->setFocus();

	if(!_fullScreen) {
		this->fullScreen();
	} else {
		this->unFullScreen();
	}
}

bool QtVideoQt::isFullScreen() {
	return _fullScreen;
}

void QtVideoQt::fullScreen() {
	_fullScreenButton->setIcon(QIcon(":/pics/video_unfullscreen.png"));

	QLayout * layout = _videoWindow->layout();
	layout->removeWidget(_frame);

	_frame->setParent(NULL);

#if defined OS_WINDOWS && defined CC_MSVC
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
	_fullScreen = true;
	_frame->showFullScreen();
}

void QtVideoQt::unFullScreen() {
	_fullScreenButton->setIcon(QIcon(":/pics/video_fullscreen.png"));
	_frame->setParent(_videoWindow);
	QLayout * layout = _videoWindow->layout();
	layout->addWidget(_frame);

#if defined OS_WINDOWS && defined CC_MSVC
	//Restores previous resolution
	ChangeDisplaySettings(NULL, 0);
#endif

	_fullScreen = false;
	_frame->show();
}

bool QtVideoQt::isInitialized() {
	return true;
}


QtVideoQtEventManager::QtVideoQtEventManager(QFrame * target, QtVideoQt * qtVideoQt)
	: QObject(target), _qtVideoQt(qtVideoQt) {
	target->installEventFilter(this);
}

bool QtVideoQtEventManager::eventFilter(QObject *obj, QEvent *event) {

	if (event->type() == QEvent::KeyPress) {
		QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);

		if( keyEvent->key() == Qt::Key_Escape) {
			_qtVideoQt->unFullScreen();
		}

	}

	return QObject::eventFilter(obj, event);
}

