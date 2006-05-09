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

#include <qtutil/QtClickableLabel.h>

QtClickableLabel::QtClickableLabel( QWidget * parent , Qt::WFlags f ) : QLabel( parent, f ) {
	_mousePressed = false;
//	setAutoFillBackground(true);
	_bgcolor = Qt::white;
}

void QtClickableLabel::mousePressEvent ( QMouseEvent * ) {
	_mousePressed = true;
}

void QtClickableLabel::mouseReleaseEvent ( QMouseEvent * e ) {
	/* get the widget rect */
	const QRect rect = this->rect();
	/* get the mouse position relative to this widget */
	QPoint mousePosition = e->pos();
	
	if ( ( mousePosition.x() >= rect.x() ) && ( mousePosition.x() <= rect.width() ) ) {
		if ( ( mousePosition.y() >= rect.y() ) && ( mousePosition.y() <= rect.height() ) ) {
			if( e->button() == Qt::LeftButton ) {

				clicked();
				clicked(text());
				clicked(this);

			} else if( e->button() == Qt::RightButton ) {

				rightClicked();
				rightClicked(text());
				rightClicked(this);
			}
		}
	}
}

void QtClickableLabel::setSelectedBgColor(const QColor & bgcolor) {
	_bgcolor = bgcolor;
}

void QtClickableLabel::setSelected(bool value) {

	if ( value ){
		QPalette p = palette();
		p.setColor(QPalette::Active,QPalette::Window,Qt::white);
		setPalette(p);
		update();
	}
	else
	{
		QPalette p = palette();
		p.setColor(QPalette::Active,QPalette::Window,qApp->palette().window().color());
		setPalette(p);
		update();
	}

}
