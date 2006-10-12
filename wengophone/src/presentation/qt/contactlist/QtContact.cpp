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

#include "QtContact.h"

#include <control/CWengoPhone.h>
#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/webservices/sms/QtSms.h>

#include <util/Logger.h>

#include <QtGui/QtGui>

//const QColor QtContact::SELECTION_BACKGROUND = QColor(201, 201, 201);
const QColor QtContact::SELECTION_BACKGROUND = QColor(193, 223, 255);

QtContact::QtContact(const std::string & contactId, CWengoPhone & cWengoPhone, QObject * parent)
	: QObject(parent),
	_cWengoPhone(cWengoPhone) {

	_contactId = contactId;
	contactUpdated();
	_mouseOn = false;
	_openStatus = false;
}

void QtContact::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & ) {
	QMutexLocker locker(&_mutex);

	QtContactPixmap * spx = QtContactPixmap::getInstance();

	QColor selectedBackground = SELECTION_BACKGROUND;

	if ((option.state & QStyle::State_Selected) == QStyle::State_Selected) {
		paintSelection(painter,option);
	} else {
		painter->setPen(option.palette.text().color());
	}

	// Draw the status pixmap
	QtContactPixmap::ContactPixmap status = getStatus();

	QPixmap px = spx->getPixmap(status);
	QRect painterRect = option.rect;
	int x = painterRect.left();

	_centeredPx_y = ((painterRect.bottom() - painterRect.top()) - px.size().height()) / 2;

	painter->drawPixmap(x, painterRect.top() + _centeredPx_y, px);

	x += px.width() + 5;
	painterRect.setLeft(x);

	// Draw the text
	painter->setFont(option.font);
	// Center the text vertically
	QRect textRect = painterRect;
	_centeredText_y = ((painterRect.bottom() - painterRect.top()) - QFontMetrics(option.font).height()) / 2;
	textRect.setTop(_centeredText_y + textRect.top());

	QString text = QString::fromUtf8(_contactProfile.getDisplayName().c_str());
	painter->drawText(textRect, Qt::AlignLeft, text, 0);
}

void QtContact::paintSelection(QPainter * painter, const QStyleOptionViewItem & option) {
	QRect qrect = option.rect;
	qrect.adjust(0, 0, 1, 1);
	painter->fillRect(qrect, QBrush(SELECTION_BACKGROUND));
	painter->setPen(option.palette.text().color());
}

QString QtContact::getId() const {
	return QString::fromStdString(_contactId);
}

QString QtContact::getUserName() const {
	return QString::fromUtf8(_contactProfile.getDisplayName().c_str());
}

QtContactPixmap::ContactPixmap QtContact::getStatus() const {
	QtContactPixmap::ContactPixmap status;

	switch (_contactProfile.getPresenceState()) {
	case EnumPresenceState::PresenceStateUnknown:
		status = QtContactPixmap::ContactUnknown;
		break;
	case EnumPresenceState::PresenceStateOnline:
		status = QtContactPixmap::ContactOnline;
		break;
	case EnumPresenceState::PresenceStateOffline:
		status = QtContactPixmap::ContactOffline;
		break;
	case EnumPresenceState::PresenceStateDoNotDisturb:
		status = QtContactPixmap::ContactDND;
		break;
	case EnumPresenceState::PresenceStateAway:
		status = QtContactPixmap::ContactAway;
		break;
	case EnumPresenceState::PresenceStateInvisible:
		status = QtContactPixmap::ContactInvisible;
		break;
	case EnumPresenceState::PresenceStateUnavailable:
		status = QtContactPixmap::ContactNoStatus;
		break;
	default:
		LOG_FATAL("unknown state=" + String::fromNumber(_contactProfile.getPresenceState()));
	}
	return status;
}

void QtContact::startChat() {
	_cWengoPhone.getCUserProfileHandler().getCUserProfile()->startIM(_contactId);
}

void QtContact::startSMS() {
	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cWengoPhone.getPresentation();
	QtSms * sms = qtWengoPhone->getQtSms();
	if (sms) {
		sms->setPhoneNumber(getMobilePhone());
		sms->setText(QString::null);
		sms->setSignature(QString::null);
		sms->getWidget()->show();
	}
}

void QtContact::setButton(const Qt::MouseButton button) {
	_mouseButton = button;
}

Qt::MouseButton QtContact::getButton() const {
	return _mouseButton;
}

void QtContact::setOpenStatus(bool value) {
	_openStatus = value;
}

int QtContact::getHeight() const {
	if (_openStatus) {
		return 95;
	}
	return UserSize;
}

QString QtContact::getMobilePhone() const {
	return QString::fromStdString(_contactProfile.getMobilePhone());
}

QString QtContact::getHomePhone() const {
	return QString::fromStdString(_contactProfile.getHomePhone());
}

QString QtContact::getWorkPhone() const {
	return QString::fromStdString(_contactProfile.getWorkPhone());
}

QString QtContact::getWengoPhoneNumber() const {
	return QString::fromStdString(_contactProfile.getFirstAvailableWengoId());
}

QString QtContact::getPreferredNumber() const {
	return QString::fromStdString(_contactProfile.getPreferredNumber());
}

bool QtContact::hasPhoneNumber() const {
	if (!getMobilePhone().isEmpty()) {
		return true;
	}
	if (!getHomePhone().isEmpty()) {
		return true;
	}
	if (!getWorkPhone().isEmpty()) {
		return true;
	}
	if (!getWengoPhoneNumber().isEmpty()) {
		return true;
	}
	return false;
}

bool QtContact::hasIM() const {
	return _contactProfile.hasIM();
}

bool QtContact::hasFreeCall() const {
	return _contactProfile.hasFreeCall();
}

bool QtContact::hasCall() const {
	return _contactProfile.hasCall();
}

bool QtContact::hasVideo() const {
	return _contactProfile.hasVideo();
}

void QtContact::startCall(const QString & number) {
	_cWengoPhone.getCUserProfileHandler().getCUserProfile()->makeCall(number.toStdString());
}

void QtContact::startCall() {
	_cWengoPhone.getCUserProfileHandler().getCUserProfile()->makeContactCall(_contactId);
}

void QtContact::contactUpdated() {
	_contactProfile = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().getContactProfile(_contactId);
}

void QtContact::startFreeCall() {
	_cWengoPhone.getCUserProfileHandler().getCUserProfile()->makeCall(_contactProfile.getFirstFreePhoneNumber());
}

QString QtContact::getDisplayName() const {
	return QString::fromStdString(_contactProfile.getDisplayName());
}

EnumPresenceState::PresenceState QtContact::getPresenceState() const {
	return _contactProfile.getPresenceState();
}
