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

#include "QtWenboxPlugin.h"

#include "presentation/qt/QtWengoPhone.h"

#include <control/CWengoPhone.h>
#include <control/wenbox/CWenboxPlugin.h>

#include <WidgetFactory.h>
#include <Object.h>

#include <QtGui>

QtWenboxPlugin::QtWenboxPlugin(CWenboxPlugin & cWenboxPlugin)
	: QObjectThreadSafe(),
	_cWenboxPlugin(cWenboxPlugin) {

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWenboxPlugin::initThreadSafe, this));
	postEvent(event);
}

void QtWenboxPlugin::initThreadSafe() {
	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cWenboxPlugin.getCWengoPhone().getPresentation();
	_phoneComboBox = qtWengoPhone->getPhoneComboBox();
}

void QtWenboxPlugin::keyNumberPressedEvent(const std::string & number) {
	typedef PostEvent1<void (const std::string &), std::string> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWenboxPlugin::keyNumberPressedEventThreadSafe, this, _1), number);
	postEvent(event);
}

void QtWenboxPlugin::keyNumberPressedEventThreadSafe(const std::string & number) {
	appendToPhoneComboBox(number);
}

void QtWenboxPlugin::keyPickUpPressedEvent() {
	clearPhoneComboBox();
}

void QtWenboxPlugin::keyPickUpPressedEventThreadSafe() {
}

void QtWenboxPlugin::keyHangUpPressedEvent() {
	clearPhoneComboBox();
}

void QtWenboxPlugin::keyHangUpPressedEventThreadSafe() {
}

void QtWenboxPlugin::appendToPhoneComboBox(const std::string & number) {
	_phoneNumberBuffer.append(number);
	_phoneComboBox->setEditText(QString::fromStdString(_phoneNumberBuffer));
}

void QtWenboxPlugin::clearPhoneComboBox() {
	_phoneNumberBuffer = "";
	_phoneComboBox->setEditText("");
}
