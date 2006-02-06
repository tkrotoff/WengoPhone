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

#ifndef QTVIDEO_H
#define QTVIDEO_H

#include <QObject>
#include <QImage>

#include <IWebcamDriver.h>

class QWidget;
class QFrame;
class QSize;
class QCheckBox;

class QtVideo : public QObject {
	Q_OBJECT
public:

	QtVideo(QWidget * parent);

	void showImage(const QImage & image);

	QWidget * getWidget() const {
		return _videoWindow;
	}

private Q_SLOTS:

	void paintEvent();
	void flipHorizontally(bool flip);

private:

	QWidget * _videoWindow;

	/** Current frame to show inside the video window. */
	QImage _image;

	QFrame * _frame;
	
	QCheckBox * _flipCheckBox;
	
	IWebcamDriver *_driver;
};

#endif	//QTVIDEO_H
