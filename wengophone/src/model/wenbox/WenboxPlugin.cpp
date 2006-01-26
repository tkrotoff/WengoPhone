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

#include "WenboxPlugin.h"

#include "model/WengoPhone.h"
#include "model/phonecall/PhoneCall.h"

WenboxPlugin::WenboxPlugin(const WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone) {

	_wenbox = new Wenbox();
	_wenbox->open();
	_wenbox->setDefaultMode(Wenbox::ModeUSB);
	_wenbox->switchMode(Wenbox::ModeUSB);
	_wenbox->keyPressedEvent += boost::bind(&WenboxPlugin::keyPressedEventHandler, this, _1, _2);
	_wenbox->keyPressedEvent += keyPressedEvent;
}

WenboxPlugin::~WenboxPlugin() {
	_wenbox->close();
	delete _wenbox;
}

void WenboxPlugin::keyPressedEventHandler(IWenbox & sender, IWenbox::Key key) {
	PhoneCall * phoneCall = _wengoPhone.getActivePhoneCall();

	switch (key) {
	case IWenbox::KeyPickUp:
		if (phoneCall) {
			phoneCall->accept();
		}
		break;

	case IWenbox::KeyHangUp:
		if (phoneCall) {
			phoneCall->close();
		}
		break;
	}
}

void WenboxPlugin::setState(Wenbox::PhoneCallState state, const std::string & phoneNumber) {
	_wenbox->setState(state, phoneNumber);
}
