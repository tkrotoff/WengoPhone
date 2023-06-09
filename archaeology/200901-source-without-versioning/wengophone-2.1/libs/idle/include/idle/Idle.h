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

#ifndef OWIDLE_H
#define OWIDLE_H

#include <util/NonCopyable.h>
#include <util/Event.h>

#include <QtCore/QObject>
#include <QtCore/QPoint>

class QTimer;

/**
 * Detects idle mode.
 *
 * Catches Qt mouse and keyboard event to detect if we are in idle mode.
 *
 * @author Tanguy Krotoff
 */
class Idle : public QObject, NonCopyable {
	Q_OBJECT
public:

	enum Status {
		/** Idle mode. */
		StatusIdle,

		/** Active mode. */
		StatusActive
	};

	/**
	 * Status changed event.
	 *
	 * @param sender this class
	 * @param status new idle or active status
	 */
	Event<void (Idle & sender, Status status)> statusChangedEvent;

	Idle(QObject * parent);

	/**
	 * @param interval timeout time in seconds before considering
	 *        the computer is in idle mode
	 */
	void setIntervalBeforeIdleStatus(unsigned interval);

	/**
	 * Starts the idle detection.
	 */
	void start();

	/**
	 * Stops the idle detection.
	 */
	void stop();

private Q_SLOTS:

	void checkMousePos();

	void timeout();

private:

	/** Is in idle mode or active mode. */
	bool _idleMode;

	QTimer * _timer;

	QTimer * _checkMousePosTimer;

	QPoint _originalMousePos;
};

#endif	//OWIDLE_H
