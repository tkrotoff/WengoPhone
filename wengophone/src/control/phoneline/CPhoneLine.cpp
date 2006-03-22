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

#include "CPhoneLine.h"

#include "model/phoneline/IPhoneLine.h"
#include "presentation/PPhoneLine.h"
#include "presentation/PFactory.h"
#include "model/phoneline/PhoneLineState.h"
#include "control/phonecall/CPhoneCall.h"
#include "control/CWengoPhone.h"

#include "model/phoneline/PhoneLineStateDefault.h"
#include "model/phoneline/PhoneLineStateOk.h"
#include "model/phoneline/PhoneLineStateClosed.h"
#include "model/phoneline/PhoneLineStateTimeout.h"
#include "model/phoneline/PhoneLineStateServerError.h"

#include <util/Logger.h>

CPhoneLine::CPhoneLine(IPhoneLine & phoneLine, CWengoPhone & cWengoPhone)
	: _phoneLine(phoneLine),
	_cWengoPhone(cWengoPhone) {

	_pPhoneLine = PFactory::getFactory().createPresentationPhoneLine(*this);

	_phoneLine.stateChangedEvent += boost::bind(&CPhoneLine::stateChangedEventHandler, this, _1);
	_phoneLine.phoneCallCreatedEvent += boost::bind(&CPhoneLine::phoneCallCreatedEventHandler, this, _1, _2);
}

int CPhoneLine::makeCall(const std::string & phoneNumber) {
	return _phoneLine.makeCall(phoneNumber);
}

void CPhoneLine::stateChangedEventHandler(IPhoneLine & sender) {
	int lineId = sender.getLineId();

	switch (sender.getState().getCode()) {
	case PhoneLineStateDefault::CODE:
		break;

	case PhoneLineStateOk::CODE:
		_pPhoneLine->phoneLineStateChangedEvent(PPhoneLine::LineOk, lineId);
		break;

	case PhoneLineStateTimeout::CODE:
		_pPhoneLine->phoneLineStateChangedEvent(PPhoneLine::LineTimeout, lineId);
		break;

	case PhoneLineStateServerError::CODE:
		_pPhoneLine->phoneLineStateChangedEvent(PPhoneLine::LineServerError, lineId);
		break;

	case PhoneLineStateClosed::CODE:
		_pPhoneLine->phoneLineStateChangedEvent(PPhoneLine::LineClosed, lineId);
		break;

	default:
		LOG_FATAL("unknown PhoneLine state=" + String::fromNumber(sender.getState().getCode()));
	};
}

void CPhoneLine::phoneCallCreatedEventHandler(IPhoneLine & sender, PhoneCall & phoneCall) {
	CPhoneCall * cPhoneCall = new CPhoneCall(phoneCall, _cWengoPhone);

	LOG_DEBUG("CPhoneCall created");
}
