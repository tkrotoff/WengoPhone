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

#include "CallWidget.h"

#include "ui_CallWidget.h"

#include <coipmanager_threaded/TCoIpManager.h>
#include <coipmanager_threaded/callsessionmanager/TCallSession.h>
#include <coipmanager_threaded/callsessionmanager/TCallSessionManager.h>

#include <coipmanager_base/contact/Contact.h>
#include <coipmanager_base/imcontact/IMContact.h>

#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

#include <QtGui/QtGui>

CallWidget::CallWidget(TCoIpManager &tCoIpManager)
	: _tCoIpManager(tCoIpManager) { 

	_tCallSession = NULL;
	_localImage = NULL;
	_remoteImage = NULL;

	_ui = new Ui::CallWidget();
	_ui->setupUi(this);

	SAFE_CONNECT(_ui->callButton, SIGNAL(clicked()), SLOT(callButtonClicked()));
	SAFE_CONNECT(_ui->hangUpButton, SIGNAL(clicked()), SLOT(hangUpButtonClicked()));

	_ui->hangUpButton->setEnabled(false);
}

CallWidget::~CallWidget() {
	OWSAFE_DELETE(_localImage);
	OWSAFE_DELETE(_remoteImage);
	OWSAFE_DELETE(_tCallSession);
	OWSAFE_DELETE(_ui);
}

void CallWidget::callButtonClicked() {
	_tCallSession = _tCoIpManager.getTCallSessionManager().createTCallSession();

	SAFE_CONNECT(_tCallSession,
		SIGNAL(videoFrameReceivedSignal(QImage, QImage)),
		SLOT(videoFrameReceivedSlot(QImage, QImage)));

	SAFE_CONNECT(_tCallSession,
		SIGNAL(phoneCallStateChangedSignal(EnumPhoneCallState::PhoneCallState)),
		SLOT(phoneCallStateChangedSlot(EnumPhoneCallState::PhoneCallState)));

	Contact contact;
	contact.addIMContact(IMContact(EnumAccountType::AccountTypeSIP,
		_ui->phoneNumberLineEdit->text().toStdString()));
	_tCallSession->addContact(contact);

	_tCallSession->enableVideo(true);
	_tCallSession->start();
}

void CallWidget::hangUpButtonClicked() {
	if (_tCallSession) {
		_tCallSession->stop();
		OWSAFE_DELETE(_tCallSession);
		_ui->hangUpButton->setEnabled(false);
		_ui->callButton->setEnabled(true);
	}
}

void CallWidget::videoFrameReceivedSlot(QImage remoteVideoFrame, QImage localVideoFrame) {
	_ui->localWebcamLabel->setImage(localVideoFrame.scaled(_ui->localWebcamLabel->size()));
	_ui->remoteWebcamLabel->setImage(remoteVideoFrame.scaled(_ui->remoteWebcamLabel->size()));

	_ui->remoteWebcamLabel->update();
	_ui->localWebcamLabel->update();
}

void CallWidget::phoneCallStateChangedSlot(EnumPhoneCallState::PhoneCallState state) {
	switch(state) {
	//case EnumPhoneCallState::PhoneCallStateUnknown:
	case EnumPhoneCallState::PhoneCallStateClosed:
	case EnumPhoneCallState::PhoneCallStateError:
		_ui->hangUpButton->setEnabled(false);
		_ui->callButton->setEnabled(true);
		OWSAFE_DELETE(_tCallSession);
		break;
	case EnumPhoneCallState::PhoneCallStateTalking:
	case EnumPhoneCallState::PhoneCallStateDialing:
	case EnumPhoneCallState::PhoneCallStateRinging:
		_ui->hangUpButton->setEnabled(true);
		_ui->callButton->setEnabled(false);
		break;
	//case EnumPhoneCallState::PhoneCallStateHold:
	//case EnumPhoneCallState::PhoneCallStateResumed:
	//case EnumPhoneCallState::PhoneCallStateMissed:
	//case EnumPhoneCallState::PhoneCallStateRedirected:

	//case EnumPhoneCallState::PhoneCallStateIncoming:

	default:
		break;
	}
}
