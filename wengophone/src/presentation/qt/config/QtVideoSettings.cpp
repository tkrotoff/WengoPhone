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

#include "QtVideoSettings.h"

#include "ui_VideoSettings.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <webcam/WebcamDriver.h>
#include <sipwrapper/EnumVideoQuality.h>

#include <util/Logger.h>

#include <qtutil/StringListConvert.h>

#include <QtGui>

static const int VIDEO_QUALITY_COLUMN = 0;

QtVideoSettings::QtVideoSettings(QWidget * parent)
	: QtISettings(parent) {

	_videoSettingsWidget = new QWidget(parent);

	_ui = new Ui::VideoSettings();
	_ui->setupUi(_videoSettingsWidget);

	_webcamDriver = WebcamDriver::getInstance();
	_webcamDriver->frameCapturedEvent += boost::bind(&QtVideoSettings::frameCapturedEventHandler, this, _1, _2);
	_openedByMe = false;
	_rgbImage = NULL;

	connect(this, SIGNAL(newWebcamImage(QImage *)), SLOT(newWebcamImageCaptured(QImage *)), Qt::QueuedConnection);
	connect(_ui->webcamPreviewButton, SIGNAL(clicked()), SLOT(webcamPreviewButtonPressed()));

	_ui->webcamDeviceComboBox->addItems(StringListConvert::toQStringList(_webcamDriver->getDeviceList()));

	readConfig();
}

QtVideoSettings::~QtVideoSettings() {
	delete _ui;
}

QString QtVideoSettings::getName() const {
	return tr("Video");
}

void QtVideoSettings::saveConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set(Config::VIDEO_ENABLE_KEY, _ui->enableVideoGroupBox->isChecked());
	config.set(Config::VIDEO_WEBCAM_DEVICE_KEY, _ui->webcamDeviceComboBox->currentText().toStdString());

	QTreeWidgetItem * item = _ui->videoQualityTreeWidget->currentItem();
	int videoQuality;
	if (item) {
		if (tr("Normal") == item->text(VIDEO_QUALITY_COLUMN)) {
			videoQuality = EnumVideoQuality::VideoQualityNormal;
		}
		if (tr("Good") == item->text(VIDEO_QUALITY_COLUMN)) {
			videoQuality = EnumVideoQuality::VideoQualityGood;
		}
		if (tr("Very good") == item->text(VIDEO_QUALITY_COLUMN)) {
			videoQuality = EnumVideoQuality::VideoQualityVeryGood;
		}
		if (tr("Excellent") == item->text(VIDEO_QUALITY_COLUMN)) {
			videoQuality = EnumVideoQuality::VideoQualityExcellent;
		}
		else {
			LOG_FATAL("unknown video quality=" + item->text(VIDEO_QUALITY_COLUMN).toStdString());
		}
	}
	config.set(Config::VIDEO_QUALITY_KEY, videoQuality);
}

void QtVideoSettings::readConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_ui->enableVideoGroupBox->setChecked(config.getVideoEnable());
	_ui->webcamDeviceComboBox->setCurrentIndex(_ui->webcamDeviceComboBox->findText(QString::fromStdString(config.getVideoWebcamDevice())));

	int videoQuality = config.getVideoQuality();
	QString videoQualityText;

	switch (videoQuality) {
	case EnumVideoQuality::VideoQualityNormal:
		videoQualityText = tr("Normal");
		break;

	case EnumVideoQuality::VideoQualityGood:
		videoQualityText = tr("Good");
		break;

	case EnumVideoQuality::VideoQualityVeryGood:
		videoQualityText = tr("Very good");
		break;

	case EnumVideoQuality::VideoQualityExcellent:
		videoQualityText = tr("Excellent");
		break;

	default:
		LOG_FATAL("unknown video quality=" + String::fromNumber(videoQuality));
	}

	QList<QTreeWidgetItem *> items = _ui->videoQualityTreeWidget->findItems(videoQualityText, Qt::MatchExactly, VIDEO_QUALITY_COLUMN);
	if (items.size() > 0) {
		_ui->videoQualityTreeWidget->setItemSelected(items[0], true);
	}
}

void QtVideoSettings::frameCapturedEventHandler(IWebcamDriver * sender, piximage * image) {
	//TODO: optimize: free and alloc a new piximage only if the size changed
	if (_rgbImage) {
		pix_free(_rgbImage);
	}
	_rgbImage = pix_alloc(PIX_OSI_RGB32, image->width, image->height);

	pix_convert(PIX_NO_FLAG, _rgbImage, image);

	newWebcamImage(QPixmap::fromImage(QImage(_rgbImage->data, _rgbImage->width, _rgbImage->height, QImage::Format_RGB32)));
}

void QtVideoSettings::newWebcamImageCaptured(QPixmap pixmap) {
	_ui->webcamPreviewLabel->setPixmap(pixmap);
}

void QtVideoSettings::webcamPreviewButtonPressed() {
	//If the Webcam is currently in used, we cannot launch a capture and
	//we cannot set any parameters as it is already launched
	if (_ui->webcamDeviceComboBox->count() > 0 && !_webcamDriver->isOpened()) {
		_openedByMe = true;
		//_webcamDriver->setDevice(_webcamDeviceComboBox->currentText().toStdString());
		_webcamDriver->setDevice("");
		//_webcamDriver->setPalette(PIX_OSI_RGB32);

		_webcamDriver->startCapture();
	}
}

/*
void QtVideoSettings::hide() {
	_webcamDriver->frameCapturedEvent -= boost::bind(&QtVideoSettings::frameCapturedEventHandler, this, _1, _2);
	if (_openedByMe) {
		_webcamDriver->stopCapture();
	}
}
*/
