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

#include "QtVolumeSlider.h"

#include <qtutil/ClickableLabel.h>

#include <QtGui/QtGui>

static const unsigned MINIMUM_VOLUME = 0;
static const unsigned MAXIMUM_VOLUME = 100;
static const unsigned MINIMUM_VOLUME_INDEX = 0;
static const unsigned MAXIMUM_VOLUME_INDEX = 6;
static const unsigned NB_VOLUME_STEPS = 16;

QtVolumeSlider::QtVolumeSlider(QObject * parent)
	: QObject(parent) {
}

void QtVolumeSlider::setVolume(int volume) {
	_volume = volume;
	updateLabels();
}

void QtVolumeSlider::addLabel(ClickableLabel * label, const QString & activePixmapFilename, const QString & pixmapFilename) {
	privateData data;

	data._label = label;
	data._activePixmapFilename = activePixmapFilename;
	data._pixmapFilename = pixmapFilename;

	connect(label, SIGNAL(clicked()), SLOT (labelClicked()));

	_dataList.append(data);
}

void QtVolumeSlider::labelClicked() {
	QObject * sender = this->sender();

	if (sender) {
		ClickableLabel * label = qobject_cast<ClickableLabel *>(sender);

		//Searches the index
		for (int i = 0; i < _dataList.size(); i++) {
			if (_dataList[i]._label->objectName() == label->objectName()) {
				//i is the label index now
				//MINIMUM_VOLUME_INDEX = volume 0% , MAXIMUM_VOLUME_INDEX = volume 100%
				if (i == MINIMUM_VOLUME_INDEX) {
					_volume = MINIMUM_VOLUME;
					updateLabels();
					volumeChanged(_volume);
					return;
				}
				if (i == MAXIMUM_VOLUME_INDEX) {
					_volume = MAXIMUM_VOLUME;
					updateLabels();
					volumeChanged(_volume);
				}
				_volume = i * NB_VOLUME_STEPS;
				updateLabels();
				volumeChanged(_volume);
				return;
			}
		}
	}
}

void QtVolumeSlider::updateLabels() {
	int pos;

	if (_volume == MINIMUM_VOLUME) {
		pos = MINIMUM_VOLUME_INDEX;
	} else if (_volume == MAXIMUM_VOLUME) {
		pos = MAXIMUM_VOLUME_INDEX;
	} else {
		pos = _volume / NB_VOLUME_STEPS;
	}

	QStringList pixNameList;

	switch (pos) {
	case MINIMUM_VOLUME_INDEX:
		//If pos == MINIMUM_VOLUME_INDEX, all pixmap are on "OFF"
		for (int i = 0; i < _dataList.size(); i++) {
			pixNameList << _dataList[i]._pixmapFilename;
		}
		break;

	case MAXIMUM_VOLUME_INDEX:
		//If pos == MAXIMUM_VOLUME_INDEX, all pixmap are on "ON"
		for (int i = 0; i < _dataList.size(); i++) {
			pixNameList << _dataList[i]._activePixmapFilename;
		}
		break;

	default: {
		int i = 0;
		for (i = 0; i <= pos; i++) {
			pixNameList << _dataList[i]._activePixmapFilename;
		}
		for (int j = i; j < _dataList.size(); j++) {
			pixNameList << _dataList[j]._pixmapFilename;
		}
	}
	}

	//Setup the label's pixmap
	for (int i = 0; i < _dataList.size(); i++) {
		_dataList[i]._label->setPixmap(pixNameList[i]);
		_dataList[i]._label->update();
	}
}
