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

#ifndef OWQTSVGDIALPAD_H
#define OWQTSVGDIALPAD_H

#include <util/NonCopyable.h>

#include <QtGui/QWidget>
#include <QtSvg/QtSvg>

//QRgb -> #AARRGGBB
#define COLOR_0		0xFF000000
#define COLOR_1		0xFFFF0000
#define COLOR_2		0xFF00FF00
#define COLOR_3		0xFFFFFF00
#define COLOR_4		0xFF0000FF
#define COLOR_5		0xFFFF00FF
#define COLOR_6		0xFF00FFFF
#define COLOR_7		0xFF990000
#define COLOR_8		0xFF99FF00
#define COLOR_9		0xFF9900FF
#define COLOR_POUND	0xFF99FFFF
#define COLOR_STAR	0xFF999999

/**
 * SVG Dialpad widget
 *
 * @author Xavier Desjardins
 */
class QtSVGDialpad : public QWidget, NonCopyable {

	Q_OBJECT

public:

	QtSVGDialpad();

	~QtSVGDialpad();

	void setNewTheme(const std::string themeRepertory);

Q_SIGNALS:

	void keyZeroSelected();

	void keyOneSelected();

	void keyTwoSelected();

	void keyThreeSelected();

	void keyFourSelected();

	void keyFiveSelected();

	void keySixSelected();

	void keySevenSelected();

	void keyEightSelected();

	void keyNineSelected();	

	void keyStarSelected();

	void keyPoundSelected();

private:

	inline void changeLayerFile( const std::string newLayerFile );

	void paintEvent(QPaintEvent *event);

	void mouseReleaseEvent(QMouseEvent *event);

	void mouseMoveEvent(QMouseEvent *event);

	QImage *_mask;

	QSvgRenderer * _svgMask;

	QSvgRenderer * _svgLayer;

	QSvgRenderer * _svgimage;

	std::string _repertory;

	std::string _layerFile;


};

#endif	//OWQTSVGDIALPAD_H
