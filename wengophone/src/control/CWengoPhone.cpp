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

#include "CWengoPhone.h"

#include <presentation/PFactory.h>
#include <presentation/PWengoPhone.h>

#include <control/profile/CUserProfileHandler.h>
#include <control/webservices/subscribe/CSubscribe.h>

#include <model/WengoPhone.h>
#include <model/webservices/subscribe/WsSubscribe.h>

#include <util/Logger.h>

using namespace std;

CWengoPhone::CWengoPhone(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone) {

	_pWengoPhone = PFactory::getFactory().createPresentationWengoPhone(*this);

	_cUserProfileHandler = 
		new CUserProfileHandler(wengoPhone.getUserProfileHandler(), *this, _wengoPhone);

	_wengoPhone.initFinishedEvent +=
		boost::bind(&CWengoPhone::initFinishedEventHandler, this, _1);
	_wengoPhone.timeoutEvent += controlTimeoutEvent;
	_wengoPhone.wsSubscribeCreatedEvent +=
		boost::bind(&CWengoPhone::wsSubscribeCreatedEventHandler, this, _1, _2);
}

CWengoPhone::~CWengoPhone() {
	if (_pWengoPhone) {
		delete _pWengoPhone;
	}
}

void CWengoPhone::start() {
	_wengoPhone.start();
}

void CWengoPhone::terminate() {
	_wengoPhone.terminate();
}

void CWengoPhone::initFinishedEventHandler(WengoPhone & sender) {
	LOG_DEBUG("WengoPhone::init() finished");
}

void CWengoPhone::wsSubscribeCreatedEventHandler(WengoPhone & sender, WsSubscribe & wsSubscribe) {
	static CSubscribe cSubscribe(wsSubscribe, *this);

	LOG_DEBUG("CSubscribe created");
}
