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
#include "QtUserWidget.h"


QtUserWidget::QtUserWidget(QWidget * parent, Qt::WFlags f) : QWidget( parent, f)
{
	_widget = WidgetFactory::create(":/forms/contactlist/userWidget.ui", NULL);
	QGridLayout * layout = new QGridLayout();
	layout->addWidget(_widget);
	layout->setMargin(0);
	setLayout(layout);
	
	_frame = findChild<QFrame *>("frame");
}

void QtUserWidget::setText(const QString & text){
	_text = text;
}

const QString & QtUserWidget::text(){
	return _text;
}

void QtUserWidget::paintEvent(QPaintEvent * event){
	QPalette  p = palette();
    QPainter painter(this);
	painter.fillRect(rect(),QBrush(QColor(255,255,128)));
}

QFrame * QtUserWidget::getFrame(){
	return _frame;
}

QPixmap * QtUserWidget::getTux(){
	return _tux;
}
