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

#include "QtPhoneCall.h"

#include "model/WengoPhoneLogger.h"
#include "control/phonecall/CPhoneCall.h"
#include "control/CWengoPhone.h"
#include "presentation/qt/QtWengoPhone.h"
#include <model/sipwrapper/WebcamVideoFrame.h>
#include "QtVideo.h"

#include <WidgetFactory.h>
#include <Object.h>

#include <QtGui>

QtPhoneCall::QtPhoneCall(CPhoneCall & cPhoneCall)
	: QObjectThreadSafe(),
	_cPhoneCall(cPhoneCall) {

	_qtWengoPhone = (QtWengoPhone *) _cPhoneCall.getCWengoPhone().getPresentation();
	_videoWindow = NULL;

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneCall::initThreadSafe, this));
	postEvent(event);
}

void QtPhoneCall::initThreadSafe() {
	_phoneCallWindow = WidgetFactory::create(":/forms/phonecall/PhoneCallWindow.ui", _qtWengoPhone->getWidget());

	QString sipAddress = QString::fromStdString(_cPhoneCall.getPeerSipAddress());
	QString callAddress = QString::fromStdString(_cPhoneCall.getPeerDisplayName());;
	if (callAddress.isEmpty()) {
		callAddress = QString::fromStdString(_cPhoneCall.getPeerUserName());
	}

	_phoneCallWindow->setWindowTitle(callAddress);

	//acceptButton
	_acceptButton = Object::findChild<QPushButton *>(_phoneCallWindow, "acceptButton");
	_acceptButton->setEnabled(true);
	_acceptButton->disconnect();
	connect(_acceptButton, SIGNAL(clicked()), SLOT(acceptButtonClicked()));

	//phoneNumberLabel
	QLabel * phoneNumberLabel = Object::findChild<QLabel *>(_phoneCallWindow, "phoneNumberLabel");
	phoneNumberLabel->setText(callAddress);
	phoneNumberLabel->setToolTip(sipAddress);

	//rejectButton
	_rejectButton = Object::findChild<QPushButton *>(_phoneCallWindow, "rejectButton");
	_rejectButton->setEnabled(true);
	_rejectButton->disconnect();
	connect(_rejectButton, SIGNAL(clicked()), SLOT(rejectButtonClicked()));

	//muteButton
	_muteButton = Object::findChild<QPushButton *>(_phoneCallWindow, "muteButton");
	_muteButton->disconnect();
	connect(_muteButton, SIGNAL(clicked()), SLOT(muteButtonClicked()));

	//holdButton
	_holdButton = Object::findChild<QPushButton *>(_phoneCallWindow, "holdButton");
	_holdButton->disconnect();
	connect(_holdButton, SIGNAL(clicked()), SLOT(holdButtonClicked()));

	//addContactButton
	_addContactButton = Object::findChild<QPushButton *>(_phoneCallWindow, "addContactButton");
	_addContactButton->disconnect();
	connect(_addContactButton, SIGNAL(clicked()), SLOT(addContactButtonClicked()));

	_phoneCallWindow->show();
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
	_phoneCallWindow->hide();
}

void QtPhoneCall::videoFrameReceived(const WebcamVideoFrame & remoteVideoFrame, const WebcamVideoFrame & localVideoFrame) {
	//Image will be deleted in videoFrameReceivedThreadSafe
	QImage * image = new QImage(remoteVideoFrame.getFrame(), remoteVideoFrame.getWidth(),
			remoteVideoFrame.getHeight(), QImage::Format_RGB32);

	typedef PostEvent1<void (QImage *), QImage *> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneCall::videoFrameReceivedThreadSafe, this, _1), image);
	postEvent(event);
}

void QtPhoneCall::videoFrameReceivedThreadSafe(QImage *image) {
	if (!_videoWindow) {
		_videoWindow = new QtVideo(_phoneCallWindow);
		_videoWindow->getWidget()->show();
	}

	//_videoWindow->getWidget()->resize(image.size());
	_videoWindow->showImage(*image);

	// image was created in videoFrameReceived
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
}

void QtPhoneCall::holdButtonClicked() {
}

void QtPhoneCall::addContactButtonClicked() {
}
