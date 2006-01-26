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

#include "QtWengoPhoneLogger.h"

#include "QtWengoPhone.h"
#include "control/CWengoPhone.h"

#include <WidgetFactory.h>
#include <Object.h>

#include <QtGui>

QtWengoPhoneLogger::QtWengoPhoneLogger(CWengoPhoneLogger & cWengoPhoneLogger)
	: QObjectThreadSafe(),
	_cWengoPhoneLogger(cWengoPhoneLogger) {

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhoneLogger::initThreadSafe, this));
	postEvent(event);
}

void QtWengoPhoneLogger::initThreadSafe() {
	_loggerWidget = WidgetFactory::create(":/forms/WengoPhoneLoggerWidget.ui", NULL);

	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cWengoPhoneLogger.getCWengoPhone().getPresentation();
	qtWengoPhone->setLogger(this);
}

void QtWengoPhoneLogger::addMessage(const std::string & message) {
	typedef PostEvent1<void (const std::string &), std::string> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhoneLogger::addMessageThreadSafe, this, _1), message);
	postEvent(event);
}

void QtWengoPhoneLogger::addMessageThreadSafe(std::string message) {
	static QListWidget * listWidget = Object::findChild<QListWidget *>(_loggerWidget, "listWidget");

	listWidget->addItem(message.c_str());
	listWidget->scrollToItem(listWidget->item(listWidget->count() - 1));
}

void QtWengoPhoneLogger::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhoneLogger::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtWengoPhoneLogger::updatePresentationThreadSafe() {
}
