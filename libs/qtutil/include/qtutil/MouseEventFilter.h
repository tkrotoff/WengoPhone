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

#ifndef OWMOUSEEVENTFILTER_H
#define OWMOUSEEVENTFILTER_H

#include <qtutil/EventFilter.h>

/**
 * Catch MouseMove event.
 *
 * @author Tanguy Krotoff
 */
class MouseMoveEventFilter : public EventFilter {
public:

	OWQTUTIL_API MouseMoveEventFilter(QObject * receiver, const char * member);

private:

	virtual bool eventFilter(QObject * watched, QEvent * event);
};


/**
 * Catch MouseButtonPress event.
 *
 * @author Tanguy Krotoff
 */
class MousePressEventFilter : public EventFilter {
public:

	OWQTUTIL_API MousePressEventFilter(QObject * receiver, const char * member, Qt::MouseButton button = Qt::NoButton);

private:

	virtual bool eventFilter(QObject * watched, QEvent * event);

	Qt::MouseButton _button;
};


/**
 * Catch MouseButtonRelease event.
 *
 * @author Tanguy Krotoff
 */
class MouseReleaseEventFilter : public EventFilter {
public:

	OWQTUTIL_API MouseReleaseEventFilter(QObject * receiver, const char * member, Qt::MouseButton button = Qt::NoButton);

private:

	virtual bool eventFilter(QObject * watched, QEvent * event);

	Qt::MouseButton _button;
};


/**
 * Catch HoverEnter event.
 *
 * @author Tanguy Krotoff
 */
class MouseHoverEnterEventFilter : public EventFilter {
public:

	OWQTUTIL_API MouseHoverEnterEventFilter(QObject * receiver, const char * member);

private:

	virtual bool eventFilter(QObject * watched, QEvent * event);
};


/**
 * Catch HoverLeave event.
 *
 * @author Tanguy Krotoff
 */
class MouseHoverLeaveEventFilter : public EventFilter {
public:

	OWQTUTIL_API MouseHoverLeaveEventFilter(QObject * receiver, const char * member);

private:

	virtual bool eventFilter(QObject * watched, QEvent * event);
};

#endif	//OWMOUSEEVENTFILTER_H
