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

#include "VideoWidget.h"
#include "webcam/WebcamDevice.h"

#include "MyWidgetFactory.h"
#include "ConfigWindow.h"
#include "Video.h"
#include "Softphone.h"
#include "StatusBar.h"
#include "MainWindow.h"
#include "util/EventFilter.h"

#include <SoundMixer.h>

#include <qwidget.h>
#include <qbuttongroup.h>
#include <qimage.h>
#include <qslider.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qlcdnumber.h>
#include <qevent.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qlistview.h>

#include <avcodec.h>

#include <phapi.h>

#include <cassert>
#include <iostream>
using namespace std;

const QString VideoWidget::excellent = tr("Excellent");
const QString VideoWidget::veryGood = tr("Very Good");
const QString VideoWidget::good = tr("Good");
const QString VideoWidget::normal = tr("Normal");

class LocalFrameEvent : public QCustomEvent {
public:
	enum Type { FrameReady = (QEvent::User + 200) };

	LocalFrameEvent(Type t, const QImage &f, int w, int h) : QCustomEvent(t), frame(f) {
		width = w;
		height = h;
	}
	~LocalFrameEvent() {}

	QString msg() { return text;}
	QImage frame;
	int width, height;

private:
	QString text;
};

int framecallback(void *userdata, uint8_t *buf_frameready, int width_frameready, int height_frameready, int pix_osi_frameready, int len_frameready) {
	VideoWidget *vw = (VideoWidget *) userdata;

	if ((!vw->getWebcam()) || (!vw->getWebcam()->isOpened())) {
		return 0;
	}

	uint8_t *rgbFrame = vw->getWebcam()->convertImage((uint8_t *) buf_frameready, width_frameready, height_frameready, pix_osi_frameready, WEBCAM_PALETTE_RGB32);
	if(!rgbFrame)
		return 0;

	QImage image((uint8_t*)rgbFrame, width_frameready, height_frameready, 32,
			(QRgb *)0, 0, QImage::LittleEndian);

	QImage scaledImage = image.scale(
		vw->getWebcamPreview()->width() & ~1,
			vw->getWebcamPreview()->height() & ~1, QImage::ScaleMax);

	QApplication::postEvent(vw, new LocalFrameEvent(
			LocalFrameEvent::FrameReady, scaledImage, scaledImage.width(), scaledImage.height()));

	av_free(rgbFrame);
	return 0;
}

VideoWidget::VideoWidget(ConfigWindow & configWindow, MainWindow & mainWindow)
: QObject(configWindow.getWidget()), _configWindow(configWindow), _video(Video::getInstance()), _mainWindow(mainWindow) {

	QWidget * parent = _configWindow.getWidget();

//	_video.addObserver(*this);
	_videoWidget = MyWidgetFactory::create("VideoWidgetForm.ui", this, parent);

	_inputComboBox = (QComboBox *) _videoWidget->child("inputComboBox");
	_inputComboBox->setDuplicatesEnabled(false);
	connect(_inputComboBox, SIGNAL(activated(const QString & )),
		SLOT(setInputDevice(const QString &)));

	_connectionType = (QListView *) _videoWidget->child("connectionType");
	_connectionType->setSorting(-1);
	_connectionType->setSortColumn(-1);
	_connectionType->insertItem(new QListViewItem(_connectionType, tr(excellent), tr("8192+"), tr("1024+")));
	_connectionType->insertItem(new QListViewItem(_connectionType, tr(veryGood), tr("2048+"), tr("256+")));
	_connectionType->insertItem(new QListViewItem(_connectionType, tr(good), tr("512 to 2048"), tr("128 to 256")));
	_connectionType->insertItem(new QListViewItem(_connectionType, tr(normal), tr("0 to 512"), tr("0 to 128")));
	_currentItem = NULL;

	connect(_connectionType, SIGNAL(selectionChanged(QListViewItem *)),
		SLOT(setConnectionType(QListViewItem *)));

	QComboBox * resolutionBox = (QComboBox *) _videoWidget->child("captureResolution");
	resolutionBox->hide();
	/*resolutionBox->setDuplicatesEnabled(false);
	if (_video.getCaptureResolution() != "") {
		resolutionBox->setCurrentText(_video.getCaptureResolution());
	}
	connect(resolutionBox, SIGNAL(activated ( const QString & )), SLOT(setResolution(const QString &)));*/

	QLabel * captureResolutionLabel = (QLabel *) _videoWidget->child("captureResolutionLabel");
	captureResolutionLabel->hide();

	_fpsSlider = (QSlider *) _videoWidget->child("fpsSlider");
	_fpsSlider->setValue(_video.getFrameRate().toInt());
	QLCDNumber * lcdFps = (QLCDNumber *) _videoWidget->child("fpsLcd");
	lcdFps->display(_video.getFrameRate().toInt());

	QLabel * l1 = (QLabel*)_videoWidget->child("qualityLabel");
	l1->hide();
	QLabel * l2 = (QLabel*)_videoWidget->child("fpsLabel");
	l2->hide();

	_qualitySlider = (QSlider *) _videoWidget->child("qualitySlider");
	_qualitySlider->setValue(_video.getQuality().toInt());
	QLCDNumber * lcdQuality = (QLCDNumber *) _videoWidget->child("frameSizeLcd");
	lcdQuality->display(_video.getQuality().toInt());

	_webcamPreview = (QFrame *) _videoWidget->child("webcamPreviewWidget", "QFrame");

	_webcamPreviewButton = (QPushButton *) _videoWidget->child("webcamPreviewButton");

	_enableCheckBox = (QCheckBox *) _videoWidget->child("enableVideoCheckBox");
	_enableCheckBox->setChecked(_video.getEnable());
	connect(_enableCheckBox, SIGNAL(toggled(bool)),
		this, SLOT(setVideoEnable(bool)));
	setVideoEnable(_video.getEnable());

	webcam = NULL;
	connect(_webcamPreviewButton, SIGNAL(clicked()),
		this, SLOT(previewButtonClicked()));

	ShowEventFilter * showEventFilter = new ShowEventFilter(this, SLOT(initComboBoxes()));
	_videoWidget->installEventFilter(showEventFilter);

	//hide FPS & quality widgets
	_fpsSlider->hide();
	lcdFps->hide();
	lcdQuality->hide();
	_qualitySlider->hide();

	//Update widget
	initComboBoxes();
}

void VideoWidget::customEvent(QCustomEvent *ev) {
	LocalFrameEvent * lfe = (LocalFrameEvent *) ev;
	bitBlt(_webcamPreview, 0, 0, &(lfe->frame));
}

void VideoWidget::defaultButtonClicked() {
	_video.defaultSettings();
}

void VideoWidget::previewButtonClicked() {
	//initComboBoxes();
	startCapture();
}

void VideoWidget::update(const Subject &, const Event &) {
	//initComboBoxes();
}

void VideoWidget::save() {
	if (_inputComboBox->count () && (_inputComboBox->currentText()  != _video.getInputDeviceName())) {
		_video.changeSettings(_inputComboBox->currentText());
		_video.storeSettings();
	}
	_video.setQuality(QString::number(_qualitySlider->value()));
	_video.setFrameRate(QString::number(_fpsSlider->value()));
	_video.setFrameRate(QString::number(_fpsSlider->value()));
	setConnectionType(NULL);
}

void VideoWidget::initComboBoxes() {
	QStringList deviceList(WebcamDevice::getWebcamList(true));
	_inputComboBox->clear();
	_inputComboBox->insertStringList(deviceList);
	if (!_video.getInputDeviceName().isNull() && isDevicePlugged()) {
		_inputComboBox->setCurrentText(_video.getInputDeviceName());
	}
	if (_inputComboBox->count()) {
		setInputDevice(_inputComboBox->currentText());
		_enableCheckBox->setEnabled(true);
		_mainWindow.getEnableVideoCheckBox()->setEnabled(true);
	} else {
		_enableCheckBox->setEnabled(false);
		_mainWindow.getEnableVideoCheckBox()->setEnabled(false);
	}
	
	if (_video.getConnectionType() == "128") {
		_connectionType->setSelected(_connectionType->findItem(tr(normal), 0), true);
	} else if (_video.getConnectionType() == "256") {
		_connectionType->setSelected(_connectionType->findItem(tr(good), 0), true);
	} else if (_video.getConnectionType() == "512") {
		_connectionType->setSelected(_connectionType->findItem(tr(veryGood), 0), true);
	} else if (_video.getConnectionType() == "1024") {
		_connectionType->setSelected(_connectionType->findItem(tr(excellent), 0), true);
	}
}

void VideoWidget::setInputDevice(const QString & inputDeviceName) {
	stopCapture();
	_video.setInputDevice(inputDeviceName);
}

void VideoWidget::setVideoEnable(bool enable) {
	_video.setEnable(enable);
	QButtonGroup * webcamButtonGroup = (QButtonGroup *) _videoWidget->child("webcamButtonGroup");
	QButtonGroup * connectionTypeButtonGroup = (QButtonGroup *) _videoWidget->child("connectionTypeButtonGroup");
	webcamButtonGroup->setEnabled(enable);
	connectionTypeButtonGroup->setEnabled(enable);
	_mainWindow.getEnableVideoCheckBox()->setChecked(enable);
}

void VideoWidget::startCapture() {
	stopCapture();
	webcam = new WebcamOSI();
	webcam->setUserData(this);
	webcam->setFrameReadyCallback(framecallback);
	webcam->setDevice((char *)_video.getInputDeviceName().ascii());
	if (webcam->open() == WEBCAM_CODE_OK) {
		webcam->startCapture();
	} else {
		webcam->uninitialize();
		delete webcam;
		webcam = NULL;
	}
}

void VideoWidget::stopCapture() {
	if (webcam != NULL && webcam->isOpened()) {
		webcam->close();
		webcam->uninitialize();
		delete webcam;
		webcam = NULL;
	}
}

bool VideoWidget::isDevicePlugged() {
	QStringList deviceList(WebcamDevice::getWebcamList(true));
	for (QStringList::Iterator it = deviceList.begin(); it != deviceList.end(); ++it) {
		if (_video.getInputDeviceName() == (*it)) {
			return true;
		}
	}
	return false;
}

void VideoWidget::setResolution(const QString & res) {
	_video.setCaptureResolution(res);
}

void VideoWidget::setConnectionType(QListViewItem * item) {
	if (item) {
		_currentItem = item;
	}
	if (_currentItem) {
		if( _currentItem->text(0) == tr(excellent) ) {
			_video.setConnectionType("1024");
		} else if(_currentItem->text(0) == tr(veryGood)) {
			_video.setConnectionType("512");
		} else if(_currentItem->text(0) == tr(good)) {
			_video.setConnectionType("256");
		} else if(_currentItem->text(0) == tr(normal)) {
			_video.setConnectionType("128");
		}
	}
}

void VideoWidget::setConnectionType(const QString & type) {
	_video.setConnectionType(type);
}
