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

#ifndef OWTOOLBARSTYLE_H
#define OWTOOLBARSTYLE_H

#include <QtGui/QCommonStyle>

class ControlElement;
class QStyleOption;
class QPainter;
class QWidget;

/**
 * Removes the ugly toolbar bottom line.
 *
 * @author Tanguy Krotoff
 */
class ToolBarStyle : public QCommonStyle {
	Q_OBJECT
public:

	ToolBarStyle();

	~ToolBarStyle();

	void drawControl(ControlElement element, const QStyleOption * option,
			QPainter * painter, const QWidget * widget = 0) const;
};

#endif	//OWTOOLBARSTYLE_H
