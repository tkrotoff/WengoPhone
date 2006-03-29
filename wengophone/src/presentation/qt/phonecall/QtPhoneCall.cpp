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

#include "QtPhoneCall.h"

#include "QtVideo.h"

#include <presentation/qt/contactlist/QtAddContact.h>
#include <presentation/qt/QtWengoPhone.h>
#include <control/phonecall/CPhoneCall.h>
#include <control/CWengoPhone.h>

#include <sipwrapper/WebcamVideoFrame.h>

#include <util/Logger.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

#include <QtGui>

QtPhoneCall::QtPhoneCall(CPhoneCall & cPhoneCall)
	: QObjectThreadSafe(),
	_cPhoneCall(cPhoneCall) {

	_qtWengoPhone = (QtWengoPhone *) _cPhoneCall.getCWengoPhone().getPresentation();
	_videoWindow = NULL;
	_hold = true;

	stateChangedEvent += boost::bind(&QtPhoneCall::stateChangedEventHandler, this, _1);
	videoFrameReceivedEvent += boost::bind(&QtPhoneCall::videoFrameReceivedEventHandler, this, _1, _2);

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneCall::initThreadSafe, this));
	postEvent(event);
}

void QtPhoneCall::initThreadSafe() {
	_phoneCallWidget = WidgetFactory::create(":/forms/phonecall/PhoneCallWidget.ui", _qtWengoPhone->getWidget());

	QString sipAddress = QString::fromStdString(_cPhoneCall.getPeerSipAddress());
	QString callAddress = QString::fromStdString(_cPhoneCall.getPeerDisplayName());
	if (callAddress.isEmpty()) {
		callAddress = QString::fromStdString(_cPhoneCall.getPeerUserName());
	}

	//acceptButton
	_acceptButton = Object::findChild<QPushButton *>(_phoneCallWidget, "acceptButton");
	_acceptButton->setEnabled(true);
	_acceptButton->disconnect();
	connect(_acceptButton, SIGNAL(clicked()), SLOT(acceptButtonClicked()));

	//phoneNumberLabel
	QLabel * phoneNumberLabel = Object::findChild<QLabel *>(_phoneCallWidget, "phoneNumberLabel");
	phoneNumberLabel->setText(callAddress);
	phoneNumberLabel->setToolTip(sipAddress);

	//rejectButton
	_rejectButton = Object::findChild<QPushButton *>(_phoneCallWidget, "rejectButton");
	_rejectButton->setEnabled(true);
	_rejectButton->disconnect();
	connect(_rejectButton, SIGNAL(clicked()), SLOT(rejectButtonClicked()));

	//holdResumeButton
	_holdResumeButton = Object::findChild<QPushButton *>(_phoneCallWidget, "holdResumeButton");
	_holdResumeButton->disconnect();
	connect(_holdResumeButton, SIGNAL(clicked()), SLOT(holdResumeButtonClicked()));

	//addContactButton
	_addContactButton = Object::findChild<QPushButton *>(_phoneCallWidget, "addContactButton");
	_addContactButton->disconnect();
	connect(_addContactButton, SIGNAL(clicked()), SLOT(addContactButtonClicked()));

	//transferButton
	QPushButton * transferButton = Object::findChild<QPushButton *>(_phoneCallWidget, "transferButton");
	transferButton->disconnect();
	connect(transferButton, SIGNAL(clicked()), SLOT(transferButtonClicked()));

	_qtWengoPhone->addPhoneCall(this);
}

void QtPhoneCall::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneCall::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtPhoneCall::updatePresentationThreadSafe() {
}

void QtPhoneCall::stateChangedEventHandler(EnumPhoneCallState::PhoneCallState state) {
	typedef PostEvent1<void (EnumPhoneCallState::PhoneCallState), EnumPhoneCallState::PhoneCallState> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneCall::stateChangedEventHandlerThreadSafe, this, _1), state);
	postEvent(event);
}

void QtPhoneCall::stateChangedEventHandlerThreadSafe(EnumPhoneCallState::PhoneCallState state) {
	static const QString originalHoldText = _holdResumeButton->text();

	switch(state) {

	case EnumPhoneCallState::PhoneCallStateDefault:
		break;

	case EnumPhoneCallState::PhoneCallStateError:
		_acceptButton->setEnabled(false);
		_rejectButton->setEnabled(false);
		break;

	case EnumPhoneCallState::PhoneCallStateResumed:
		_holdResumeButton->setText(originalHoldText);
		_hold = true;
		break;

	case EnumPhoneCallState::PhoneCallStateTalking:
		_acceptButton->setEnabled(false);
		_rejectButton->setEnabled(true);
		break;

	case EnumPhoneCallState::PhoneCallStateDialing:
		_acceptButton->setEnabled(false);
		_rejectButton->setEnabled(true);
		break;

	case EnumPhoneCallState::PhoneCallStateRinging:
		_acceptButton->setEnabled(false);
		_rejectButton->setEnabled(true);
		break;

	case EnumPhoneCallState::PhoneCallStateClosed:
		_acceptButton->setEnabled(false);
		_rejectButton->setEnabled(false);
		break;

	case EnumPhoneCallState::PhoneCallStateIncoming:
		_acceptButton->setEnabled(true);
		_rejectButton->setEnabled(true);
		break;

	case EnumPhoneCallState::PhoneCallStateHold:
		_holdResumeButton->setText(tr("Resume"));
		_hold = false;
		break;

	case EnumPhoneCallState::PhoneCallStateMissed:
		break;

	case EnumPhoneCallState::PhoneCallStateRedirected:
		break;

	default:
		LOG_FATAL("unknown PhoneCallState=" + String::fromNumber(state));
	}
}

void QtPhoneCall::videoFrameReceivedEventHandler(const WebcamVideoFrame & remoteVideoFrame, const WebcamVideoFrame & localVideoFrame) {
	//image will be deleted in videoFrameReceivedThreadSafe
	QImage * image = new QImage(remoteVideoFrame.getFrame(), remoteVideoFrame.getWidth(),
			remoteVideoFrame.getHeight(), QImage::Format_RGB32);

	typedef PostEvent1<void (QImage *), QImage *> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneCall::videoFrameReceivedEventHandlerThreadSafe, this, _1), image);
	postEvent(event);
}

void QtPhoneCall::videoFrameReceivedEventHandlerThreadSafe(QImage * image) {
	if (!_videoWindow) {
		_videoWindow = new QtVideo(_phoneCallWidget);
		_videoWindow->getWidget()->show();
	}

	//_videoWindow->getWidget()->resize(image.size());
	_videoWindow->showImage(*image);

	//image was created in videoFrameReceived
	delete image;
}

void QtPhoneCall::acceptButtonClicked() {
	_cPhoneCall.pickUp();
}

void QtPhoneCall::rejectButtonClicked() {
	_cPhoneCall.hangUp();
}

void QtPhoneCall::holdResumeButtonClicked() {
	if (_hold) {
		_cPhoneCall.hold();
	} else {
		_cPhoneCall.resume();
	}
}

void QtPhoneCall::addContactButtonClicked() {
	std::string callAddress = _cPhoneCall.getPeerDisplayName();
	if (callAddress.empty()) {
		callAddress = _cPhoneCall.getPeerUserName();
	}
	QtAddContact * qtAddContact = new QtAddContact(_cPhoneCall.getCWengoPhone(), _phoneCallWidget, callAddress);
	LOG_DEBUG("add contact=" + callAddress);
}

void QtPhoneCall::transferButtonClicked() {
	static QLineEdit * transferPhoneNumberLineEdit = Object::findChild<QLineEdit *>(_phoneCallWidget, "transferPhoneNumberLineEdit");

	_cPhoneCall.blindTransfer(transferPhoneNumberLineEdit->text().toStdString());
}
