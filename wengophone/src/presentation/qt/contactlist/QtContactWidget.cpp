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

#include "QtContactWidget.h"

#include "ui_ContactWidget.h"

#include "QtContactListManager.h"
#include "QtContact.h"

#include <presentation/qt/contactlist/QtContactListManager.h>
#include <presentation/qt/profile/QtProfileDetails.h>

#include <control/CWengoPhone.h>
#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <model/contactlist/ContactProfile.h>

#include <util/OWPicture.h>
#include <util/Logger.h>

#include <qtutil/PixmapMerging.h>

#include <QtGui/QtGui>

static const std::string AVATAR_BACKGROUND = ":/pics/avatar_background.png";

QtContactWidget::QtContactWidget(const std::string & contactId,
	CWengoPhone & cWengoPhone, QWidget * parent)
	: QWidget(parent),
	_cWengoPhone(cWengoPhone) {

	_contactId = contactId;
	contactProfileUpdated();

	_ui = new Ui::ContactWidget();
	_ui->setupUi(this);

	createAvatar();

	QString str = QString::fromUtf8(_contactProfile.getHomePhone().c_str());
	if (!str.isEmpty()) {
		_ui->landlineButton->setText(str);
	}
	str = QString::fromUtf8(_contactProfile.getMobilePhone().c_str());
	if (!str.isEmpty()) {
		_ui->mobileButton->setText(str);
	}
	else {
		_ui->smsButton->setEnabled(false);
	}
	if (!_contactProfile.hasFreeCall()) {
		_ui->callButton->setEnabled(false);
	}
	if (!_contactProfile.hasIM()) {
		_ui->chatButton->setEnabled(false);
	}
	connect(_ui->callButton, SIGNAL(clicked()), SLOT(callButtonClicked()));
	connect(_ui->chatButton, SIGNAL(clicked()), SLOT(chatButtonClicked()));
	connect(_ui->smsButton, SIGNAL(clicked()), SLOT(smsButtonClicked()));
	connect(_ui->landlineButton, SIGNAL(clicked()), SLOT(landlineButtonClicked()));
	connect(_ui->mobileButton, SIGNAL(clicked()), SLOT(mobileButtonClicked()));
	connect(_ui->avatarButton, SIGNAL(clicked()), SLOT(avatarButtonClicked()));
}

QtContactWidget::~QtContactWidget() {
	delete _ui;
}

void QtContactWidget::callButtonClicked() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	ul->startCall(QString::fromStdString(_contactId));
}

void QtContactWidget::smsButtonClicked() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	ul->startSMS(QString::fromStdString(_contactId));
}

void QtContactWidget::chatButtonClicked() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	ul->startChat(QString::fromStdString(_contactId));
}

void QtContactWidget::contactProfileUpdated() {
	_contactProfile = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().getContactProfile(_contactId);
}

void QtContactWidget::updateToolTips() {

	QtContactListManager * ul = QtContactListManager::getInstance();

	if (!ul->getMobilePhone(QString::fromStdString(_contactId)).isEmpty()) {
		_ui->mobileButton->setToolTip(tr("Click here to call"));
	} else {
		_ui->mobileButton->setToolTip(tr("Click here to set a phone number"));
	}

	if (!ul->getHomePhone(QString::fromStdString(_contactId)).isEmpty()) {
		_ui->landlineButton->setToolTip(tr("Click here to call"));
	} else {
		_ui->landlineButton->setToolTip(tr("Click here to set a phone number"));
	}

	_ui->avatarButton->setToolTip(tr("Click here to edit the contact"));

	if (_ui->smsButton->isEnabled()) {
		_ui->smsButton->setToolTip(tr("Click here to send a SMS"));
	} else {
		_ui->smsButton->setToolTip("");
	}

	if (_ui->chatButton->isEnabled()) {
		_ui->chatButton->setToolTip(tr("Click here to start a chat"));
	} else {
		_ui->chatButton->setToolTip("");
	}

	if (_ui->callButton->isEnabled()) {
		_ui->callButton->setToolTip(tr("Click here to start a free call"));
	} else {
		_ui->callButton->setToolTip("");
	}
}

void QtContactWidget::mobileButtonClicked() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	if (!ul->getMobilePhone(QString::fromStdString(_contactId)).isEmpty()) {
		ul->startCall(QString::fromStdString(_contactId), _ui->mobileButton->text());
	} else {
		showContactProfile();
	}
}

void QtContactWidget::landlineButtonClicked() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	if (!ul->getHomePhone(QString::fromStdString(_contactId)).isEmpty()) {
		ul->startCall(QString::fromStdString(_contactId), _ui->landlineButton->text());
	} else {
		showContactProfile();
	}
}

void QtContactWidget::avatarButtonClicked() {
	showContactProfile();
}

void QtContactWidget::showContactProfile() {
	ContactProfile contactProfile =
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().getContactProfile(_text.toStdString());
	QtProfileDetails qtProfileDetails(*_cWengoPhone.getCUserProfileHandler().getCUserProfile(),
		contactProfile, this, tr("Edit Contact"));
	if (qtProfileDetails.show()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().updateContact(contactProfile);
	}
}

void QtContactWidget::createAvatar() {
	std::string foregroundPixmapData = _contactProfile.getIcon().getData();
	_ui->avatarButton->setIcon(PixmapMerging::merge(foregroundPixmapData, AVATAR_BACKGROUND));
}

void QtContactWidget::paintEvent(QPaintEvent *) {
	QPalette p = palette();
	QRect r = rect();

	QLinearGradient lg(QPointF(1, r.top()), QPointF(1, r.bottom()));
	QColor dest = QColor(193, 223, 255);
	lg.setColorAt(0, dest);
	float red = ((float) dest.red()) / 1.4f;
	float blue = ((float) dest.blue()) / 1.4f;
	float green = ((float) dest.green()) / 1.4f;
	dest = QColor((int) red, (int) green, (int) blue);
	lg.setColorAt(1, dest);
	QPainter painter(this);
	painter.fillRect(r, QBrush(lg));
	paintContact(&painter, r);
	painter.end();
}

void QtContactWidget::paintContact(QPainter * painter, const QRect & rect) {
	QtContactListManager * ul = QtContactListManager::getInstance();

	QtContact * qtContact = ul->getContact(QString::fromStdString(_contactId));
	if (!qtContact) {
		return;
	}

	QPalette p = palette();
	QtContactPixmap * spx = QtContactPixmap::getInstance();
	painter->setPen(p.text().color());
	//Draw the status pixmap
	QPixmap px = spx->getPixmap(qtContact->getStatus());
	QRect r = rect;
	int x = r.left();

	int centeredPx_y = (QtContact::UserSize - px.size().height()) / 2;

	painter->drawPixmap(x, r.top() + centeredPx_y, px);
	x += px.width() + 5;
	r.setLeft(x);

	//Draw the user
	QRect textRect = r;
	int centeredText_y = (QtContact::UserSize - QFontMetrics(font()).height()) / 2;
	textRect.setTop(textRect.top() + centeredText_y);

	QFont font = painter->font();
	font.setBold(true);
	painter->setFont(font);
	painter->drawText(textRect, Qt::AlignLeft, qtContact->getUserName(), 0);
	font.setBold(false);
	painter->setFont(font);

	updateToolTips();
}
