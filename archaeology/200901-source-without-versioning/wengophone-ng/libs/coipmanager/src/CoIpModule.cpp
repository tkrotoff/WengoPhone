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

#include <coipmanager/CoIpModule.h>

CoIpModule::CoIpModule(CoIpManager & coIpManager)
	: Identifiable(),
	_coIpManager(coIpManager) {

	_started = false;
}

CoIpModule::CoIpModule(const CoIpModule & coIpModule)
	: QObject(),
	Identifiable(),
	_coIpManager(coIpModule._coIpManager) {

	_started = coIpModule._started;
}

CoIpModule::~CoIpModule() {
}

bool CoIpModule::isStarted() const {
	return _started;
}

void CoIpModule::setStarted(bool started) {
	_started = started;
}
