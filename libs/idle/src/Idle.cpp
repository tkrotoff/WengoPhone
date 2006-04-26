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

#include <idle/Idle.h>

#include "MouseOrKeyboardEventFilter.h"

#include <QtGui>

Idle::Idle(unsigned timeout, QObject * parent)
	: QObject(parent) {

	_idleMode = false;

	MouseOrKeyboardEventFilter * mouseOrKeyboardEventFilter = new MouseOrKeyboardEventFilter(this, SLOT(mouseOrKeyboardEvent()));
	QDesktopWidget * desktop = QApplication::desktop();
	desktop->installEventFilter(mouseOrKeyboardEventFilter);

	_timer = new QTimer(parent);
	_timer->setInterval(timeout);
	_timer->setSingleShot(true);
	connect(_timer, SIGNAL(timeout()), SLOT(timeout()));
}

void Idle::start() {
	stop();
	_timer->start();
}

void Idle::stop() {
	_timer->stop();
}

void Idle::mouseOrKeyboardEvent() {
	if (_idleMode) {
		idleEvent(*this, StatusActive);
	}

	_idleMode = false;

	//Restart timer
	start();
}

void Idle::timeout() {
	_idleMode = true;
	idleEvent(*this, StatusIdle);
}
