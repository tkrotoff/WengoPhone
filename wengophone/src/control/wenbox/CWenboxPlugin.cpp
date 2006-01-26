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

#include "CWenboxPlugin.h"

#include "presentation/PFactory.h"
#include "presentation/PWenboxPlugin.h"
#include "model/wenbox/WenboxPlugin.h"

CWenboxPlugin::CWenboxPlugin(WenboxPlugin & wenboxPlugin, CWengoPhone & cWengoPhone)
	: _wenboxPlugin(wenboxPlugin),
	_cWengoPhone(cWengoPhone) {

	_pWenboxPlugin = PFactory::getFactory().createPresentationWenboxPlugin(*this);

	_wenboxPlugin.keyPressedEvent += boost::bind(&CWenboxPlugin::keyPressedEventHandler, this, _1, _2);
}

CWenboxPlugin::~CWenboxPlugin() {
}

void CWenboxPlugin::keyPressedEventHandler(IWenbox & sender, IWenbox::Key key) {
	switch (key) {
	case Wenbox::KeyPickUp:
		_pWenboxPlugin->keyPickUpPressedEvent();
		break;

	case Wenbox::KeyHangUp:
		_pWenboxPlugin->keyHangUpPressedEvent();
		break;

	case Wenbox::Key0:
		_pWenboxPlugin->keyNumberPressedEvent("0");
		break;

	case Wenbox::Key1:
		_pWenboxPlugin->keyNumberPressedEvent("1");
		break;

	case Wenbox::Key2:
		_pWenboxPlugin->keyNumberPressedEvent("2");
		break;

	case Wenbox::Key3:
		_pWenboxPlugin->keyNumberPressedEvent("3");
		break;

	case Wenbox::Key4:
		_pWenboxPlugin->keyNumberPressedEvent("4");
		break;

	case Wenbox::Key5:
		_pWenboxPlugin->keyNumberPressedEvent("5");
		break;

	case Wenbox::Key6:
		_pWenboxPlugin->keyNumberPressedEvent("6");
		break;

	case Wenbox::Key7:
		_pWenboxPlugin->keyNumberPressedEvent("7");
		break;

	case Wenbox::Key8:
		_pWenboxPlugin->keyNumberPressedEvent("8");
		break;

	case Wenbox::Key9:
		_pWenboxPlugin->keyNumberPressedEvent("9");
		break;

	case Wenbox::KeyStar:
		_pWenboxPlugin->keyNumberPressedEvent("*");
		break;
	}
}
