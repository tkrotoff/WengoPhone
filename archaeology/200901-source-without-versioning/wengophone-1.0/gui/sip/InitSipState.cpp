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

#include "InitSipState.h"

#include "DialingSipState.h"
#include "InputCallSipState.h"
#include "ConversationSipState.h"
#include "SipPrimitives.h"
#include "AudioCall.h"
#include "PhonePageWidget.h"
#include "SipAddress.h"

#include <qpushbutton.h>
#include <qcombobox.h>
#include <qmessagebox.h>

#include <cassert>
#include <iostream>
#include <phapi.h>

using namespace std;

InitSipState::InitSipState(AudioCall & audioCall)
: SipState(audioCall) {
	_kind = SipState::init_state;
	_phonePageWidget.setStatusText(QString::null);
	initButtons();
	_audioCall.setCallId(AudioCall::NOT_CALLING);
}

InitSipState::~InitSipState() {
}

void InitSipState::initButtons() const {
	QComboBox * phoneComboBox = _phonePageWidget.getPhoneComboBox();
	connect(phoneComboBox, SIGNAL(activated(const QString &)),
		this, SLOT(videoCall()));
	connect(phoneComboBox, SIGNAL(textChanged(const QString &)),
		this, SLOT(callButtonEnabled(const QString &)));
	QPushButton * audioCallButton = _phonePageWidget.getAudioCallButton();
	QPushButton * videoCallButton = _phonePageWidget.getVideoCallButton();
	if (phoneComboBox->currentText().isEmpty()) {
		audioCallButton->setEnabled(false);
		videoCallButton->setEnabled(false);
	} else {
		audioCallButton->setEnabled(true);
		videoCallButton->setEnabled(true);
	}
	disconnect(audioCallButton, SIGNAL(clicked()), this, SLOT(audioCall()));
	connect(audioCallButton, SIGNAL(clicked()), this, SLOT(audioCall()));
	disconnect(videoCallButton, SIGNAL(clicked()), this, SLOT(videoCall()));
	connect(videoCallButton, SIGNAL(clicked()), this, SLOT(videoCall()));

	QPushButton * hangUpButton = _phonePageWidget.getHangUpButton();
	hangUpButton->setEnabled(false);
}

void InitSipState::handle(int callId, const CallStateInfo * info) const {
	switch (info->event) {
	case phDIALING:
		break;

	case phRINGING:
		break;

	case phNOANSWER:
		break;

	case phCALLBUSY:
		break;

	case phCALLREDIRECTED:
		break;

	case phCALLOK:
		break;

	case phCALLHELD:
		break;

	case phCALLRESUMED:
		break;

	case phHOLDOK:
		break;

	case phRESUMEOK:
		break;

	case phINCALL:
		_audioCall.setCallId(callId);
		_audioCall.setSipState(new InputCallSipState(_audioCall,
					SipAddress(info->u.remoteUri)));
		break;

	case phCALLCLOSED:
		break;

	case phCALLERROR:
		break;

	default:
		assert(NULL && "This phapi event is not handled by this class");
	}
}

void InitSipState::audioCall() {
	call(false);
}

void InitSipState::videoCall() {
	call(true);
}

void InitSipState::call(bool video) {
	QString phoneNumber = _phonePageWidget.getPhoneNumberToCall();

	if (phoneNumber.isEmpty()) {
		QMessageBox::warning(_phonePageWidget.getWidget(),
					tr("Wrong phone number"),
					tr("Wrong phone number"));
		initButtons();
		return;
	}

	int ret;
	if (video) {
		ret = SipPrimitives::placeVideoCall(SipAddress::fromPhoneNumber(phoneNumber));
	} else {
		ret = SipPrimitives::placeAudioCall(SipAddress::fromPhoneNumber(phoneNumber));
	}

	if (ret == -1) {
		QMessageBox::warning(_phonePageWidget.getWidget(),
					tr("Wrong phone number"),
					tr("Wrong phone number or check that you enough credits to make a call or check your Internet connection"));
		initButtons();
	} else {
		_audioCall.setCallId(ret);
		_audioCall.setSipState(new DialingSipState(_audioCall));
	}
}

void InitSipState::callButtonEnabled(const QString &) {
	QComboBox * phoneComboBox = _phonePageWidget.getPhoneComboBox();
	QPushButton * audioCallButton = _phonePageWidget.getAudioCallButton();
	QPushButton * videoCallButton = _phonePageWidget.getVideoCallButton();

	if (!phoneComboBox->currentText().isEmpty()) {
		if (!audioCallButton->isEnabled()) {
			audioCallButton->setEnabled(true);
		}
		if (!videoCallButton->isEnabled()) {
			videoCallButton->setEnabled(true);
		}
	} else {
		if (!audioCallButton->isEnabled()) {
			audioCallButton->setEnabled(false);
		}
		if (!videoCallButton->isEnabled()) {
			videoCallButton->setEnabled(false);
		}
	}
}
