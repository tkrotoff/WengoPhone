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

#ifndef OWWIDGETBACKGROUNDIMAGE_H
#define OWWIDGETBACKGROUNDIMAGE_H

#include <qtutil/owqtutildll.h>

#include <util/NonCopyable.h>

class QWidget;

/**
 * Replacement for QWidget::setBackgroundPixmap() that does not exist in Qt4 anymore.
 *
 * Draws a background image inside a QWidget.
 *
 * @author Tanguy Krotoff
 */
class WidgetBackgroundImage : NonCopyable {
public:

	/**
	 * Sets a background image to a QWidget.
	 *
	 * @param widget QWidget that will get the background image
	 * @param imageFile background image filename
	 * @param resizeWidget true if widget should be resize to fit the image; false otherwise
	 */
	OWQTUTIL_API static void setBackgroundImage(QWidget * widget, const char * imageFile, bool resizeWidget);
};

#endif	//OWWIDGETBACKGROUNDIMAGE_H
