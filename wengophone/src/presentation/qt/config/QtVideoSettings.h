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

#ifndef QTVIDEOSETTINGS_H
#define QTVIDEOSETTINGS_H

#include <pixertool/pixertool.h>

#include <QtGui>
#include <QImage>

class IWebcamDriver;

class QtVideoSettings : public QWidget {
	Q_OBJECT

public:

	QtVideoSettings ( QWidget * parent = 0, Qt::WFlags f = 0 );

	void saveData();

Q_SIGNALS:

	void newWebcamImage(QImage *image);

public Q_SLOTS:

	void widgetHidden();

protected Q_SLOTS:

	void newWebcamImageCaptured(QImage *image);

	void webcamPreviewButtonPressed();

protected:

	void frameCapturedEventHandler(IWebcamDriver *sender, piximage *image);

	void setupChilds();

	void readConfigData();

	QWidget * _widget;

	QComboBox * _webcamDeviceComboBox;

	QPushButton * _webcamPreviewPushButton;

	QLabel * _webcamPreviewLabel;

	QTreeWidget * _videoQualityTreeWidget;

	QGroupBox * _videoEnabled;

	QPushButton * _makeTestVideoCallPushButton;

	IWebcamDriver * _webcamDriver;

	/** True when the webcam has been opened by this widget */
	bool _openedByMe;

	/** Contains the converted picture from the Webcam. */
	piximage * _rgbImage;

};

#endif
