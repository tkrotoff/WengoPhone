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

#include "WidgetStackControl.h"

#include <qwidgetstack.h>
#include <qaction.h>
#include <qwidget.h>

#include <iostream>
using namespace std;

WidgetStackControl::WidgetStackControl(QWidgetStack * stack)
: _stack(stack) {
}

WidgetStackControl::~WidgetStackControl() {
	for (MapActionWidget::Iterator it = _map.begin(); it != _map.end(); ++it) {
		delete it.key();
		delete it.data();
	}
}

void WidgetStackControl::addToStack(QAction * action, QWidget * widget) {
	_stack->addWidget(widget);

	//send a signal for showing a default widget inside QWidgetStack
	//the first widget inserted into QWidgetStack is then raised/showed
	if (_map.empty()) {
		emit _stack->raiseWidget(widget);
	}

	_map[action] = widget;
	if (action) {
		QObject::connect(action, SIGNAL(activated()), this, SLOT(raiseWidget()));
	}
}

void WidgetStackControl::raiseWidget() {
	emit _stack->raiseWidget(_map[(QAction *) sender()]);
}
