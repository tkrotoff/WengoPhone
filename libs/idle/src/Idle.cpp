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

#include <util/Logger.h>

#include <QtCore/QtCore>
#include <QtGui/QCursor>

Idle::Idle(QObject * parent)
	: QObject(parent) {

	_idleMode = false;

	_timer = new QTimer(parent);
	_timer->setSingleShot(true);
	connect(_timer, SIGNAL(timeout()), SLOT(timeout()));

	//Check mouse position every second
	static const int CHECK_MOUSE_POS_TIMEOUT = 1000;

	_checkMousePosTimer = new QTimer(parent);
	_checkMousePosTimer->setSingleShot(false);
	_checkMousePosTimer->setInterval(CHECK_MOUSE_POS_TIMEOUT);
	connect(_checkMousePosTimer, SIGNAL(timeout()), SLOT(checkMousePos()));
	_checkMousePosTimer->start();
}

void Idle::setIntervalBeforeIdleStatus(unsigned interval) {
	_timer->setInterval(interval);
}

void Idle::start() {
	_originalMousePos = QCursor::pos();

	stop();
	_timer->start();
}

void Idle::stop() {
	_timer->stop();
}

void Idle::checkMousePos() {
	QPoint currentMousePos = QCursor::pos();
	if (currentMousePos != _originalMousePos) {
		if (_idleMode) {
			statusChangedEvent(*this, StatusActive);
			LOG_DEBUG("active state");
			_idleMode = false;
		}
		start();
	}
}

void Idle::timeout() {
	statusChangedEvent(*this, StatusIdle);
	LOG_DEBUG("idle state");
	_idleMode = true;
}
