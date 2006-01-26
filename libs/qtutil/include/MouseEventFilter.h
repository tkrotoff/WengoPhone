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

#ifndef MOUSEEVENTFILTER_H
#define MOUSEEVENTFILTER_H

#include <EventFilter.h>

/**
 * Catch MouseMove.
 *
 * @author Tanguy Krotoff
 */
class MouseMoveEventFilter : public EventFilter {
public:

	MouseMoveEventFilter(QObject * receiver, const char * member);

protected:

	virtual bool eventFilter(QObject * watched, QEvent * event);
};


/**
 * Catch MouseButtonPress.
 *
 * @author Tanguy Krotoff
 */
class MousePressEventFilter : public EventFilter {
public:

	MousePressEventFilter(QObject * receiver, const char * member);

protected:

	virtual bool eventFilter(QObject * watched, QEvent * event);
};


/**
 * Catch MouseButtonRelease.
 *
 * @author Tanguy Krotoff
 */
class MouseReleaseEventFilter : public EventFilter {
public:

	MouseReleaseEventFilter(QObject * receiver, const char * member);

protected:

	virtual bool eventFilter(QObject * watched, QEvent * event);
};


/**
 * Catch HoverEnter.
 *
 * @author Tanguy Krotoff
 */
class MouseHoverEnterEventFilter : public EventFilter {
public:

	MouseHoverEnterEventFilter(QObject * receiver, const char * member);

protected:

	virtual bool eventFilter(QObject * watched, QEvent * event);
};


/**
 * Catch HoverLeave.
 *
 * @author Tanguy Krotoff
 */
class MouseHoverLeaveEventFilter : public EventFilter {
public:

	MouseHoverLeaveEventFilter(QObject * receiver, const char * member);

protected:

	virtual bool eventFilter(QObject * watched, QEvent * event);
};

#endif	//MOUSEEVENTFILTER_H
