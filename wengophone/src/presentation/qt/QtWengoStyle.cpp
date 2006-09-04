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

#include "QtWengoStyle.h"

#include <QtGui/QtGui>

QtWengoStyle::QtWengoStyle() {
}

QtWengoStyle::~QtWengoStyle() {
}

void QtWengoStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex * option,
		QPainter * painter, const QWidget * widget) const {

#if defined(OS_MACOSX)
	//QToolButton
	//Changes QToolButton style under MacOSX
	if (control == CC_ToolButton) {
		QCommonStyle::drawComplexControl(control, option, painter, widget);
		return;
	}
#endif

	SystemStyle::drawComplexControl(control, option, painter, widget);
}

void QtWengoStyle::drawControl(ControlElement element, const QStyleOption * option,
		QPainter * painter, const QWidget * widget) const {

	//QToolbar
	//Removes the ugly toolbar bottom line
	if (element == CE_ToolBar) {
		return;
	}

	SystemStyle::drawControl(element, option, painter, widget);
}

void QtWengoStyle::drawPrimitive(PrimitiveElement element, const QStyleOption * option,
		QPainter * painter, const QWidget * widget) const {

	//QStatusBar
	//Removes the ugly frame/marging around the status bar icons under Windows
	if (element == PE_FrameStatusBar) {
		return;
	}

	SystemStyle::drawPrimitive(element, option, painter, widget);
}
