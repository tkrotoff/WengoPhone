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

#include "SystrayPopupWindow.h"

#include "MyWidgetFactory.h"

#include "contact/Contact.h"
#include "AudioCallManager.h"
#include "AudioCall.h"
#include "SessionWindow.h"
#include "PhonePageWidget.h"
#include "util/emit_signal.h"
#include "sip/InputCallSipState.h"

#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qtimer.h>
#include <qhbox.h>

#include <iostream>
using namespace std;

#include <cmath>

const int SystrayPopupWindow::TIMER_TIMEOUT = 100;

SystrayPopupWindow::SystrayPopupWindow() {
	_systrayPopupWindow = (QDialog *) MyWidgetFactory::create("SystrayPopupWindowForm.ui");

	_phoneNumberLabel = (QLabel *) _systrayPopupWindow->child("phoneNumberLabel", "QLabel");

	QPushButton * audioCallButton = (QPushButton *) _systrayPopupWindow->child("audioCallButton", "QPushButton");
	connect(audioCallButton, SIGNAL(clicked()),
		this, SLOT(audioCallButtonClicked()));

	QPushButton * videoCallButton = (QPushButton *) _systrayPopupWindow->child("videoCallButton", "QPushButton");
	connect(videoCallButton, SIGNAL(clicked()),
		this, SLOT(videoCallButtonClicked()));

	QPushButton * hangUpButton = (QPushButton *) _systrayPopupWindow->child("hangUpButton", "QPushButton");
	connect(hangUpButton, SIGNAL(clicked()),
		this, SLOT(rejectCallButtonClicked()));

	_timeoutOpacity = 0;

	int desktopWidth = QApplication::desktop()->width() - 10;
	int desktopHeight = QApplication::desktop()->height() - 50;

	int popupWidth = _systrayPopupWindow->width();
	int popupHeight = _systrayPopupWindow->height();

	QPoint position(desktopWidth - popupWidth,
			desktopHeight - popupHeight);

	_systrayPopupWindow->reparent(0,
				WStyle_Customize | WDestructiveClose | WStyle_Tool | WStyle_Title |
				WStyle_StaysOnTop | WX11BypassWM | WStyle_SysMenu,
				position);

	//Timer
	_timer = new QTimer(_systrayPopupWindow);
	_timer->start(TIMER_TIMEOUT);
	connect(_timer, SIGNAL(timeout()),
		this, SLOT(showPopupProgressing()));
}

SystrayPopupWindow::~SystrayPopupWindow() {
}

void SystrayPopupWindow::setTitle(const QString & title) {
	_systrayPopupWindow->setCaption(title);
}

void SystrayPopupWindow::setContact(const QString & phoneNumber, Contact * contact) {
	QString label;
	if (contact != NULL && !contact->toString().isEmpty()) {
		label = contact->toString();
	} else {
		label = phoneNumber;
	}
	_phoneNumberLabel->setText(label);
}

void SystrayPopupWindow::showPopupProgressing() {
	static const double timeout = 15000;	//After 15 seconds we hide the popup
	static const double top = timeout / 2;	//Top of the curve

	_timeoutOpacity += TIMER_TIMEOUT;

	//f(x) = -(((x-5000)/5000)^2)+1
	double opacity = - ( pow((_timeoutOpacity - top) / top, 2) ) + 1.2;

	//So that the popup stays opaque
	if (opacity > 1.0) {
		opacity = 1.0;
	}

	_systrayPopupWindow->setWindowOpacity(opacity);
	_systrayPopupWindow->show();

	if (_timeoutOpacity == timeout) {
		_timer->stop();
		_systrayPopupWindow->hide();
	}
}

void SystrayPopupWindow::audioCallButtonClicked() {
	AudioCall * call = AudioCallManager::getInstance().getActiveAudioCall();
	if (call) {
		SipState * state = call->getSipState();
		if (state->getKind() == SipState::input_state) {
			((InputCallSipState *) state)->acceptAudioCall();
		}
	}
	_timer->stop();
	_systrayPopupWindow->hide();
}

void SystrayPopupWindow::videoCallButtonClicked() {
	AudioCall * call = AudioCallManager::getInstance().getActiveAudioCall();
	if (call) {
		SipState * state = call->getSipState();
		if (state->getKind() == SipState::input_state) {
			((InputCallSipState *) state)->acceptVideoCall();
			call->getSessionWindow().showCurrentCallPhonePage();
		}
	}
	_timer->stop();
	_systrayPopupWindow->hide();
}

void SystrayPopupWindow::rejectCallButtonClicked() {
	AudioCall * call = AudioCallManager::getInstance().getActiveAudioCall();
	if (call) {
		SipState * state = call->getSipState();
		if (state->getKind() == SipState::input_state) {
			((InputCallSipState *) state)->rejectCall();
		}
	}
	_timer->stop();
	_systrayPopupWindow->hide();
}
