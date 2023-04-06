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

#include "SipState.h"

#include "AudioCall.h"
#include "PhonePageWidget.h"

#include <qcombobox.h>
#include <qpushbutton.h>


SipState::SipState(AudioCall & audioCall) 
: _audioCall(audioCall), _phonePageWidget(_audioCall.getPhonePageWidget()) {

	//Disconnect the buttons of PhonePageWidget
	QComboBox * phoneComboBox = _phonePageWidget.getPhoneComboBox();
	phoneComboBox->disconnect();
	//QPushButton * callButton = _phonePageWidget.getCallButton();
	//callButton->disconnect();
	QPushButton * hangUpButton = _phonePageWidget.getHangUpButton();
	hangUpButton->disconnect();
}

SipState::kind SipState::getKind() const {
	return _kind;
}
