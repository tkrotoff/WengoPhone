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

#include "QtUserWidget.h"

#include "QtUserWidgetAvatarManager.h"
#include "QtUserList.h"
#include "QtUser.h"

#include <model/contactlist/ContactProfile.h>

#include <control/CWengoPhone.h>
#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <presentation/qt/contactlist/QtUserList.h>
#include <presentation/qt/profile/QtProfileDetails.h>

#include <util/Picture.h>
#include <qtutil/WidgetFactory.h>

#include <QtGui>

#include <util/Logger.h>

QtUserWidget::QtUserWidget(const std::string & contactId, 
	CWengoPhone & cWengoPhone, QWidget * parent, Qt::WFlags f)
  : QWidget(parent, f), _cWengoPhone(cWengoPhone) {
	
  //contactProfileUpdated();
  
	_contactId = contactId;
	contactProfileUpdated();

	_widget = new QWidget(NULL);
	_ui.setupUi(_widget);

	QGridLayout * layout = new QGridLayout();
	layout->addWidget(_widget);
	layout->setMargin(0);
	setLayout(layout);

	_avatarManager = new QtUserWidgetAvatarManager(this, _ui.avatarLabel);
	_ui.avatarLabel->installEventFilter(_avatarManager);

	QString str = QString::fromUtf8(_contactProfile.getHomePhone().c_str());
	if (!str.isEmpty()) {
		_ui.homePhoneLabel->setText(str);
	}

	str = QString::fromUtf8(_contactProfile.getMobilePhone().c_str());
	if (!str.isEmpty()) {
		_ui.cellPhoneLabel->setText(str);
	}
	else {
		_ui.smsButton->setEnabled(false);
	}

	if (!_contactProfile.hasFreeCall()) {
		_ui.callButton->setEnabled(false);
	}

	if (!_contactProfile.hasIM()) {
		_ui.chatButton->setEnabled(false);
	}

	connect(_ui.callButton, SIGNAL(clicked()), SLOT(callButtonClicked()));
	connect(_ui.chatButton, SIGNAL(clicked()), SLOT(chatButtonClicked()));
	connect(_ui.smsButton, SIGNAL(clicked()), SLOT(smsButtonClicked()));
	connect(_ui.landLineButton, SIGNAL(clicked()), SLOT(landLineButtonClicked()));
	connect(_ui.mobileButton, SIGNAL(clicked()), SLOT(mobileButtonClicked()));
}

void QtUserWidget::callButtonClicked() {
	QtUserList * ul = QtUserList::getInstance();
	ul->startCall(QString::fromStdString(_contactId));
}

void QtUserWidget::smsButtonClicked() {
	QtUserList * ul = QtUserList::getInstance();
	ul->startSMS(QString::fromStdString(_contactId));
}

void QtUserWidget::chatButtonClicked() {
	QtUserList * ul = QtUserList::getInstance();
	ul->startChat(QString::fromStdString(_contactId));
}

QPixmap QtUserWidget::getIcon() const {
	QMutexLocker locker(& _mutex);

	Picture picture = _contactProfile.getIcon();
	std::string data = picture.getData();

	QPixmap result;
	result.loadFromData((uchar *) data.c_str(), data.size());

	return result;
}

void QtUserWidget::contactProfileUpdated() {
	QMutexLocker locker(& _mutex);

	_contactProfile = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().getContactProfile(_contactId);
}

QLabel * QtUserWidget::getAvatarLabel() const {
	return _ui.avatarLabel;
}


void QtUserWidget::mobileButtonClicked() {
	LOG_DEBUG("\n\n\n\n\n" + _ui.cellPhoneLabel->text().toStdString() + "\n\n");
	
	
	if (_ui.cellPhoneLabel->text() != tr("No cell phone number set")) {
		QtUserList * ul = QtUserList::getInstance();
		ul->startCall(QString::fromStdString(_contactId), _ui.cellPhoneLabel->text());
	} else {
		ContactProfile contactProfile =
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().getContactProfile(_text.toStdString());
		QtProfileDetails qtProfileDetails(*_cWengoPhone.getCUserProfileHandler().getCUserProfile(), contactProfile, this);
		if( qtProfileDetails.show() ) {
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().updateContact(contactProfile);
		}
	}
}

void QtUserWidget::landLineButtonClicked() {
	LOG_DEBUG("\n\n\n\n\n" + _ui.homePhoneLabel->text().toStdString() + "\n\n");

	if (_ui.homePhoneLabel->text() != tr("No phone number set")) {
		QtUserList * ul = QtUserList::getInstance();
		ul->startCall(QString::fromStdString(_contactId), _ui.homePhoneLabel->text());
	} else {
		ContactProfile contactProfile = 
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().getContactProfile(_text.toStdString());
		QtProfileDetails qtProfileDetails(*_cWengoPhone.getCUserProfileHandler().getCUserProfile(), contactProfile, this);
		if( qtProfileDetails.show() ) {
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().updateContact(contactProfile);
		}
	}
}
