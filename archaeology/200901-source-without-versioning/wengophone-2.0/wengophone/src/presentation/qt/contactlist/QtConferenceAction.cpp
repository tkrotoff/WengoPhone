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

#include "QtConferenceAction.h"

#include <model/phonecall/PhoneCall.h>

QtConferenceAction::QtConferenceAction(const QString & text, QObject * parent)
	: QAction(text, parent) {
}

QtConferenceAction::QtConferenceAction(QObject * parent)
	: QAction(parent) {
}

QtConferenceAction::QtConferenceAction(const QIcon & icon, const QString & text, QObject * parent)
	: QAction(icon, text, parent) {
}

void QtConferenceAction::setPhoneCall(PhoneCall * phoneCall) {
	_phoneCall = phoneCall;
}

PhoneCall * QtConferenceAction::getPhoneCall() const {
	return _phoneCall;
}
