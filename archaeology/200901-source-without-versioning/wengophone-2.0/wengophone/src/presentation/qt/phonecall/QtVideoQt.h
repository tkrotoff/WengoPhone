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

#ifndef OWQTVIDEOQT_H
#define OWQTVIDEOQT_H

#include "QtVideo.h"

#include <QtGui/QImage>

class QtVideoQtEventManager;

class QtVideoQt : public QtVideo {
	Q_OBJECT
public:

	QtVideoQt(QWidget * parent);

	~QtVideoQt();

	void showImage(piximage * remoteVideoFrame, piximage * localVideoFrame);

	QSize getFrameSize() const;

	bool isInitialized() const {
		return true;
	}

	bool isFullScreen() const {
		return _fullScreen;
	}

	void fullScreen();

	void unFullScreen();

Q_SIGNALS:

	void toggleFlipVideoImageSignal();

private Q_SLOTS:

	void paintEvent();

	void fullScreenButtonClicked();

	void flipWebcamButtonClicked();

private:

	/** Current frame to show inside the video window. */
	QImage _image;

	bool _fullScreen;

	QtVideoQtEventManager * _qtVideoQtEventManager;
};


class QFrame;
class QEvent;

class QtVideoQtEventManager : public QObject {
	Q_OBJECT
public:

	QtVideoQtEventManager(QFrame * target, QtVideoQt * qtVideoQt);

private:

	bool eventFilter(QObject * object, QEvent * event);

	QtVideoQt * _qtVideoQt;
};

#endif	//OWQTVIDEOQT_H
