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

#include <timer/Timer.h>

#include <qtimer.h>

/**
 * Internal Timer.
 *
 * Uses QTimer and Qt slots/signals.
 *
 * @author Tanguy Krotoff
 */
class Timer::TimerPrivate : public QTimer {
	Q_OBJECT
public:

	/**
	 * Creates a new Timer.
	 *
	 * @param task the task to perform after timeout milliseconds
	 */
	TimerPrivate(TimerTask * task) {
		_nbRetry = 0;
		_task = task;
		connect(this, SIGNAL(timeout()),
			this, SLOT(doTask()));
	}

	/**
	 * Changes the number of shots/retries.
	 *
	 * Call start() after setting a new number of shots.
	 *
	 * @param nbShots number of limited shots, if 0 then no limit
	 */
	void setNbShots(unsigned int nbShots) {
		_nbShots = nbShots;
	}

	void singleShotNow() {
		_task->run();
	}

public slots:

	/**
	 * Executes the task.
	 */
	void doTask() {
		if (_nbShots == 0 || _nbRetry < _nbShots) {
			_nbRetry++;
			_task->run();
		} else {
			//Stops the timer, the task is not executed anymore
			stop();
		}
	}

private:

	/**
	 * Task to execute.
	 */
	TimerTask * _task;

	/**
	 * Current number of retry.
	 */
	unsigned int _nbRetry;

	/**
	 * Number of maximum shots.
	 */
	unsigned int _nbShots;
};


Timer::Timer(TimerTask * task) {
	_timer = new TimerPrivate(task);
}

Timer::~Timer() {
	stop();
	delete _timer;
}

void Timer::start(unsigned int timeout, unsigned int nbShots) {
	_timer->stop();
	_timer->setNbShots(nbShots);
	_timer->start(timeout);
}

void Timer::singleShotNow() {
	_timer->singleShotNow();
}

void Timer::stop() {
	_timer->stop();
}

//For the moc preprocessor
#include "Timer.moc"
