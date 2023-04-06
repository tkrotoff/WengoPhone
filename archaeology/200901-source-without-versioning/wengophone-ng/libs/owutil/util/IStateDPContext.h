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

#ifndef OWISTATEDPCONTEXT_H
#define OWISTATEDPCONTEXT_H

#include <util/owutildll.h>

#include <QtCore/QMutex>

#include <map>

class IStateDPState;

/**
 * State Design Pattern implementation (StateDP).
 * Context part.
 *
 * @author Philippe Bernery
 */
class IStateDPContext {
public:

	/**
	 * Id used for getCurrentStateId.
	 */
	const int NO_STATE_ID;

	OWUTIL_API IStateDPContext();

	OWUTIL_API virtual ~IStateDPContext();

	/**	
	 * @return the id of the current state, NO_STATE_ID otherwise
	 */
	OWUTIL_API int getStateId() const;

	/**
	 * Sets the current state.
	 */
	OWUTIL_API virtual void setState(int stateId);

	/**
	 * Calls 'execute' on current state.
	 */
	OWUTIL_API void execute();

protected:

	/**
	 * Add a new state in the state map.
	 * Added states are automatically destroyed at Context destroy.
	 */
	OWUTIL_API void addState(int stateId, IStateDPState * state);

	IStateDPState *_currentState;

private:

	typedef std::map<int, IStateDPState *> StateMap;
	StateMap _stateMap;

	QMutex *_stateDPMutex;

};

#endif //OWISTATEDPCONTEXT_H
