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
#include "MyPresenceStateInvisible.h"

MyPresenceState * MyPresenceStateOnline::offline() {
	return MyPresenceStateOffline::getInstance();
}

MyPresenceState * MyPresenceStateOnline::online() {
	return this;
}

MyPresenceState * MyPresenceStateOnline::doNotDisturb() {
	return MyPresenceStateDoNotDisturb::getInstance();
}

MyPresenceState * MyPresenceStateOnline::away() {
	return MyPresenceStateAway::getInstance();
}

MyPresenceState * MyPresenceStateOnline::invisible() {
	return MyPresenceStateInvisible::getInstance();
}
