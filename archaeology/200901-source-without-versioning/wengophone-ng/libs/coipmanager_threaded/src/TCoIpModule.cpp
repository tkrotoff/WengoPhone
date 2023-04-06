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

#include <coipmanager_threaded/TCoIpModule.h>

#include <coipmanager_threaded/TCoIpManager.h>

#include <coipmanager/CoIpModule.h>

#include <util/PostEvent.h>
#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

TCoIpModule::TCoIpModule(TCoIpManager & tCoIpManager, CoIpModule * module)
	: _tCoIpManager(tCoIpManager) {

	_module = module;

	SAFE_CONNECT(_module, SIGNAL(moduleFinishedSignal()),
		SIGNAL(moduleFinishedSignal()));
}

TCoIpModule::~TCoIpModule() {
	OWSAFE_DELETE(_module);
}

void TCoIpModule::start() {
	PostEvent::invokeMethod(_module, "start");
}

void TCoIpModule::pause() {
	PostEvent::invokeMethod(_module, "pause");
}

void TCoIpModule::resume() {
	PostEvent::invokeMethod(_module, "resume");
}

void TCoIpModule::stop() {
	PostEvent::invokeMethod(_module, "stop");
}
