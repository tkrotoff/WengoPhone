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

#ifndef QTVIDEOQT_H
#define QTVIDEOQT_H

#include "QtVideo.h"

#include <QtGui>

class QtVideoQt : public QtVideo {
	Q_OBJECT
public:

	QtVideoQt(QWidget * parent);

	~QtVideoQt();

	void showImage(piximage* remoteVideoFrame, piximage* localVideoFrame);

	QWidget * getWidget();

	QSize getFrameSize();

	bool isInitialized();

	bool isFullScreen();

private Q_SLOTS:

	void paintEvent();

	void flipWebcam();

	void fullScreenButtonClicked();

private:

	void fullScreen();

	void unFullScreen();

	QWidget * _videoWindow;

	/** Current frame to show inside the video window. */
	QImage _image;

	QFrame * _frame;

	int _frameWindowFlags;

	QPushButton * _fullScreenButton;

	bool _encrustLocalWebcam;

	bool _fullScreen;
};

#endif  //QTVIDEOQT_H
