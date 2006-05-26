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

#include <qtutil/QtWengoStyleLabel.h>

QtWengoStyleLabel::QtWengoStyleLabel( QWidget * parent , Qt::WFlags f  ) :
QLabel(parent,f){

	_parent = parent;
	_pressed = false;
	_selected = false;

	// Default background color
	_backgroundColor = _parent->palette().color(QPalette::Window );
	// Default text color
	_textColor = _parent->palette().color(QPalette::Text);

	_alignment = Qt::AlignCenter;
}

QtWengoStyleLabel::~QtWengoStyleLabel ( ){
}


void QtWengoStyleLabel::paintEvent(QPaintEvent * event){
/*
	qDebug() << "Paint event";
	QLabel::paintEvent( event );
	return;
*/
	QRect rect = this->rect();

	QPainter painter( this );
	rect.adjust(-1,-1,1,1);


	// painter.fillRect(rect,QBrush(_backgroundColor));

	if ( ! _pressed && ! _selected ){
		// Draw the left side if any
		if ( !_normalLeftPixmap.isNull() ){
			painter.drawPixmap(0,0,_normalLeftPixmap);
		}
		// Fill the the label
		if ( !_normalFillPixmap.isNull() ){
			QBrush brush(_normalFillPixmap);

			QRect fillRect = rect;
			if ( ! _normalLeftPixmap.isNull() )
				fillRect.adjust(_normalLeftPixmap.rect().width()-1,0,0,0);
			if ( ! _normalRightPixmap.isNull() )
				fillRect.adjust(0,0,0-_normalRightPixmap.rect().width(),0);
			painter.fillRect(fillRect,brush);
		}
		// Draw the right side
		if ( ! _normalRightPixmap.isNull() ){
			painter.drawPixmap( (rect.width()-1) - _normalRightPixmap.rect().width(),0,_normalRightPixmap);
		}

	} // if ( ! _pressed )
	else
	{
		// Draw the left side if any
		if ( !_pressedLeftPixmap.isNull() ){
			painter.drawPixmap(0,0,_pressedLeftPixmap);
		}
		// Fill the the label
		if ( !_pressedFillPixmap.isNull() ){
			QBrush brush(_pressedFillPixmap);

			QRect fillRect = rect;
			if ( ! _pressedLeftPixmap.isNull() )
				fillRect.adjust(_pressedLeftPixmap.rect().width()-1,0,0,0);
			if ( ! _pressedRightPixmap.isNull() )
				fillRect.adjust(0,0,0-_pressedRightPixmap.rect().width(),0);
			painter.fillRect(fillRect,brush);
		}
		// Draw the right side
		if ( ! _pressedRightPixmap.isNull() ){
			painter.drawPixmap( (rect.width()-1) - _pressedRightPixmap.rect().width(),0,_pressedRightPixmap);
		}
	}
	painter.end();
	QPainter p ( this );
	drawText(&p);
	p.end();
}

void QtWengoStyleLabel::drawText(QPainter * painter){
	QRect rect = this->rect();

	painter->save();
	painter->setPen(_textColor);
	painter->drawText ( rect, _alignment , this->text(), &rect );
	painter->restore();

}

void QtWengoStyleLabel::resizeEvent ( QResizeEvent * event ){
	QLabel::resizeEvent (event);
}

void QtWengoStyleLabel::setPixmaps(const QPixmap & normaleLeftPixmap,
								const QPixmap & normaleRightPixmap,
				                const QPixmap & normaleFillPixmap,
				                const QPixmap & pressedLeftPixmap,
				                const QPixmap & pressedRightPixmap,
				                const QPixmap & pressedFillPixmap){

	_normalLeftPixmap = normaleLeftPixmap;
	_normalRightPixmap = normaleRightPixmap;
	_normalFillPixmap = normaleFillPixmap;

	_pressedLeftPixmap = pressedLeftPixmap;
	_pressedRightPixmap = pressedRightPixmap;
	_pressedFillPixmap = pressedFillPixmap;

}

void QtWengoStyleLabel::setTextColor(const QColor & color){
	_textColor = color;
}

void QtWengoStyleLabel::setBackgroundColor(const QColor & color){
	_backgroundColor = color;
}

void QtWengoStyleLabel::mouseMoveEvent ( QMouseEvent * event ){

	_pressed = false;
	QPoint mousePosition = event->pos();
	const QRect rect = this->rect();
	if ( ( mousePosition.x() >= rect.x() ) && ( mousePosition.x() <= rect.width() ) ){
		if ( ( mousePosition.y() >= rect.y() ) && ( mousePosition.y() <= rect.height() ) ){
			_pressed = true;
		}
	}
	update();
}

void QtWengoStyleLabel::mousePressEvent ( QMouseEvent * event ){
	if ( event->button() == Qt::LeftButton ){
		_pressed = true;
		update();
	}
}

void QtWengoStyleLabel::mouseReleaseEvent ( QMouseEvent * event ){

	if ( ! _pressed )
		return;
	/* get the widget rect */
	const QRect rect = this->rect();
	/* get the mouse position relative to this widget */
	QPoint mousePosition = event->pos();
	if ( ( mousePosition.x() >= rect.x() ) && ( mousePosition.x() <= rect.width() ) ){
		if ( ( mousePosition.y() >= rect.y() ) && ( mousePosition.y() <= rect.height() ) ){
			clicked();
			clicked(text());
			clicked(this);
			_pressed = false;
			update();
		}
	}
}

void QtWengoStyleLabel::setText(const QString & text){

    QLabel::setText(text);
    QFontMetrics fm(font());
    int textWidth = fm.width(text);
    textWidth+=40;
    QSize s = size();
    if ( textWidth > s.width()){
//        setMaximumSize(textWidth,s.height());
        setMinimumSize(textWidth,s.height());
    }

}


