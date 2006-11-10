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

#include "QtSVGDialpad.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <cutil/global.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/File.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>
#include <QtSvg/QtSvg>

//QRgb -> #AARRGGBB
static const unsigned COLOR_0 = 0xFF000000;
static const unsigned COLOR_1 = 0xFFFF0000;
static const unsigned COLOR_2 = 0xFF00FF00;
static const unsigned COLOR_3 = 0xFFFFFF00;
static const unsigned COLOR_4 = 0xFF0000FF;
static const unsigned COLOR_5 = 0xFFFF00FF;
static const unsigned COLOR_6 = 0xFF00FFFF;
static const unsigned COLOR_7 = 0xFF990000;
static const unsigned COLOR_8 = 0xFF99FF00;
static const unsigned COLOR_9 = 0xFF9900FF;
static const unsigned COLOR_POUND = 0xFF99FFFF;
static const unsigned COLOR_STAR = 0xFF999999;

QtSVGDialpad::QtSVGDialpad()
	: QWidget(NULL) {

	_svgMask = new QSvgRenderer();
	_mask = new QImage();

	_svgimage = new QSvgRenderer();

	_svgLayer = new QSvgRenderer();
	SAFE_CONNECT(_svgimage, SIGNAL(repaintNeeded()), SLOT(update()));

	setMouseTracking(true);
}

void QtSVGDialpad::setNewTheme(const std::string & themeRepertory) {

	//destroy old theme
	OWSAFE_DELETE(_svgMask);
	OWSAFE_DELETE(_svgimage);
	OWSAFE_DELETE(_svgLayer);
	OWSAFE_DELETE(_mask);

	//new repertory
	_repertory = themeRepertory;

	//new mask
	_svgMask = new QSvgRenderer(QString::fromStdString(_repertory + "dialpad_mask.svg"), this);
	_mask = new QImage();

	//new image
	_svgimage = new QSvgRenderer(QString::fromStdString(_repertory + "dialpad_image.svg"), this);

	//reinitialize layer
	_svgLayer = new QSvgRenderer();
}

QtSVGDialpad::~QtSVGDialpad() {
	OWSAFE_DELETE(_svgMask);
	OWSAFE_DELETE(_svgimage);
	OWSAFE_DELETE(_svgLayer);
	OWSAFE_DELETE(_mask);
}

void QtSVGDialpad::changeLayerFile(const std::string & newLayerFile) {

	if (newLayerFile != _layerFile) {

		//delete old layer
		OWSAFE_DELETE(_svgLayer);

		//construct new layer
		_svgLayer = new QSvgRenderer(QString::fromStdString(_repertory + newLayerFile), this);
		SAFE_CONNECT(_svgLayer, SIGNAL(repaintNeeded()), SLOT(update()));

		//save _layerFile
		_layerFile = newLayerFile;

		update();
	}
}

void QtSVGDialpad::mouseMoveEvent(QMouseEvent *event) {

	//get mouse pos
	QPoint pos = event->pos();

	//get pixel color
	unsigned color = _mask->pixel(pos.x(), pos.y());

	if (color == COLOR_0) {
		changeLayerFile("layer_0.svg");
	}
	else if (color == COLOR_1) {
		changeLayerFile("layer_1.svg");
	}
	else if (color == COLOR_2) {
		changeLayerFile("layer_2.svg");
	}
	else if (color == COLOR_3) {
		changeLayerFile("layer_3.svg");
	}
	else if (color == COLOR_4) {
		changeLayerFile("layer_4.svg");
	}
	else if (color == COLOR_5) {
		changeLayerFile("layer_5.svg");
	}
	else if (color == COLOR_6) {
		changeLayerFile("layer_6.svg");
	}
	else if (color == COLOR_7) {
		changeLayerFile("layer_7.svg");
	}
	else if (color == COLOR_8) {
		changeLayerFile("layer_8.svg");
	}
	else if (color == COLOR_9) {
		changeLayerFile("layer_9.svg");
	}
	else if (color == COLOR_POUND) {
		changeLayerFile("layer_pound.svg");
	}
	else if (color == COLOR_STAR) {
		changeLayerFile("layer_star.svg");
	}
	else {
		changeLayerFile("");
	}
}

void QtSVGDialpad::mouseReleaseEvent(QMouseEvent * event) {

	if (event->button() == Qt::LeftButton) {

		//get mouse pos
		QPoint pos = event->pos();

		//get pixel color
		unsigned color = _mask->pixel(pos.x(), pos.y());

		if (color == COLOR_0) {
			keyZeroSelected();
		}
		else if (color == COLOR_1) {
			keyOneSelected();
		}
		else if (color == COLOR_2) {
			keyTwoSelected();
		}
		else if (color == COLOR_3) {
			keyThreeSelected();
		}
		else if (color == COLOR_4) {
			keyFourSelected();
		}
		else if (color == COLOR_5) {
			keyFiveSelected();
		}
		else if (color == COLOR_6) {
			keySixSelected();
		}
		else if (color == COLOR_7) {
			keySevenSelected();
		}
		else if (color == COLOR_8) {
			keyEightSelected();
		}
		else if (color == COLOR_9) {
			keyNineSelected();
		}
		else if (color == COLOR_POUND) {
			keyPoundSelected();
		}
		else if (color == COLOR_STAR) {
			keyStarSelected();
		}
	}
}

void QtSVGDialpad::paintEvent(QPaintEvent *) {

	//mask
	if (_svgMask->isValid()) {
		if (_mask->size() != size()) {
			OWSAFE_DELETE(_mask);
			_mask = new QImage(size(), QImage::Format_ARGB32_Premultiplied);
			QPainter pm(_mask);
			pm.setViewport(0, 0, width(), height());
			pm.eraseRect(0, 0, width(), height());
			_svgMask->render(&pm);
		}
	}

	QPainter pi(this);
	pi.setViewport(0, 0, width(), height());

	//image
	if (_svgimage->isValid()) {
		_svgimage->render(&pi);
	}

	//layer
	if (_svgLayer->isValid()) {
		_svgLayer->render(&pi);
	}
}
