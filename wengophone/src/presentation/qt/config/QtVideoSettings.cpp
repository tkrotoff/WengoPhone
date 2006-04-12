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

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <webcam/WebcamDriver.h>

#include <qtutil/Object.h>
#include <qtutil/StringListConvert.h>
#include <qtutil/WidgetFactory.h>

#include "QtVideoSettings.h"


QtVideoSettings::QtVideoSettings( QWidget * parent, Qt::WFlags f ) : QWidget( parent, f ) {
	_widget = WidgetFactory::create( ":/forms/config/VideoSettings.ui", this );

	_webcamDriver = WebcamDriver::getInstance();
	_webcamDriver->frameCapturedEvent +=
		boost::bind(&QtVideoSettings::frameCapturedEventHandler, this, _1, _2);
	_openedByMe = false;
	_rgbImage = NULL;

	QGridLayout * layout = new QGridLayout();
	layout->addWidget(_widget);
	layout->setMargin(0);
	setLayout(layout);

	setupChilds();

	readConfigData();

	connect(this, SIGNAL(newWebcamImage(QImage *)), SLOT(newWebcamImageCaptured(QImage *)), Qt::QueuedConnection);
	connect(_webcamPreviewPushButton, SIGNAL(clicked()), SLOT(webcamPreviewButtonPressed()));
}

void QtVideoSettings::setupChilds() {
	_webcamDeviceComboBox = Object::findChild<QComboBox *>(_widget,"webcamDeviceComboBox" );
	_webcamDeviceComboBox->addItems(StringListConvert::toQStringList(_webcamDriver->getDeviceList()));
	
	_webcamPreviewPushButton = Object::findChild<QPushButton *>(_widget,"webcamPreviewPushButton" );

	_webcamPreviewLabel = Object::findChild<QLabel *>(_widget,"webcamPreviewLabel" );

	_videoQualityTreeWidget = Object::findChild<QTreeWidget *>(_widget,"videoQualityTreeWidget" );

	_makeTestVideoCallPushButton = Object::findChild<QPushButton *>(_widget,"makeTestVideoCallPushButton" );

	_videoEnabled = Object::findChild<QGroupBox *> (_widget,"enableVideoGroupBox");
}

void QtVideoSettings::saveData(){
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	config.set( config.VIDEO_ENABLE_KEY , _videoEnabled->isChecked() );

	config.set( config.VIDEO_WEBCAM_DEVICE_KEY, _webcamDeviceComboBox->currentText().toStdString() );

	QTreeWidgetItem * item = _videoQualityTreeWidget->currentItem ();
	if (item)
	{
		if ( tr("Normale") == item->text(0))
			config.set ( config.VIDEO_QUALITY_KEY, std::string("normale") );
		if ( tr("Good") == item->text(0))
			config.set ( config.VIDEO_QUALITY_KEY, std::string("good") );
		if ( tr("Very good") == item->text(0))
			config.set ( config.VIDEO_QUALITY_KEY, std::string("very_good") );
		if ( tr("Excellent") == item->text(0))
			config.set ( config.VIDEO_QUALITY_KEY, std::string("excellent") );
	}
}

void QtVideoSettings::readConfigData(){

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_videoEnabled->setChecked( config.getVideoEnable() );

	_webcamDeviceComboBox->setCurrentIndex (_webcamDeviceComboBox->findText (QString::fromStdString(config.getVideoWebCamDevice())));

	QString text = QString::fromStdString(config.VIDEO_QUALITY_KEY);

	if ( text == "normale" )
		text = tr ("Normale");
	if ( text == "good" )
		text = tr ("Good");
	if ( text == "very_good" )
		text = tr ("Very good");
	if ( text == "excellent" )
		text = tr ("Excellent");

	QList<QTreeWidgetItem *> items = _videoQualityTreeWidget->findItems( text, Qt::MatchExactly,0) ;
	if ( items.size() > 0)
		_videoQualityTreeWidget->setItemSelected(items[0],true);

}

void QtVideoSettings::frameCapturedEventHandler(IWebcamDriver *sender, piximage *image) {
	//TODO: optimize: free and alloc a new piximage only if the size changed
	if (_rgbImage) {
		pix_free(_rgbImage);
	}
	_rgbImage = pix_alloc(PIX_OSI_RGB32, image->width, image->height);

	pix_convert(PIX_NO_FLAG, _rgbImage, image);

	newWebcamImage(QPixmap::fromImage(QImage(_rgbImage->data, _rgbImage->width, _rgbImage->height, QImage::Format_RGB32)));
}

void QtVideoSettings::newWebcamImageCaptured(QPixmap pixmap) {
	_webcamPreviewLabel->setPixmap(pixmap);
}

void QtVideoSettings::webcamPreviewButtonPressed() {
	// If the Webcam is currently in used, we cannot launch a capture and
	// we cannot set any parameters as it is already launched
	if (_webcamDeviceComboBox->count() > 0 && !_webcamDriver->isOpened()) {
		_openedByMe = true;
		//_webcamDriver->setDevice(_webcamDeviceComboBox->currentText().toStdString());
		_webcamDriver->setDevice("");
		//_webcamDriver->setPalette(PIX_OSI_RGB32);

		_webcamDriver->startCapture();
	}
}

void QtVideoSettings::widgetHidden() {
	/*FIXME: we should unregister ourself from the event
	_webcamDriver->frameCapturedEvent -=
		boost::bind(&QtVideoSettings::frameCapturedEventHandler, this, _1, _2);	
	*/
	if (_openedByMe) {
		_webcamDriver->stopCapture();
	}
}
