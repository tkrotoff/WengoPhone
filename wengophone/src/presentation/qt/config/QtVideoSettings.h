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

#ifndef QTVIDEOSETTINGS_H
#define QTVIDEOSETTINGS_H

#include "QtISettings.h"

#include <pixertool/pixertool.h>

#include <QPixmap>

class IWebcamDriver;

class QWidget;
class QString;
namespace Ui { class VideoSettings; }

/**
 * Video configuration panel.
 *
 * @author Tanguy Krotoff
 */
class QtVideoSettings : public QtISettings {
	Q_OBJECT
public:

	QtVideoSettings(QWidget * parent);

	virtual ~QtVideoSettings();

	QWidget * getWidget() const {
		return _videoSettingsWidget;
	}

	QString getName() const;

	void saveConfig();

private Q_SLOTS:

	void newWebcamImageCaptured(QPixmap pixmap);

	void webcamPreviewButtonPressed();

Q_SIGNALS:

	void newWebcamImage(QPixmap pixmap);

private:

	void readConfig();

	void frameCapturedEventHandler(IWebcamDriver * sender, piximage * image);

	Ui::VideoSettings * _ui;

	QWidget * _videoSettingsWidget;

	IWebcamDriver * _webcamDriver;

	/** True when the webcam has been opened by this widget. */
	bool _openedByMe;

	/** Contains the converted picture from the Webcam. */
	piximage * _rgbImage;
};

#endif	//QTVIDEOSETTINGS_H
