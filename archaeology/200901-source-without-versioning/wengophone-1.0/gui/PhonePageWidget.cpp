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

#include "PhonePageWidget.h"

#include "MyWidgetFactory.h"
#include "DialPadWidget.h"
#include "contact/Contact.h"
#include "util/emit_signal.h"
#include "util/EventFilter.h"
#include "callhistory/CallHistory.h"
#include "config/Video.h"
#include "SessionWindow.h"

#include <qwidget.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qstatusbar.h>
#include <qwidgetstack.h>
#include <qvalidator.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qdatetime.h>

#include <iostream>
using namespace std;

const Q_ULLONG PhonePageWidget::TIMER_TIMEOUT = 1000;

PhonePageWidget::PhonePageWidget(SessionWindow * sessionWindow)
: QObject(sessionWindow->getWidget()) {
	_phonePageWidget = MyWidgetFactory::create("PhonePageWidgetForm.ui", this, sessionWindow->getWidget());

	//Phone combobox
	_phoneComboBox = (QComboBox *) _phonePageWidget->child("phoneComboBox", "QComboBox");

	_audioCallButton = (QPushButton *) _phonePageWidget->child("audioCallButton", "QPushButton");

	_videoCallButton = (QPushButton *) _phonePageWidget->child("videoCallButton", "QPushButton");

	_hangUpButton = (QPushButton *) _phonePageWidget->child("hangUpButton", "QPushButton");

	_statusLabel = (QLabel *) _phonePageWidget->child("statusLabel", "QLabel");

	_videoFrame = (QFrame *) _phonePageWidget->child("videoFrame");

	_contactPixmap = (QLabel *) _phonePageWidget->child("contactPixmap");
	MousePressEventFilter * mousePressEventFilter = new MousePressEventFilter(sessionWindow, SLOT(showVisioPage()));
	_contactPixmap->installEventFilter(mousePressEventFilter);

	//QRegExpValidator * validator = new QRegExpValidator(QRegExp("([0-9])*"), _phonePageWidget);
	//_phoneComboBox->lineEdit().setValidator(validator);

	_dialPadWidget = new DialPadWidget(_phonePageWidget, *_phoneComboBox);
	QWidgetStack * stack = (QWidgetStack *)
			_phonePageWidget->child("dialPadWidgetStack", "QWidgetStack");
	stack->addWidget(_dialPadWidget->getWidget());
	stack->raiseWidget(_dialPadWidget->getWidget());

	//_nameLabel = (QLabel *) _phonePageWidget->child("nameLabel", "QLabel");
	_nameLabel = new QLabel(0);
	_nameLabel->hide();

	_phoneLabel = (QLabel *) _phonePageWidget->child("phoneLabel", "QLabel");
}

void PhonePageWidget::call() {
#ifdef ENABLE_VIDEO
	if (Video::getInstance().getEnable()) {
		emit_signal(getVideoCallButton(), SIGNAL(clicked()));
	} else {
		emit_signal(getAudioCallButton(), SIGNAL(clicked()));
	}
#else
	emit_signal(getAudioCallButton(), SIGNAL(clicked()));
#endif
}

std::vector<QPushButton *> PhonePageWidget::getNumberButtons() const {
	return _dialPadWidget->getNumberButtons();
}

QPushButton * PhonePageWidget::getStarButton() const {
	return _dialPadWidget->getStarButton();
}

QPushButton * PhonePageWidget::getSharpButton() const {
	return _dialPadWidget->getSharpButton();
}

void PhonePageWidget::setCallerPhoneNumber(const QString & phoneNumber) {
	_phoneLabel->setText(phoneNumber);
}

QString PhonePageWidget::getCallerPhoneNumber() const {
	return _phoneLabel->text();
}

void PhonePageWidget::setPhoneNumberToCall(const QString & phoneNumber) {
	_phoneComboBox->setCurrentText(phoneNumber);
}

QString PhonePageWidget::getPhoneNumberToCall() const {
	return _phoneComboBox->currentText();
	return "";
}

void PhonePageWidget::setContact(Contact & contact) {
	_contact = &contact;

	//Updates the different widgets using this Contact
	setContactName(contact.toString());
	setPhoneNumberToCall(contact.getDefaultPhone());
	//setContactPicture();
}

void PhonePageWidget::setContactName(const QString & name) {
	_nameLabel->setText(name);
}

QString PhonePageWidget::getContactName() const {
	return _nameLabel->text();
}

void PhonePageWidget::setContactPicture(const QString & /*picture*/) {
}

void PhonePageWidget::setStatusText(const QString & statusText) {
	_statusLabel->setText(statusText);
}

void PhonePageWidget::reset() {
	const QString empty = "";

	_statusLabel->setText(empty);
	_phoneComboBox->setCurrentText(empty);
	_nameLabel->setText(empty);
	_phoneLabel->setText(empty);
}

void PhonePageWidget::startCallDurationTimer() {
	//To show the call duration
	_callDurationTimer = new QTimer(_phonePageWidget);
	connect(_callDurationTimer, SIGNAL(timeout()),
			this, SLOT(updateCallDuration()));
	_callDurationTimer->start(TIMER_TIMEOUT);

	_callDuration = new QTime();
	_callDuration->start();
}

void PhonePageWidget::stopCallDurationTimer() {
	assert(_callDurationTimer && "Call startCallDurationTimer() first");
	assert(_callDuration && "Call startCallDurationTimer() first");

	_callDurationTimer->stop();
	delete _callDurationTimer;
	delete _callDuration;
}

void PhonePageWidget::updateCallDuration() {
	QTime duration(0, 0, 0);
	duration = duration.addMSecs(_callDuration->elapsed());
	_statusLabel->setText(duration.toString());
}

void PhonePageWidget::setFocus() {
	_phoneComboBox->setFocus();
}
