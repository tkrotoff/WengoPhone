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

#include "XPCOMWengoPhone.h"

#include <Listener.h>
#include "ListenerList.h"
#include "XPCOMIMHandler.h"

#include <control/CWengoPhone.h>

#include <Logger.h>

#include <cassert>

CWengoPhone * XPCOMWengoPhone::_cWengoPhone = NULL;

XPCOMWengoPhone::XPCOMWengoPhone(CWengoPhone * cWengoPhone) {
	_cWengoPhone = cWengoPhone;
}

CWengoPhone & XPCOMWengoPhone::getCWengoPhone() {
	assert(_cWengoPhone && "XPCOMWengoPhone::XPCOMWengoPhone() must be call first");
	return *_cWengoPhone;
}

void XPCOMWengoPhone::initThreadSafe() { }

void XPCOMWengoPhone::addPhoneLine(PPhoneLine * pPhoneLine) { }

void XPCOMWengoPhone::addPhoneLineThreadSafe(PPhoneLine * pPhoneLine) { }

void XPCOMWengoPhone::wengoLoginStateChangedEvent(WengoPhone::LoginState state, const std::string & login, const std::string & password) {
	ListenerList & listenerList = ListenerList::getInstance();
	for (int i = 0; i < listenerList.size(); i++) {
		Listener * listener = listenerList[i];

		switch (state) {
		case WengoPhone::LoginOk:
			XPCOMIMHandler::getInstance().setWengoLoginPassword(login, password);
			listener->wengoLoginStateChangedEvent(Listener::LoginOk, login, password);
			break;

		case WengoPhone::LoginPasswordError:
			listener->wengoLoginStateChangedEvent(Listener::LoginPasswordError, login, password);
			break;

		case WengoPhone::LoginNetworkError:
			listener->wengoLoginStateChangedEvent(Listener::LoginNetworkError, login, password);
			break;

		case WengoPhone::LoginNoAccount:
			listener->wengoLoginStateChangedEvent(Listener::LoginNoAccount,login, password);
			break;

		default:
			LOG_FATAL("Unknown state");
		};
	}
}

void XPCOMWengoPhone::updatePresentation() { }

void XPCOMWengoPhone::updatePresentationThreadSafe() { }
