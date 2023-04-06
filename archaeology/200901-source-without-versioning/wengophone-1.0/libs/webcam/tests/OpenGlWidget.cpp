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

#include "OpenGlWidget.h"

#if defined(Q_CC_MSVC)
#pragma warning(disable:4305) // init: truncation from const double to float
#endif

#include <qapplication.h>

#include <stdio.h>


class OglPaintEvent : public QCustomEvent {
	public:
		enum Type { OglPaint = (QEvent::User + 205) };
		OglPaintEvent(Type t, QImage i) : QCustomEvent(t), img(i) {
		}
		~OglPaintEvent() {}
		QImage img;
};

OpenGlWidget::OpenGlWidget( QWidget* parent, const char* name, const QGLWidget* shareWidget )
	: QGLWidget( parent, name, shareWidget ) {
}

OpenGlWidget::~OpenGlWidget(){
}

void OpenGlWidget::initializeGL() {
	// Let OpenGL clear to black by default
	qglClearColor(black);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glDisable(GL_LOGIC_OP);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_TEXTURE_2D);
}

void OpenGlWidget::resizeGL( int w, int h ) {
	float wRatio = (float)w / 176;
	float hRatio = (float)h / 144;
	glPixelZoom(wRatio, hRatio);
}

void OpenGlWidget::paintGL() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode( GL_MODELVIEW );
	glDrawPixels(_gldata.width(), _gldata.height(), GL_RGBA, GL_UNSIGNED_BYTE, _gldata.bits());
}

void OpenGlWidget::updateBuffer(QImage i) {
	QApplication::postEvent(this, new OglPaintEvent(OglPaintEvent::OglPaint, i));
}

void OpenGlWidget::customEvent(QCustomEvent *e) {
	OglPaintEvent * ev = (OglPaintEvent*)e;
	_gldata = QGLWidget::convertToGLFormat(ev->img);
	glDraw();
}
