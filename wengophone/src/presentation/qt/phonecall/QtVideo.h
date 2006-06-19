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

#ifndef QTVIDEO_H
#define QTVIDEO_H

#include <QObject>

#include <pixertool/pixertool.h>

class QSize;
class QWidget;
class QImage;
class QPushButton;

class QtVideo : public QObject {

public:

	QtVideo() {};

	virtual ~QtVideo() {};

	virtual void showImage(piximage* remoteVideoFrame, piximage* localVideoFrame) = 0;

	virtual QWidget * getWidget() = 0;

	virtual QSize getFrameSize() = 0;

	virtual bool isInitialized() = 0;

	virtual bool isFullScreen() = 0;

	virtual void unFullScreen() = 0;
	
};

#endif	//QTVIDEO_H
