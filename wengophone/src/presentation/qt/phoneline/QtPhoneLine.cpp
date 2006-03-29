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

#include "QtPhoneLine.h"

#include <presentation/qt/phonecall/QtPhoneCall.h>
#include <presentation/qt/QtWengoPhone.h>
#include <control/phoneline/CPhoneLine.h>
#include <control/CWengoPhone.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

#include <util/Logger.h>

#include <QtGui>

QtPhoneLine::QtPhoneLine(CPhoneLine & cPhoneLine)
	: QObjectThreadSafe(),
	_cPhoneLine(cPhoneLine) {

	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cPhoneLine.getCWengoPhone().getPresentation();
	_phoneLineStateLabel = qtWengoPhone->getPhoneLineStateLabel();

	stateChangedEvent += boost::bind(&QtPhoneLine::stateChangedEventHandler, this, _1);

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneLine::initThreadSafe, this));
	postEvent(event);
}

void QtPhoneLine::initThreadSafe() {
}

void QtPhoneLine::updatePresentation() {
}

void QtPhoneLine::updatePresentationThreadSafe() {
}

void QtPhoneLine::addPhoneCall(QtPhoneCall * qtPhoneCall) {
	//_stateLabel->setText(qtPhoneCall->);
}

void QtPhoneLine::stateChangedEventHandler(EnumPhoneLineState::PhoneLineState state) {
	typedef PostEvent1<void (EnumPhoneLineState::PhoneLineState), EnumPhoneLineState::PhoneLineState> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtPhoneLine::stateChangedEventHandlerThreadSafe, this, _1), state);
	postEvent(event);
}

void QtPhoneLine::stateChangedEventHandlerThreadSafe(EnumPhoneLineState::PhoneLineState state) {
	QString tooltip;
	QString pixmap;

	switch (state) {
	case EnumPhoneLineState::PhoneLineStateDefault:
		tooltip = tr("Not connected");
		pixmap = ":/pics/statusbar_sip_error.png";
		break;

	case EnumPhoneLineState::PhoneLineStateServerError:
		tooltip = tr("An error occured");
		pixmap = ":/pics/statusbar_sip_error.png";
		break;

	case EnumPhoneLineState::PhoneLineStateTimeout:
		tooltip = tr("An error occured");
		pixmap = ":/pics/statusbar_sip_error.png";
		break;

	case EnumPhoneLineState::PhoneLineStateOk:
		tooltip = tr("Register done");
		pixmap = ":/pics/statusbar_sip.png";
		break;

	case EnumPhoneLineState::PhoneLineStateClosed:
		tooltip = tr("Unregister done");
		pixmap = ":/pics/statusbar_sip_error.png";
		break;

	default:
		LOG_FATAL("unknown state=" + String::fromNumber(state));
	};

	_phoneLineStateLabel->setPixmap(pixmap);
	_phoneLineStateLabel->setToolTip(tooltip);
}
