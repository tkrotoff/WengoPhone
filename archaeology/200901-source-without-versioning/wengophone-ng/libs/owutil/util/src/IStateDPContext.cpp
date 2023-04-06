/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include <util/IStateDPContext.h>

#include <util/IStateDPState.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/String.h>

IStateDPContext::IStateDPContext()
	: NO_STATE_ID(-1) {
	_currentState = NULL;
	_stateDPMutex = new QMutex();
}

IStateDPContext::~IStateDPContext() {
	OWSAFE_DELETE(_stateDPMutex);

	for (StateMap::iterator it = _stateMap.begin();
		it != _stateMap.end();
		++it) {
		delete (*it).second;
	}
}

void IStateDPContext::addState(int stateId, IStateDPState * state) {
	if (state) {
		StateMap::iterator it = _stateMap.find(stateId);
		if (it != _stateMap.end()) {
			LOG_FATAL("id already in map. Cannot add this state.");
			return;
		}

		_stateMap[stateId] = state;
	}
}

int IStateDPContext::getStateId() const {
	int result = NO_STATE_ID;

	if (_currentState) {
		for (StateMap::const_iterator it = _stateMap.begin();
			it != _stateMap.end();
			++it) {
			if ((*it).second == _currentState) {
				result = (*it).first;
				break;
			}
		}
	}

	return result;
}

void IStateDPContext::setState(int stateId) {
	QMutexLocker lock(_stateDPMutex);

	StateMap::iterator it = _stateMap.find(stateId);
	if (it == _stateMap.end()) {
		LOG_FATAL("id not in map. Cannot set this state.");
		return;
	}

	LOG_DEBUG("switch state to " + String::fromNumber(stateId));
	_currentState = (*it).second;
}

void IStateDPContext::execute() {
	_stateDPMutex->lock();
	IStateDPState * state = _currentState;
	_stateDPMutex->unlock();

	if (state) {
		state->execute();
	}
}
