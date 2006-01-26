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

#include "MyPresenceStateOnline.h"

#include "MyPresenceStateAway.h"
#include "MyPresenceStateOffline.h"
#include "MyPresenceStateDoNotDisturb.h"
#include "MyPresenceStateUserDefined.h"
#include "MyPresenceStateException.h"

MyPresenceState * MyPresenceStateOnline::offline() throw(MyPresenceStateException) {
	return MyPresenceStateOffline::getInstance();
}

MyPresenceState * MyPresenceStateOnline::online() throw(MyPresenceStateException) {
	return this;
}

MyPresenceState * MyPresenceStateOnline::doNotDisturb() throw(MyPresenceStateException) {
	return MyPresenceStateDoNotDisturb::getInstance();
}

MyPresenceState * MyPresenceStateOnline::away() throw(MyPresenceStateException) {
	return MyPresenceStateAway::getInstance();
}

MyPresenceState * MyPresenceStateOnline::userDefined() throw(MyPresenceStateException) {
	return MyPresenceStateUserDefined::getInstance();
}
