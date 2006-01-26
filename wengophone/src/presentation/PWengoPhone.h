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

#ifndef PWENGOPHONE_H
#define PWENGOPHONE_H

#include "Presentation.h"

#include "model/WengoPhone.h"

#include <string>

class PPhoneLine;

/**
 *
 * @ingroup presentation
 * @author Tanguy Krotoff
 */
class PWengoPhone : public Presentation {
public:

	virtual ~PWengoPhone() {
	}

	virtual void addPhoneLine(PPhoneLine * pPhoneLine) = 0;

	virtual void wengoLoginStateChangedEvent(WengoPhone::LoginState state, const std::string & login, const std::string & password) = 0;

	/*virtual void addCommand(PCommand * command) = 0;

	virtual void addContactList(PContactList * pContactList) = 0;

	virtual void addHistory(PHistory * pHistory) = 0;

	virtual void addLogger(PLogger * pLogger) = 0;*/

protected:

	virtual void addPhoneLineThreadSafe(PPhoneLine * pPhoneLine) = 0;
};

#endif	//PWENGOPHONE_H
