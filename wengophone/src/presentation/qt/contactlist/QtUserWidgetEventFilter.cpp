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

#include "QtUserWidgetEventFilter.h"
#include "QtContactPixmap.h"

QtUserWidgetEventFilter::QtUserWidgetEventFilter(QObject * parent, QWidget * target, QtUser * user) : QObject(parent) {
	_target = target;
	_user = user;
}

bool QtUserWidgetEventFilter::eventFilter(QObject * obj, QEvent * event) {
	switch (event->type()) {
		case QEvent::Paint:
			paintEvent(dynamic_cast < QPaintEvent * > (event));
			return true;
		default:
			return QObject::eventFilter(obj, event);
	}
}

void QtUserWidgetEventFilter::paintEvent(QPaintEvent * event) {
	QPalette p = _target->palette();

/*
	QLinearGradient lg( QPointF(1,1), QPointF(_target->rect().width(),1));
	lg.setSpread(QGradient::RepeatSpread);
	lg.setColorAt ( 0, p.midlight().color());
	lg.setColorAt ( .8, QColor(210, 216, 234));
*/

	QColor lg(201, 201, 201);
	QRect rect = _target->rect(); // = event->rect();
	QPainter painter(_target);

/*
    painter.setPen(QColor(233,233,233));
    painter.drawLine(rect.left(),rect.top(),rect.right(),rect.top());
    rect.adjust(0,1,0,0);
    painter.setPen(QColor(215,215,215));
    painter.drawLine(rect.left(),rect.top(),rect.right(),rect.top());
    rect.adjust(0,1,0,0);
    painter.setPen(QColor(255,255,255));
    painter.drawLine(rect.left(),rect.top(),rect.right(),rect.top());
    rect.adjust(0,1,0,0);
    painter.setPen(QColor(255,255,255));
    painter.drawLine(rect.left(),rect.top(),rect.right(),rect.top());
*/

	painter.fillRect(rect, QBrush(lg));

	painter.setPen(QColor(233, 233, 233));
	painter.drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());
	rect.adjust(0, 0, 0, -1);
	painter.setPen(QColor(215, 215, 215));
	painter.drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());
	rect.adjust(0, 0, 0, -1);
	painter.setPen(QColor(255, 255, 255));
	painter.drawLine(rect.left(), rect.top(), rect.right(), rect.top());
	rect.adjust(0, 0, 0, -1);
	painter.setPen(QColor(255, 255, 255));
	painter.drawLine(rect.left(), rect.top(), rect.right(), rect.top());
	paintUser(& painter, _target->rect());

	painter.end();
}

void QtUserWidgetEventFilter::paintUser(QPainter * painter, QRect rect) {
	QRect r;
	QPixmap px;
	QtContactPixmap * spx;
	int x;
	QPalette p = _target->palette();

	spx = QtContactPixmap::getInstance();
	painter->setPen(p.text().color());
	// Draw the status pixmap
	px = spx->getPixmap(_user->getStatus());
	r = rect;
	x = r.left();

	int centeredPx_y = (QtUser::UserSize - px.size().height()) / 2;

	painter->drawPixmap(x, r.top() + centeredPx_y, px);
	x += px.width() + 5;
	r.setLeft(x);
	// Draw the user
	//painter->setFont(option.font);
	QRect textRect = r;
	int centeredText_y = (QtUser::UserSize - QFontMetrics(_target->font()).height()) / 2;
	textRect.setTop(textRect.top() + centeredText_y);


	QFont font = painter->font();
	font.setBold(true);
	painter->setFont(font);
	painter->drawText(textRect, Qt::AlignLeft, _user->getUserName(), 0);
	font.setBold(false);
	painter->setFont(font);

	int _iconsStartPosition;

/*
	x=rect.width()-1;
	if ( _user->getStatus() != QtContactPixmap::ContactOffline)
	{
		px = spx->getPixmap(QtContactPixmap::ContactVideo);
		if (_user->hasVideo())
		{
			x-=px.width();
			painter->drawPixmap (x,r.top(),px);
		}
		else
			x-=px.width();
		px = spx->getPixmap(QtContactPixmap::ContactCall);
		if (_user->hasCall())
		{
			x-=px.width();
			painter->drawPixmap (x,r.top(),px);
		}
		else
			x-=px.width();

		px = spx->getPixmap(QtContactPixmap::ContactIM);
		if (_user->hasIM())
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

*/

	_iconsStartPosition = 10000;
}
