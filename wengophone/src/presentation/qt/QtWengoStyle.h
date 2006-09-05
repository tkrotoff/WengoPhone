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

#ifndef OWQTWENGOSTYLE_H
#define OWQTWENGOSTYLE_H

#include <cutil/global.h>

#ifdef OS_WINDOWS
	#include <QtGui/QWindowsXPStyle>
	class SystemStyle : public QWindowsXPStyle {};
#elif defined(OS_MACOSX)
	#include <QtGui/QMacStyle>
	class SystemStyle : public QMacStyle {};
#else
	#include <QtGui/QPlastiqueStyle>
	class SystemStyle : public QPlastiqueStyle {};
#endif

/**
 * General Qt style for correcting some bugs or ugly style.
 *
 * - Removes the ugly toolbar bottom line
 * - Changes QToolButton style under MacOSX
 * - Removes the ugly frame/marging around the status bar icons under Windows
 *
 * @author Tanguy Krotoff
 */
class QtWengoStyle : public SystemStyle {
public:

	QtWengoStyle();

	~QtWengoStyle();

	void drawComplexControl(ComplexControl control, const QStyleOptionComplex * option,
				QPainter * painter, const QWidget * widget = 0) const;

	void drawControl(ControlElement element, const QStyleOption * option,
				QPainter * painter, const QWidget * widget = 0) const;

	void drawPrimitive(PrimitiveElement element, const QStyleOption * option,
				QPainter * painter, const QWidget * widget = 0) const;
};

#endif	//OWQTWENGOSTYLE_H
