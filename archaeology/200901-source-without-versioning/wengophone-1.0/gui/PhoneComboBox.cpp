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

#include "PhoneComboBox.h"

#include "callhistory/CallLog.h"
#include "AudioCallManager.h"
#include "util/EventFilter.h"
#include "sip/SipAddress.h"
#include "util/PhoneLineEdit.h"

#include <qwidget.h>
#include <qcombobox.h>

#include <cassert>
#include <iostream>
using namespace std;

const unsigned int PhoneComboBox::MAX_PHONE_NUMBERS = 10;

PhoneComboBox::PhoneComboBox(QWidget * parent)
: QObject(parent) {
	_phoneComboBox = (QComboBox *) parent->child("phoneComboBox", "QComboBox");

	//Mouse Wheel on combobox -> Refresh list
	WheelEventFilter * wheelFilter = new WheelEventFilter(this, SLOT(insertCallHistory()));
	_phoneComboBox->installEventFilter(wheelFilter);

	//Mouse click on combobox -> Refresh list
	MousePressEventFilter * mousePressFilter = new MousePressEventFilter(this, SLOT(insertCallHistory()));
	_phoneComboBox->installEventFilter(mousePressFilter);

	//Return key pressed -> call
	ReturnKeyEventFilter * returnKeyFilter = new ReturnKeyEventFilter(this, SLOT(callButtonClicked()));
	_phoneComboBox->installEventFilter(returnKeyFilter);

	//New QLineEdit
	PhoneLineEdit * phoneLineEdit = new PhoneLineEdit(_phoneComboBox);
	_phoneComboBox->setLineEdit(phoneLineEdit);
	phoneLineEdit->setHelpText(tr("Enter a phone number or a Wengo name"));
	_phoneComboBox->setFocus();
}

PhoneComboBox::~PhoneComboBox() {
}

void PhoneComboBox::insertCallHistory() {
	CallHistory & callHistory = CallHistory::getInstance();

	//Clear the combobox
	_phoneComboBox->clear();

	//Inserts the last MAX_PHONE_NUMBERS phone numbers inside the combobox
	if (callHistory.size() != 0) {
		unsigned int endlist = callHistory.size() - 1;
		for (unsigned int i = endlist; i > (endlist - MAX_PHONE_NUMBERS); i--) {
			QString log = extractStringFromCallLog(callHistory[i]);
			//Insert log with the type of call
			_phoneComboBox->insertItem(callHistory[i].getTypeIcon(), log);
		}
	}
}

void PhoneComboBox::callButtonClicked() {
	PhoneLineEdit * phoneLineEdit = (PhoneLineEdit *) _phoneComboBox->lineEdit();
	if (!phoneLineEdit->text().isEmpty()) {
		QString phoneNumber = extractPhoneNumberFromString(_phoneComboBox->currentText());

		//Call Phone Number
		if (!phoneNumber.isEmpty()) {
			//TODO autocompletion + recognition of the text from the combo box
			AudioCallManager::getInstance().createAudioCall(SipAddress::fromPhoneNumber(phoneNumber));
		}
	}
}

QString PhoneComboBox::extractStringFromCallLog(const CallLog & callLog) {
	//The phone number is not inside ContactList
	if (callLog.getContactName() == callLog.getPhoneNumber()) {
		return callLog.getContactName();
	}

	//The phone number is inside ContactList
	else {
		return (callLog.getContactName() + " (" + callLog.getPhoneNumber() + ")");
	}
}

QString PhoneComboBox::extractPhoneNumberFromString(const QString & str) {
	//There is no name corresponding to the phone number
	if ((str.find('(')) == 0) {
		return str;
	}

	//Extract the phone number + the contact' name
	else {
		return str.mid(str.find('(') + 1, (str.find(')') - (str.find('(') + 1)));
	}
}
