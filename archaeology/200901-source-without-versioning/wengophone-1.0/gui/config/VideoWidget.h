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

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <observer/Observer.h>
#include "util/WidgetStackControl.h"
#include "ConfigWindow.h"

#ifdef ENABLE_VIDEO
#include <webcam/webcam.h>
#endif

#include <qobject.h>
#include <qcheckbox.h>
#include <qevent.h>

class Event;
class Subject;
class MainWindow;
class QWidget;
class QPushButton;
class QComboBox;
class QListView;
class QListViewItem;
class Video;
class QSlider;
class QFrame;

/**
 * GUI Widget used to select video devices.
 *
 * @author David Ferlier
 * @author Mathieu Stute
 */
class VideoWidget : public QObject, public Observer {
	Q_OBJECT
public:

	VideoWidget(ConfigWindow & configWindow, MainWindow & mainWindow);

	/**
	 * Gets the low-level widget.
	 *
	 * @return the low-level widget for this gui component
	 */
	QWidget * getWidget() const {
		return _videoWidget;
	}

	void customEvent(QCustomEvent *);

	/**
	 * Callback, called each time a Subject has changed.
	 *
	 * @param subject Subject that has been changed
	 * @param event Event
	 * @see Observer::update();
	 */
	void update(const Subject &, const Event & event);

	/**
	 * Saves changes.
	 */
	void save();


	VideoWidget(const VideoWidget &);
	VideoWidget & operator=(const VideoWidget &);

	QPushButton * getWebcamPreviewButton() {
		return _webcamPreviewButton;
	}

#ifdef ENABLE_VIDEO
	WebcamOSI * getWebcam() {
		return webcam;
	}
#endif

	QWidget * getWebcamPreview() {
		return _webcamPreview;
	}

	void stopCapture();
	
	QCheckBox * getEnableVideoCheckBox() {
		return _enableCheckBox;
	}
	
public slots:

	void setInputDevice(const QString & inputDeviceName);

	/// init combo boxes
	void initComboBoxes();

private slots:

	/**
	 * Sets default settings.
	 */
	void defaultButtonClicked();

	void previewButtonClicked();

	void setVideoEnable(bool enable);

	void setResolution(const QString & res);
	void setConnectionType(QListViewItem * item);

private:

	void setConnectionType(const QString & type);

	/// graphical component
	QWidget * _videoWidget;

	// webcam preview
	QFrame * _webcamPreview;

	/// webcam preview button
	QPushButton * _webcamPreviewButton;

	/// connectiontype combobox
	QListView * _connectionType;

	QListViewItem * _currentItem;

	/// input device combo box
	QComboBox * _inputComboBox;

	/// FPS slider
	QSlider * _fpsSlider;

	/// Video quality slider
	QSlider * _qualitySlider;

	/// subject
	Video & _video;

	/// enable video checkbox
	QCheckBox * _enableCheckBox;

	ConfigWindow & _configWindow;
	MainWindow & _mainWindow;

#ifdef ENABLE_VIDEO
	WebcamOSI *webcam;
#endif

	void startCapture();

	bool isDevicePlugged();
	
	static const QString excellent;
	static const QString veryGood;
	static const QString good;
	static const QString normal;
};

#endif	//VIDEOWIDGET_H
