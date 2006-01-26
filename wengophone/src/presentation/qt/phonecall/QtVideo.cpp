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

#include <QPainter>
#include <QSize>
#include <QFrame>
#include <QCheckBox>

#include <WebcamDriver.h>

#include <iostream>
using namespace std;

QtVideo::QtVideo(QWidget * parent) {
	_videoWindow = WidgetFactory::create(":/forms/phonecall/VideoWindow.ui", parent);

	//frame
	_frame = Object::findChild<QFrame *>(_videoWindow, "frame");

	PaintEventFilter * paintFilter = new PaintEventFilter(this, SLOT(paintEvent()));
	_frame->installEventFilter(paintFilter);

	//flip check box
	_flipCheckBox = Object::findChild<QCheckBox *>(_frame, "flipCheckBox");
	QObject::connect(_flipCheckBox, SIGNAL(toggled(bool)), this, SLOT(flipHorizontally(bool)));
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

void QtVideo::flipHorizontally(bool flip) {
	IWebcamDriver *driver = WebcamDriver::getInstance();
	driver->flipHorizontally(flip);
}
