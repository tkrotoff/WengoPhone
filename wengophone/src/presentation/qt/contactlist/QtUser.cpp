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

#include <presentation/PContact.h>

#include <model/WengoPhone.h>

QtUser::QtUser(PContact & pContact, WengoPhone & wengoPhone, QObject * parent) 
	: QObject (parent), _pContact(pContact), _wengoPhone(wengoPhone)
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
	
	/*
	QLinearGradient lg2( QPointF(1,1), QPointF(option.rect.width(),1));
	lg2.setSpread(QGradient::RepeatSpread);
	lg2.setColorAt ( 0, QColor(181,203,255));
	lg2.setColorAt ( .8, QColor(255, 255, 234));
	painter->fillRect(option.rect, lg2);
	*/
	
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
	painter->drawPixmap (x,r.top(),px);
	x+=px.width()+5;
	r.setLeft(x);
	// Draw the user
	painter->setFont(option.font);
	// Center the text vertically
	QRect textRect = r;
	_centeredText_y = ((r.bottom()-r.top()) - QFontMetrics(option.font).height() ) / 2;
	textRect.setTop(_centeredText_y+textRect.top());
	// Draw the text
	
	painter->drawText(textRect,Qt::AlignLeft,index.data().toString(),0);
    /*
            Draw Functions icons
        */
	x=option.rect.width();
	if (_mouseOn)
	{
/*
		if (status != QtContactPixmap::ContactNotAvailable)
		{
*/
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
/*
		}
*/
//		else
//			_iconsStartPosition = x;
	}
		
}

QString  QtUser::getId()
{
	return _userId;
}
void	QtUser::setId(const QString & id)
{
	_userId = id;
}

void QtUser::setUserName(const QString & uname)
{
	_userName = uname;
}
QString	QtUser::getUserName()
{
	return _userName;
}

QtContactPixmap::contactPixmap QtUser::getStatus(){
	QtContactPixmap::contactPixmap status;

	switch (_pContact.getPresenceState()) {
	case EnumPresenceState::PresenceStateOnline:
		status = QtContactPixmap::ContactOnline;
		break;
	case EnumPresenceState::PresenceStateOffline:
		status = QtContactPixmap::ContactInvisible;
		break;
	default:
		status = QtContactPixmap::ContactNotAvailable;
	}

	return status;
}

void QtUser::mouseClicked(const QPoint & pos, const QRect & rect)
{
	QPixmap px;
	QtContactPixmap * spx;	
	int x;
	
	spx = QtContactPixmap::getInstance();
	
	//x=rect.width();
	x = _painterRect.width();
	
	
	
	px = spx->getPixmap(QtContactPixmap::ContactVideo);
	if (_pContact.hasVideo())
	{
		x-=px.width();
		if ( (pos.x()>=x) && (pos.x()<=x+px.width()) ) {
			qDebug() << "Video clicked";
			_wengoPhone.makeCall(_pContact.getContact());
		}
	}
	else
		x-=px.width();


	px = spx->getPixmap(QtContactPixmap::ContactCall);		
	if (_pContact.hasCall())
	{
		x-=px.width();
		if ( (pos.x()>=x) && (pos.x()<=x+px.width()) ) {
			qDebug() << "Call clicked";
			_wengoPhone.makeCall(_pContact.getContact());
		}
	}
	else
		x-=px.width();

	px = spx->getPixmap(QtContactPixmap::ContactIM);		
	if (_pContact.hasIM())
	{
		x-=px.width();
		if ( (pos.x()>=x) && (pos.x()<=x+px.width()) ) {
			qDebug() << "IM clicked";
			_wengoPhone.startIM(_pContact.getContact());
		}
	}
	else
		x-=px.width();
}

void QtUser::setButton(const Qt::MouseButton button)
{
	_mouseButton = button;
}

 Qt::MouseButton QtUser::getButton()
{
	return _mouseButton;
}

void QtUser::setOpenStatus(bool value)
{
	_openStatus = value;
}

int QtUser::getHeight()
{
	if (_openStatus)
		return 90;
	return UserSize;
}
