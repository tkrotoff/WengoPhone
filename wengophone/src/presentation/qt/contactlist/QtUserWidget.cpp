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
#include "QtContactListManager.h"
#include "QtContact.h"

#include <model/contactlist/ContactProfile.h>

#include <control/CWengoPhone.h>
#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <presentation/qt/contactlist/QtContactListManager.h>
#include <presentation/qt/profile/QtProfileDetails.h>

#include <util/Picture.h>

#include <qtutil/WidgetFactory.h>

#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QPainter>

#include <util/Logger.h>

const QString QtUserWidget::AVATAR_BACKGROUND=":/pics/fond_avatar.png";

QtUserWidget::QtUserWidget(const std::string & contactId,
	CWengoPhone & cWengoPhone, QWidget * parent, Qt::WFlags f)
	: QWidget(parent, f), _cWengoPhone(cWengoPhone) {

	_contactId = contactId;
	contactProfileUpdated();

	_widget = new QWidget(NULL);
	_ui.setupUi(_widget);

	QGridLayout * layout = new QGridLayout();
	layout->addWidget(_widget);
	layout->setMargin(0);
	setLayout(layout);

	_ui.avatarLabel->setPixmap(createAvatar());

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
	QtContactListManager * ul = QtContactListManager::getInstance();
	ul->startCall(QString::fromStdString(_contactId));
}

void QtUserWidget::smsButtonClicked() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	ul->startSMS(QString::fromStdString(_contactId));
}

void QtUserWidget::chatButtonClicked() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	ul->startChat(QString::fromStdString(_contactId));
}

QPixmap QtUserWidget::getIcon() const {
	Picture picture = _contactProfile.getIcon();
	std::string data = picture.getData();

	QPixmap result;
	result.loadFromData((uchar *) data.c_str(), data.size());

	return result;
}

void QtUserWidget::contactProfileUpdated() {
	_contactProfile = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().getContactProfile(_contactId);
}

QLabel * QtUserWidget::getAvatarLabel() const {
	return _ui.avatarLabel;
}

void QtUserWidget::mobileButtonClicked() {
	if (_ui.cellPhoneLabel->text() != tr("No cell phone number set")) {
		QtContactListManager * ul = QtContactListManager::getInstance();
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
	if (_ui.homePhoneLabel->text() != tr("No phone number set")) {
		QtContactListManager * ul = QtContactListManager::getInstance();
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

QPixmap QtUserWidget::createAvatar() {
	QPixmap background = QPixmap(AVATAR_BACKGROUND);
	QPixmap avatar = getIcon();

	if (!avatar.isNull()) {
		QRect rect = _ui.avatarLabel->rect();
		QPainter pixpainter(&background);
		pixpainter.drawPixmap(5, 5, avatar.scaled(60, 60));
		pixpainter.end();
	}
	return background;
}
