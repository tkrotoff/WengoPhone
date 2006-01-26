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

#ifndef MYPRESENCESTATEUSERDEFINED_H
#define MYPRESENCESTATEUSERDEFINED_H

#include "MyPresenceState.h"

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class MyPresenceStateUserDefined : public MyPresenceState {
public:

	MyPresenceState * online() throw(MyPresenceStateException);

	MyPresenceState * offline() throw(MyPresenceStateException);

	MyPresenceState * doNotDisturb() throw(MyPresenceStateException);

	MyPresenceState * away() throw(MyPresenceStateException);
	
	MyPresenceState * userDefined() throw(MyPresenceStateException);

	std::string toString() const {
		return toString_static();
	}

	static MyPresenceState * getInstance() {
		static MyPresenceStateUserDefined presenceState;
		
		return &presenceState;
	}

private:

	MyPresenceStateUserDefined() {
	}

	static std::string toString_static() {
		return "UserDefined";
	}
};

#endif	//MYPRESENCESTATEUSERDEFINED_H
