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

#include <model/commandserver/CommandServer.h>
#include <model/WengoPhone.h>

#include <thread/ThreadEvent.h>
#include <util/Logger.h>

CWengoPhone::CWengoPhone(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone) {

	_pWengoPhone = NULL;
	typedef ThreadEvent0<void ()> MyThreadEvent;
	MyThreadEvent * event = new MyThreadEvent(boost::bind(&CWengoPhone::initPresentationThreadSafe, this));
	PFactory::postEvent(event);
}

CWengoPhone::~CWengoPhone() {
	/*if (_pWengoPhone) {
		delete _pWengoPhone;
	}*/
}

void CWengoPhone::initPresentationThreadSafe() {

	_pWengoPhone = PFactory::getFactory().createPresentationWengoPhone(*this);

	_cUserProfileHandler = new CUserProfileHandler(_wengoPhone.getUserProfileHandler(), *this);

	_wengoPhone.initFinishedEvent += boost::bind(&CWengoPhone::initFinishedEventHandler, this, _1);
	_wengoPhone.exitEvent += boost::bind(&CWengoPhone::exitEventHandler, this);

	CommandServer & commandServer = CommandServer::getInstance(_wengoPhone);
	commandServer.showAddContactEvent += boost::bind(&CWengoPhone::showAddContactEventHandler, this, _1, _2, _3, _4, _5, _6, _7, _8);
}

Presentation * CWengoPhone::getPresentation() const {
	return _pWengoPhone;
}

CWengoPhone & CWengoPhone::getCWengoPhone() const {
	return (CWengoPhone &) *this;
}

CUserProfileHandler & CWengoPhone::getCUserProfileHandler() const {
	return *_cUserProfileHandler;
}

WengoPhone & CWengoPhone::getWengoPhone() const {
	return _wengoPhone;
}

void CWengoPhone::start() {
	_wengoPhone.start();
}

void CWengoPhone::terminate() {
	_wengoPhone.prepareToTerminate();
}

void CWengoPhone::initFinishedEventHandler(WengoPhone & sender) {
	LOG_DEBUG("WengoPhone::init() finished");
}

void CWengoPhone::exitEventHandler() {
	typedef ThreadEvent0<void ()> MyThreadEvent;
	MyThreadEvent * event = new MyThreadEvent(boost::bind(&CWengoPhone::exitEventHandlerThreadSafe, this));
	PFactory::postEvent(event);
}

void CWengoPhone::exitEventHandlerThreadSafe() {
	_wengoPhone.terminate();
	if (_pWengoPhone) {
		_pWengoPhone->exitEvent();
	}
}

void CWengoPhone::showAddContactEventHandler(const std::string & wengoName,
	const std::string & sip, const std::string & firstname,
	const std::string & lastname, const std::string & country,
	const std::string & city, const std::string & state,
	const std::string & group) {

	_pWengoPhone->showAddContact(wengoName, sip, firstname, lastname, country, city, state, group);
}
