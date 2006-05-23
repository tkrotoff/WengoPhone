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

#ifndef QTSTATUSBARSTYLE_H
#define QTSTATUSBARSTYLE_H

#include <QCommonStyle>

class PrimitiveElement;
class QStyleOption;
class QPainter;
class QWidget;

/**
 * Removes the ugly frame/marging around the status bar icons.
 *
 * The ugly frame is present under Windows XP with classic style and
 * under Linux. (it should be present under all platforms, only
 * Windows XP with XP style does not have this problem.)
 */
class QtStatusBarStyle : public QCommonStyle {
	Q_OBJECT
public:

	QtStatusBarStyle();

	~QtStatusBarStyle();

	virtual void drawPrimitive(PrimitiveElement elem, const QStyleOption * option,
			QPainter * painter, const QWidget * widget = 0) const;
};

#endif	//QTSTATUSBARSTYLE_H
