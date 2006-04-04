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

#include "QtUser.h"

#include <model/WengoPhone.h>
#include <model/profile/UserProfile.h>
#include <model/contactlist/Contact.h>

#include <control/CWengoPhone.h>

#include <util/Logger.h>
#include "../QtWengoPhone.h"
#include "../sms/QtSms.h"

QtUser::QtUser(CContact & cContact, WengoPhone & wengoPhone, QObject * parent)
	: QObject (parent), _cContact(cContact), _wengoPhone(wengoPhone)
{
	_mouseOn = false;
	_openStatus = false;
}

void QtUser::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index)
{
    QRect r;
	QPixmap px;
	QtContactPixmap * spx;
	int x;
    bool parentItem = false;

	_painterRect = option.rect;

	QLinearGradient lg( QPointF(1,1), QPointF(option.rect.width(),1));
	lg.setSpread(QGradient::RepeatSpread);
	lg.setColorAt ( 0, option.palette.midlight().color());
	lg.setColorAt ( .8, QColor(210, 216, 234));

	spx = QtContactPixmap::getInstance();

	if (_mouseOn)
	{
		painter->fillRect(option.rect,QBrush(lg));
		painter->setPen(option.palette.text().color());
	}
    else
    {
        painter->setPen(option.palette.text().color() );
    }

	// Draw the status pixmap
	QtContactPixmap::contactPixmap status = getStatus();

	px = spx->getPixmap(status);
    r = option.rect;
	x = r.left();

	_centeredPx_y = ((r.bottom()-r.top()) - px.size().height()) / 2;

	painter->drawPixmap (x,r.top() + _centeredPx_y,px);

	x+=px.width()+5;
	r.setLeft(x);
	// Draw the user
	painter->setFont(option.font);
	// Center the text vertically
	QRect textRect = r;
	_centeredText_y = ((r.bottom()-r.top()) - QFontMetrics(option.font).height() ) / 2;
	textRect.setTop(_centeredText_y+textRect.top());
	// Draw the text

	painter->drawText(textRect, Qt::AlignLeft, QString::fromStdString(_cContact.getDisplayName()), 0);

    /*
            Draw Functions icons
	*/
	x=option.rect.width();
	if (_mouseOn)
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

}

QString QtUser::getId() const
{
	return _userId;
}

void QtUser::setId(const QString & id)
{
	_userId = id;
}

QString	QtUser::getUserName() const
{
	return QString::fromStdString(_cContact.getDisplayName());
}

QtContactPixmap::contactPixmap QtUser::getStatus() const {
	QtContactPixmap::contactPixmap status;

	switch (_cContact.getPresenceState()) {
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
void QtUser::startChat(){
	_wengoPhone.getCurrentUserProfile().startIM(_cContact.getContact());
}

void QtUser::startSMS(){
	// _wengoPhone.
	QtWengoPhone * qwengophone = dynamic_cast<QtWengoPhone *>( _cContact.getCWengoPhone().getPresentation());
	if ( qwengophone->getSms() ){

		QString mobilePhone = QString::fromStdString( _cContact.getContact().getMobilePhone() );
		qwengophone->getSms()->setPhoneNumber(mobilePhone);
		qwengophone->getSms()->getWidget()->show();
	}
}

void QtUser::mouseClicked(const QPoint & pos, const QRect & rect)
{
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
			_wengoPhone.getCurrentUserProfile().makeCall(_cContact.getContact());
		}
	}
	else
		x-=px.width();


	px = spx->getPixmap(QtContactPixmap::ContactCall);
	if (_cContact.hasCall())
	{
		x-=px.width();
		if ( (pos.x()>=x) && (pos.x()<=x+px.width()) ) {
			_wengoPhone.getCurrentUserProfile().makeCall(_cContact.getContact());
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
}

void QtUser::setButton(const Qt::MouseButton button)
{
	_mouseButton = button;
}

 Qt::MouseButton QtUser::getButton() const
{
	return _mouseButton;
}

void QtUser::setOpenStatus(bool value)
{
	_openStatus = value;
}

int QtUser::getHeight() const
{
	if (_openStatus)
		return 90;
	return UserSize;
}
