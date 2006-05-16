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

#include <QWidget>
#include <QPixmap>

class IWebcamDriver;

class QWidget;
class QHideEvent;
class QShowEvent;
class QString;
namespace Ui { class VideoSettings; }

/**
 * Video configuration panel.
 *
 * Inherits from QWidget so we can overwrite QWidget::hideEvent() and
 * QWidget::showEvent() that are protected methods from QWidget.
 *
 * @author Tanguy Krotoff
 */
class QtVideoSettings : public QWidget, public QtISettings {
	Q_OBJECT
public:

	QtVideoSettings(QWidget * parent);

	virtual ~QtVideoSettings();

	QWidget * getWidget() const {
		return (QWidget *) this;
	}

	QString getName() const;

	void saveConfig();

private Q_SLOTS:

	void newWebcamImageCaptured(QPixmap pixmap);

	void webcamPreview(const QString & deviceName);

Q_SIGNALS:

	void newWebcamImage(QPixmap pixmap);

private:

	void readConfig();

	void frameCapturedEventHandler(IWebcamDriver * sender, piximage * image);

	void hideEvent(QHideEvent * event);

	void showEvent(QShowEvent * event);

	Ui::VideoSettings * _ui;

	IWebcamDriver * _webcamDriver;

	/** Contains the converted picture from the Webcam. */
	piximage * _rgbImage;

	bool _webcamDeviceOpened;
};

#endif	//QTVIDEOSETTINGS_H
