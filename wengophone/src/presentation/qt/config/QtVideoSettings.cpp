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

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>


#include "QtVideoSettings.h"


QtVideoSettings::QtVideoSettings( QWidget * parent, Qt::WFlags f ) : QWidget( parent, f ) {
	_widget = WidgetFactory::create( ":/forms/config/VideoSettings.ui", this );
	QGridLayout * layout = new QGridLayout();
	layout->addWidget( _widget );
	layout->setMargin( 0 );
	setLayout( layout );
	setupChilds();
	readConfigData();
}

void QtVideoSettings::setupChilds() {

	_webcamDeviceComboBox = Object::findChild<QComboBox *>(_widget,"webcamDeviceComboBox" );

	_webcamPreviewPushButton = Object::findChild<QPushButton *>(_widget,"webcamPreviewPushButton" );

	_webcamPreviewFrame = Object::findChild<QFrame *>(_widget,"webcamPreviewFrame" );

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

