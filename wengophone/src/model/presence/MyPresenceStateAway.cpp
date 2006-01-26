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

#include "MyPresenceStateAway.h"
#include "MyPresenceStateOnline.h"
#include "MyPresenceStateOffline.h"
#include "MyPresenceStateDoNotDisturb.h"
#include "MyPresenceStateUserDefined.h"
#include "MyPresenceStateException.h"

MyPresenceState * MyPresenceStateAway::offline() throw(MyPresenceStateException) {
	return MyPresenceStateOffline::getInstance();
}

MyPresenceState * MyPresenceStateAway::online() throw(MyPresenceStateException) {
	return MyPresenceStateOnline::getInstance();
}

MyPresenceState * MyPresenceStateAway::doNotDisturb() throw(MyPresenceStateException) {
	return MyPresenceStateDoNotDisturb::getInstance();
}

MyPresenceState * MyPresenceStateAway::away() throw(MyPresenceStateException) {
	return this;
}

MyPresenceState * MyPresenceStateAway::userDefined() throw(MyPresenceStateException) {
	return MyPresenceStateUserDefined::getInstance();
}
