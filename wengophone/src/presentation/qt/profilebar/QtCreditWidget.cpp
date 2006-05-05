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

#include "QtCreditWidget.h"

#include <control/CWengoPhone.h>

QtCreditWidget::QtCreditWidget(QWidget * parent , Qt::WFlags f ) : QWidget(parent,f){

	//buy call-out label
	QtClickableLabel * buyLabel = new QtClickableLabel(this);
	buyLabel->setText(tr("Buy call-out credits"));

	QPalette palette = buyLabel->palette();
	palette.setColor(QPalette::WindowText, Qt::blue);
	buyLabel->setPalette(palette);

	QFont font = buyLabel->font();
	font.setUnderline(true);
	buyLabel->setFont(font);

	QCursor cursor = buyLabel->cursor();
	cursor.setShape(Qt::PointingHandCursor);
	buyLabel->setCursor(cursor);

	connect(buyLabel, SIGNAL(clicked()), SLOT(buyOutClicked()));
	
	//Pstn number label
	_pstnNumber = new QLabel(this);

	//add labels to the layout
	_gridLayout = new QGridLayout(this);
	_gridLayout->addWidget(buyLabel, 0, 0);
}

void QtCreditWidget::buyOutClicked(){
	_cWengoPhone->showWengoBuyWengos();
}

void QtCreditWidget::setCWengoPhone(CWengoPhone * cwengophone){
	_cWengoPhone = cwengophone;
}

void QtCreditWidget::setPstnNumber(const QString & number) {
	//TODO: remove the '0' hack when the ws will be updated
	_pstnNumber->setText(tr("Your number: 0") + number);
	_gridLayout->addWidget(_pstnNumber, 1, 0);
}
