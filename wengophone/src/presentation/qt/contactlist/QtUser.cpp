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

QtUser::QtUser(PContact & pContact, QObject * parent) 
	: QObject (parent), _pContact(pContact)
{
	_mouseOn = false;
}

void QtUser::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index)
{
	QRect r;
	QPixmap px;
	QtContactPixmap * spx;
	int x;
	bool parentItem = false;
	
	QLinearGradient lg( QPointF(1,1), QPointF(option.rect.width(),1));
	lg.setSpread(QGradient::RepeatSpread);
	lg.setColorAt ( 0, option.palette.midlight().color());
	lg.setColorAt ( .8, QColor(210, 216, 234));
	
	spx = QtContactPixmap::getInstance();
	/*
	if ( (option.state & QStyle::State_Selected)){
		painter->fillRect(option.rect,option.palette.highlight());
		painter->setPen(option.palette.highlightedText().color() );
	}
	*/
	if (_mouseOn)
	{
		// painter->fillRect(option.rect,option.palette.midlight());
		painter->fillRect(option.rect,QBrush(lg));
		painter->setPen(option.palette.text().color());
	}
	else
	{
		painter->setPen(option.palette.text().color() );
	}
	// Draw the status pixmap
	px = spx->getPixmap(_status);
	
	r = option.rect;
	x = r.left();
	painter->drawPixmap (x,r.top(),px);
	x+=px.width()+5;
	r.setLeft(x);
	
	if (  ! index.child(0,0).isValid() ){
		painter->setFont(option.font);
		painter->drawText(r,Qt::AlignLeft,index.data().toString(),0);
	}
	else
	{
		QFont f = option.font;
		f.setBold(true);
		painter->setFont(f);
		painter->drawText(r,Qt::AlignLeft,index.data().toString(),0);
		parentItem=true;
	}
	/*
	    Draw Functions icons
	*/
	x=option.rect.width();
	if (_mouseOn)
	{
		if (_status != QtContactPixmap::ContactNotAvailable)
		{
			
			px = spx->getPixmap(QtContactPixmap::ContactVideo);
			if (_pContact.haveVideo())
			{
				x-=px.width();
				painter->drawPixmap (x,r.top(),px);
			}
			else
				x-=px.width();
				
			px = spx->getPixmap(QtContactPixmap::ContactCall);
			if (_pContact.haveCall())
			{
				x-=px.width();
				painter->drawPixmap (x,r.top(),px);
			}
			else
				x-=px.width();
				
			px = spx->getPixmap(QtContactPixmap::ContactIM);
			if (_pContact.haveIM())
			{
				px = spx->getPixmap(QtContactPixmap::ContactIM);
				x-=px.width();
				painter->drawPixmap (x,r.top(),px);
			}
			else
				x-=px.width();
			_iconsStartPosition = x;
		}
		else
			_iconsStartPosition = x;
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

void QtUser::setStatus(QtContactPixmap::contactPixmap status)
{
	_status = status;
}

QtContactPixmap::contactPixmap QtUser::getStatus(){
	return _status;
}

void QtUser::mouseClicked(const QPoint & pos, const QRect & rect)
{
	QPixmap px;
	QtContactPixmap * spx;	
	int x;
	
	spx = QtContactPixmap::getInstance();
	
	x=rect.width();
	
	
	
	px = spx->getPixmap(QtContactPixmap::ContactVideo);
	if (_pContact.haveVideo())
	{
		x-=px.width();
		if ( (pos.x()>=x) && (pos.x()<=x+px.width()) ) {
			qDebug() << "Video clicked";
			_pContact.placeVideoCall();
		}
	}
	else
		x-=px.width();


	px = spx->getPixmap(QtContactPixmap::ContactCall);		
	if (_pContact.haveCall())
	{
		x-=px.width();
		if ( (pos.x()>=x) && (pos.x()<=x+px.width()) ) {
			qDebug() << "Call clicked";
			_pContact.placeCall();
		}
	}
	else
		x-=px.width();

	px = spx->getPixmap(QtContactPixmap::ContactIM);		
	if (_pContact.haveIM())
	{
		x-=px.width();
		if ( (pos.x()>=x) && (pos.x()<=x+px.width()) ) {
			qDebug() << "IM clicked";
			_pContact.startIM();
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