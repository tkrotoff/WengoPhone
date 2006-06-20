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

class IWebcamDriver;

class QImage;
class QHideEvent;
class QString;
class CWengoPhone;
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

	QtVideoSettings(CWengoPhone & cWengoPhone, QWidget * parent);

	virtual ~QtVideoSettings();

	QWidget * getWidget() const {
		return (QWidget *) this;
	}

	QString getName() const;

	void saveConfig();

private Q_SLOTS:

	void webcamPreview();

	void newWebcamImageCaptured(QImage * image);

	void startWebcamPreview(const QString & deviceName);

	void makeTestCallClicked();

Q_SIGNALS:

	void newWebcamImage(QImage * image);

private:

	void readConfig();

	void stopWebcamPreview();

	void frameCapturedEventHandler(IWebcamDriver * sender, piximage * image);

	void hideEvent(QHideEvent * event);

	Ui::VideoSettings * _ui;

	IWebcamDriver * _webcamDriver;

	/** Contains the converted picture from the Webcam. */
	piximage * _rgbImage;

	CWengoPhone & _cWengoPhone;
};

#endif	//QTVIDEOSETTINGS_H
