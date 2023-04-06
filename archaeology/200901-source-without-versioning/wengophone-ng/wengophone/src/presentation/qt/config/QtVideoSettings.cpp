/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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
#include <model/profile/UserProfile.h>
#include <model/phoneline/IPhoneLine.h>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/phonecall/QtVideoQt.h>

#include <webcam/WebcamDriver.h>
#include <sipwrapper/EnumVideoQuality.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <qtutil/StringListConvert.h>
#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

static const int VIDEO_QUALITY_COLUMN = 0;
static const char * VIDEO_TEST_CALL = "335";

QtVideoSettings::QtVideoSettings(CWengoPhone & cWengoPhone, QWidget * parent)
	: QWidget(NULL),
	_cWengoPhone(cWengoPhone) {

	_ui = new Ui::VideoSettings();
	_ui->setupUi(this);

	_webcamDriver = WebcamDriver::getInstance();
	_previewStarted = false;

	SAFE_CONNECT_TYPE(this, SIGNAL(newWebcamImage()), SLOT(newWebcamImageCaptured()), Qt::QueuedConnection);
	SAFE_CONNECT(_ui->webcamDeviceComboBox, SIGNAL(activated(const QString &)), SLOT(startWebcamPreview(const QString &)));
	SAFE_CONNECT(_ui->makeTestVideoCallButton, SIGNAL(clicked()), SLOT(makeTestCallClicked()));
	SAFE_CONNECT(_ui->webcamPreviewButton, SIGNAL(clicked()), SLOT(webcamPreview()));
	
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	//No webcam driver
	if (_webcamDriver->getDeviceList().empty()) {
		config.set(Config::VIDEO_ENABLE_KEY, false);
		_ui->enableVideoCheckBox->setChecked(false);
		_ui->enableVideoCheckBox->setEnabled(false);
	}

	// set all columns to the same size
	_ui->videoQualityTreeWidget->header()->setResizeMode(2, QHeaderView::Stretch);
	_ui->videoQualityTreeWidget->header()->setResizeMode(1, QHeaderView::Stretch);
	_ui->videoQualityTreeWidget->header()->setResizeMode(0, QHeaderView::Stretch);
	////

	readConfig();
}

QtVideoSettings::~QtVideoSettings() {
	OWSAFE_DELETE(_ui);
}

QString QtVideoSettings::getName() const {
	return tr("Video");
}

QString QtVideoSettings::getTitle() const {
	return tr("Video Settings");
}

QString QtVideoSettings::getIconName() const {
	return "video";
}

void QtVideoSettings::saveConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set(Config::VIDEO_ENABLE_KEY, _ui->enableVideoCheckBox->isChecked());
	config.set(Config::VIDEO_WEBCAM_DEVICE_KEY, std::string(_ui->webcamDeviceComboBox->currentText().toUtf8().constData()));

	QTreeWidgetItem * item = _ui->videoQualityTreeWidget->currentItem();
	if (item) {
		EnumVideoQuality::VideoQuality videoQuality = EnumVideoQuality::VideoQualityNormal;
		QString text = item->text(VIDEO_QUALITY_COLUMN);
		if (tr("Normal") == text) {
			videoQuality = EnumVideoQuality::VideoQualityNormal;
		}
		else if (tr("Good") == text) {
			videoQuality = EnumVideoQuality::VideoQualityGood;
		}
		else if (tr("Very good") == text) {
			videoQuality = EnumVideoQuality::VideoQualityVeryGood;
		}
		else if (tr("Excellent") == text) {
			videoQuality = EnumVideoQuality::VideoQualityExcellent;
		}
		else {
			LOG_FATAL("unknown video quality=" + text.toStdString());
		}
		config.set(Config::VIDEO_QUALITY_KEY, EnumVideoQuality::toString(videoQuality));
	}
}

void QtVideoSettings::readConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_ui->enableVideoCheckBox->setChecked(config.getVideoEnable());

	_ui->webcamDeviceComboBox->clear();
	_ui->webcamDeviceComboBox->addItems(StringListConvert::toQStringList(_webcamDriver->getDeviceList()));
	_ui->webcamDeviceComboBox->setCurrentIndex(_ui->webcamDeviceComboBox->findText(QString::fromUtf8(config.getVideoWebcamDevice().c_str())));

	EnumVideoQuality::VideoQuality videoQuality = EnumVideoQuality::toVideoQuality(config.getVideoQuality());
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
	Mutex::ScopedLock lock(_mutex);

	if (!_ui->webcamSelectionGroupBox->isEnabled()) {
		return;
	}

	QtVideoQt::convertPixImageToQImage(image, QSize(_ui->webcamPreviewLabel->width(), _ui->webcamPreviewLabel->height()), &_rgbImage);
	_lastWebcamPixmap = QPixmap::fromImage(_rgbImage);

	newWebcamImage();
}

void QtVideoSettings::newWebcamImageCaptured() {
	Mutex::ScopedLock lock(_mutex);

	_ui->webcamPreviewLabel->setPixmap(_lastWebcamPixmap);
}

void QtVideoSettings::startWebcamPreview(const QString & deviceName) {
	if (!_previewStarted) {
		_webcamDriver->frameCapturedEvent += boost::bind(&QtVideoSettings::frameCapturedEventHandler, this, _1, _2);
		_webcamDriver->setDevice(deviceName.toStdString());
		_webcamDriver->setResolution(320, 240);
		_webcamDriver->setPalette(PIX_OSI_YUV420P);
		_webcamDriver->startCapture();

		_previewStarted = true;
	}
}

void QtVideoSettings::stopWebcamPreview() {
	if (_previewStarted) {
		_webcamDriver->frameCapturedEvent -= boost::bind(&QtVideoSettings::frameCapturedEventHandler, this, _1, _2);
		_webcamDriver->stopCapture();

		_previewStarted = false;
	}
}

void QtVideoSettings::webcamPreview() {
	startWebcamPreview(_ui->webcamDeviceComboBox->currentText());
}

void QtVideoSettings::hideEvent(QHideEvent * event) {
	stopWebcamPreview();
}

void QtVideoSettings::makeTestCallClicked() {
	CUserProfile * cUserProfile = _cWengoPhone.getCUserProfileHandler().getCUserProfile();
	if ((cUserProfile) && (cUserProfile->getUserProfile().getActivePhoneLine())) {
		cUserProfile->getUserProfile().getActivePhoneLine()->makeCall(VIDEO_TEST_CALL);
		QtWengoPhone* qWengoPhone = static_cast<QtWengoPhone*>(_cWengoPhone.getPresentation());
		qWengoPhone->getWidget()->raise();
	}
}
