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

#include <Logger.h>

#include <WidgetFactory.h>
#include <Object.h>

#include <QtGui>

QtPhoneCall::QtPhoneCall(CPhoneCall & cPhoneCall)
	: QObjectThreadSafe(),
	_cPhoneCall(cPhoneCall) {

	_qtWengoPhone = (QtWengoPhone *) _cPhoneCall.getCWengoPhone().getPresentation();
	_videoWindow = NULL;
	_hold = true;

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

	//muteButton
	_muteButton = Object::findChild<QPushButton *>(_phoneCallWidget, "muteButton");
	_muteButton->disconnect();
	connect(_muteButton, SIGNAL(clicked()), SLOT(muteButtonClicked()));

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
	_acceptButton->setEnabled(_cPhoneCall.canPickUp());
	_rejectButton->setEnabled(_cPhoneCall.canHangUp());
}

void QtPhoneCall::close() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneCall::closeThreadSafe, this));
	postEvent(event);
}

void QtPhoneCall::closeThreadSafe() {
	//_phoneCallWidget->hide();
}

void QtPhoneCall::videoFrameReceived(const WebcamVideoFrame & remoteVideoFrame, const WebcamVideoFrame & localVideoFrame) {
	//image will be deleted in videoFrameReceivedThreadSafe
	QImage * image = new QImage(remoteVideoFrame.getFrame(), remoteVideoFrame.getWidth(),
			remoteVideoFrame.getHeight(), QImage::Format_RGB32);

	typedef PostEvent1<void (QImage *), QImage *> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneCall::videoFrameReceivedThreadSafe, this, _1), image);
	postEvent(event);
}

void QtPhoneCall::videoFrameReceivedThreadSafe(QImage * image) {
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
	if (_videoWindow) {
		//_videoWindow->getWidget()->hide();
	}
}

void QtPhoneCall::muteButtonClicked() {
	_cPhoneCall.mute();
}

void QtPhoneCall::holdResumeButtonClicked() {
	static const QString originalHoldText = _holdResumeButton->text();

	if (_hold) {
		_holdResumeButton->setEnabled(false);
		_cPhoneCall.hold();
		_holdResumeButton->setText(tr("Resume"));
		_holdResumeButton->setEnabled(true);
	} else {
		_holdResumeButton->setEnabled(false);
		_cPhoneCall.resume();
		_holdResumeButton->setText(originalHoldText);
		_holdResumeButton->setEnabled(true);
	}

	_hold = !_hold;
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
