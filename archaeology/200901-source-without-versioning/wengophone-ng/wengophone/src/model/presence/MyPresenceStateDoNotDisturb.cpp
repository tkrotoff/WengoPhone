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

#include "MyPresenceStateDoNotDisturb.h"

#include "MyPresenceStateAway.h"
#include "MyPresenceStateOnline.h"
#include "MyPresenceStateOffline.h"
#include "MyPresenceStateInvisible.h"
#include "MyPresenceStateException.h"

MyPresenceState * MyPresenceStateDoNotDisturb::offline() throw(MyPresenceStateException) {
	return MyPresenceStateOffline::getInstance();
}

MyPresenceState * MyPresenceStateDoNotDisturb::online() throw(MyPresenceStateException) {
	return MyPresenceStateOnline::getInstance();
}

MyPresenceState * MyPresenceStateDoNotDisturb::doNotDisturb() throw(MyPresenceStateException) {
	return this;
}

MyPresenceState * MyPresenceStateDoNotDisturb::away() throw(MyPresenceStateException) {
	return MyPresenceStateAway::getInstance();
}

MyPresenceState * MyPresenceStateDoNotDisturb::invisible() throw(MyPresenceStateException) {
	return MyPresenceStateInvisible::getInstance();
}
