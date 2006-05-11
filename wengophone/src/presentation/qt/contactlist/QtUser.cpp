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

#include "QtUser.h"

#include <model/contactlist/ContactProfile.h>

#include <control/CWengoPhone.h>
#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>

#include <util/Logger.h>

#include "../QtWengoPhone.h"
#include "../webservices/sms/QtSms.h"

QtUser::QtUser(const std::string & contactId, CWengoPhone & cWengoPhone, QObject * parent)
: QObject(parent), _cWengoPhone(cWengoPhone) {
	_contactId = contactId;
	contactUpdated();
	_mouseOn = false;
	_openStatus = false;
}

void QtUser::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) {
	QMutexLocker locker(& _mutex);

	QRect r;
	QPixmap px;
	QtContactPixmap * spx;
	int x;
	bool parentItem = false;

	_painterRect = option.rect;

	spx = QtContactPixmap::getInstance();

/*
	QLinearGradient lg( QPointF(1,1), QPointF(option.rect.width(),1));
	lg.setSpread(QGradient::RepeatSpread);
	lg.setColorAt ( 0, option.palette.midlight().color());
	lg.setColorAt ( .8, QColor(210, 216, 234));
*/

	QColor lg(201, 201, 201);

	/*

	if (_mouseOn)
	{
		painter->fillRect(option.rect,QBrush(lg));
		painter->setPen(option.palette.text().color());
	}
    else
    {
        painter->setPen(option.palette.text().color() );
    }

	*/

	if ((option.state & QStyle::State_Selected) == QStyle::State_Selected) {
		QRect rect = option.rect;

		painter->fillRect(option.rect, QBrush(lg));
		painter->setPen(option.palette.text().color());
	}
	else {
		painter->setPen(option.palette.text().color());
	}

	// Draw the status pixmap
	QtContactPixmap::contactPixmap status = getStatus();

	px = spx->getPixmap(status);
	r = option.rect;
	x = r.left();

	_centeredPx_y = ((r.bottom() - r.top()) - px.size().height()) / 2;

	painter->drawPixmap(x, r.top() + _centeredPx_y, px);

	x += px.width() + 5;
	r.setLeft(x);

    /*
            Draw Functions icons

	*/

	/*
	x=option.rect.width();
	if ( (option.state & QStyle::State_Selected) )
	{
		px = spx->getPixmap(QtContactPixmap::ContactVideo);
		if (hasVideo())
		{
			x-=px.width();
			painter->drawPixmap (x,r.top(),px);
		}
		else
			x-=px.width();
		px = spx->getPixmap(QtContactPixmap::ContactCall);
		if (hasCall())
		{
			x-=px.width();
			painter->drawPixmap (x,r.top(),px);
		}
		else
			x-=px.width();

		px = spx->getPixmap(QtContactPixmap::ContactIM);
		if (hasIM())
		{
			px = spx->getPixmap(QtContactPixmap::ContactIM);
			x-=px.width();
			painter->drawPixmap (x,r.top(),px);
		}
		else
			x-=px.width();
		_iconsStartPosition = x;
	}
	*/

	_iconsStartPosition = 10000;

	// Draw the text
	painter->setFont(option.font);
	// Center the text vertically
	QRect textRect = r;
	_centeredText_y = ((r.bottom() - r.top()) - QFontMetrics(option.font).height()) / 2;
	textRect.setTop(_centeredText_y + textRect.top());

/*
	QFontMetrics fontMetrics = painter->fontMetrics();
	// get the size of a dot
	int dotsize = fontMetrics.width(QChar('.'));
	dotsize = dotsize * 3;

	QString showText;
	QByteArray textArray
	QString text = QString::fromUtf8(_cContact.getDisplayName().c_str());
	int maxTextWidth = _iconsStartPosition - dotsize;
	int textWidth = 0;

	for (int i = 0; i < text.size(); i++ ){
		if ( textWidth + fontMetrics.width( text.at(i) ) < maxTextWidth )
		{
			showText
		}
	}
*/

	QString text = QString::fromUtf8(_contactProfile.getDisplayName().c_str());
	painter->drawText(textRect, Qt::AlignLeft, text, 0);
}

QString QtUser::getId() const {
	return QString::fromStdString(_contactId);
}

QString QtUser::getUserName() const {
	//QMutexLocker locker(&_mutex);

	return QString::fromUtf8(_contactProfile.getDisplayName().c_str());
}

QtContactPixmap::contactPixmap QtUser::getStatus() const {
	//QMutexLocker locker(&_mutex);

	QtContactPixmap::contactPixmap status;

	switch (_contactProfile.getPresenceState()) {
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
		case EnumPresenceState::PresenceStateForward:
			status = QtContactPixmap::ContactForward;
			break;
		default:
			LOG_FATAL("Unknown state");

	}

	return status;
}

void QtUser::startChat() {
	//QMutexLocker locker(&_mutex);

	_cWengoPhone.getCUserProfile()->startIM(_contactId);
}

void QtUser::startSMS() {
	//QMutexLocker locker(&_mutex);

	QtWengoPhone * qwengophone = dynamic_cast < QtWengoPhone * > (_cWengoPhone.getPresentation());

	if (!qwengophone)
		return;

	if (qwengophone->getSms()) {
		QString mobilePhone = QString::fromStdString(_contactProfile.getMobilePhone());
		qwengophone->getSms()->setPhoneNumber(mobilePhone);
		qwengophone->getSms()->getWidget()->show();
	}
}

void QtUser::mouseClicked(const QPoint & pos, const QRect & rect) {
	//QMutexLocker locker(&_mutex);

/*
	QPixmap px;
	QtContactPixmap * spx;
	int x;

	spx = QtContactPixmap::getInstance();

	x = _painterRect.width();

	px = spx->getPixmap(QtContactPixmap::ContactVideo);
	if (_cContact.hasVideo())
	{
		x-=px.width();
		if ( (pos.x()>=x) && (pos.x()<=x+px.width()) ) {
			_wengoPhone.getCurrentUserProfile().makeCall(_cContact.getContact(), false);
		}
	}
	else
		x-=px.width();


	px = spx->getPixmap(QtContactPixmap::ContactCall);
	if (_cContact.hasCall())
	{
		x-=px.width();
		if ( (pos.x()>=x) && (pos.x()<=x+px.width()) ) {
			_wengoPhone.getCurrentUserProfile().makeCall(_cContact.getContact(), false);
		}
	}
	else
		x-=px.width();

	px = spx->getPixmap(QtContactPixmap::ContactIM);
	if (_cContact.hasIM())
	{
		x-=px.width();
		if ( (pos.x()>=x) && (pos.x()<=x+px.width()) ) {
			_wengoPhone.getCurrentUserProfile().startIM(_cContact.getContact());
		}
	}
	else
		x-=px.width();
*/
}

void QtUser::setButton(const Qt::MouseButton button) {
	_mouseButton = button;
}

Qt::MouseButton QtUser::getButton() const {
	return _mouseButton;
}

void QtUser::setOpenStatus(bool value) {
	_openStatus = value;
}

int QtUser::getHeight() const {
	if (_openStatus)
		return 90;
	return UserSize;
}

QString QtUser::getMobilePhone() const {
	//QMutexLocker locker(&_mutex);

	QString mphone = QString::fromStdString(_contactProfile.getMobilePhone());

	return mphone;
}

QString QtUser::getHomePhone() const {
	//QMutexLocker locker(&_mutex);

	QString hphone = QString::fromStdString(_contactProfile.getHomePhone());

	return hphone;
}

QString QtUser::getWorkPhone() const {
	//QMutexLocker locker(&_mutex);

	QString wphone = QString::fromStdString(_contactProfile.getWorkPhone());

	return wphone;
}

QString QtUser::getWengoPhoneNumber() const {
	//QMutexLocker locker(&_mutex);

	QString wphone = QString::fromStdString(_contactProfile.getWengoPhoneId());

	return wphone;
}

QString QtUser::getPreferredNumber() const {
	//QMutexLocker locker(&_mutex);

	QString wphone = QString::fromStdString(_contactProfile.getPreferredNumber());

	return wphone;
}

bool QtUser::havePhoneNumber() {
	if (!getMobilePhone().isEmpty())
		return true;
	if (!getHomePhone().isEmpty())
		return true;
	if (!getWorkPhone().isEmpty())
		return true;
	if (!getWengoPhoneNumber().isEmpty())
		return true;

	return false;
}

bool QtUser::hasIM() const {
	//QMutexLocker locker(&_mutex);

	return _contactProfile.hasIM();
}

bool QtUser::hasCall() const {
	//QMutexLocker locker(&_mutex);

	return _contactProfile.hasCall();
}

bool QtUser::hasVideo() const {
	//QMutexLocker locker(&_mutex);

	return _contactProfile.hasVideo();
}

void QtUser::startCall(const QString & number) {
	//QMutexLocker locker(&_mutex);

	_cWengoPhone.getCUserProfile()->makeCall(number.toStdString());
}

void QtUser::startCall() {
	_cWengoPhone.getCUserProfile()->makeContactCall(_contactId);
}

void QtUser::contactUpdated() {
	_contactProfile = _cWengoPhone.getCUserProfile()->getCContactList().getContactProfile(_contactId);
}

void QtUser::startFreeCall() {
	_cWengoPhone.getCUserProfile()->makeCall(_contactProfile.getFreePhoneNumber(), false);
}
