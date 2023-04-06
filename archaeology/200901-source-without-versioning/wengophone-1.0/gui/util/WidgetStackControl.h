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

#ifndef WIDGETSTACKCONTROL_H
#define WIDGETSTACKCONTROL_H

#include <qmap.h>
#include <qobject.h>

class QWidgetStack;
class QAction;
class QWidget;

/**
 * Utility class for QWidgetStack.
 *
 * Adds widgets to the stack and associates to them an action (a callback).
 *
 * @author Tanguy Krotoff
 */
class WidgetStackControl : public QObject {
	Q_OBJECT
public:

	/**
	 * Constructs the utility class using a QWidgetStack.
	 *
	 * @param stack QWidgetStack used by this class
	 */
	WidgetStackControl(QWidgetStack * stack);

	~WidgetStackControl();

	/**
	 * Associates an action to a widget and add the widget to the stack.
	 * Each time the action will be launched, the widget will be raised/showed by the stack.
	 *
	 * @param action action associated with the widget
	 * @param widget widget to add to the stack
	 */
	void addToStack(QAction * action, QWidget * widget);

private slots:

	/**
	 * Raises the current widget that is present in the stack.
	 */
	void raiseWidget();

private:

	WidgetStackControl(const WidgetStackControl &);
	WidgetStackControl & operator=(const WidgetStackControl &);

	/**
	 * Defines the map of QAction / QWidget.
	 */
	typedef QMap<QAction *, QWidget *> MapActionWidget;

	/**
	 * Maps a widget with an action.
	 */
	MapActionWidget _map;

	/**
	 * The QWidgetStack itself.
	 */
	QWidgetStack * _stack;
};

#endif	//WIDGETSTACKCONTROL_H
