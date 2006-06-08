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
#include <control/profile/CUserProfile.h>
#include <control/wenbox/CWenboxPlugin.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

#include <thread/Timer.h>

#include <QtGui>

QtWenboxPlugin::QtWenboxPlugin(CWenboxPlugin & cWenboxPlugin)
	: QObjectThreadSafe(NULL),
	_cWenboxPlugin(cWenboxPlugin) {

	_timer = new Timer();
	_timer->timeoutEvent += boost::bind(&QtWenboxPlugin::timeoutEventHandler, this);

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWenboxPlugin::initThreadSafe, this));
	postEvent(event);
}

QtWenboxPlugin::~QtWenboxPlugin() {
	_timer->stop();
	delete _timer;
}

void QtWenboxPlugin::initThreadSafe() {
	//FIXME: we should avoid to get QtWengoPhone via CWenboxPlugin and it should
	// be given in constructor.
	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cWenboxPlugin.getCWengoPhone().getPresentation();
	_phoneComboBox = qtWengoPhone->getPhoneComboBox();
}

void QtWenboxPlugin::phoneNumberBufferUpdatedEvent(const std::string & phoneNumberBuffer) {
	typedef PostEvent1<void (const std::string &), std::string> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWenboxPlugin::phoneNumberBufferUpdatedEventThreadSafe, this, _1), phoneNumberBuffer);
	postEvent(event);
}

void QtWenboxPlugin::phoneNumberBufferUpdatedEventThreadSafe(const std::string & phoneNumberBuffer) {
	static const unsigned TIMEOUT = 3 * 1000;	//3 seconds

	_phoneComboBox->setEditText(QString::fromStdString(phoneNumberBuffer));
	_timer->stop();
	if (!phoneNumberBuffer.empty()) {
		_timer->start(TIMEOUT, TIMEOUT);
	}
}

void QtWenboxPlugin::timeoutEventHandler() {
	_timer->stop();

	CWengoPhone & cWengoPhone = _cWenboxPlugin.getCWengoPhone();
	cWengoPhone.getCUserProfile()->makeCall(_phoneComboBox->currentText().toStdString());
}
