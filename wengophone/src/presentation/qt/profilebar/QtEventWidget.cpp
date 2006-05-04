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

#include "QtEventWidget.h"

QtEventWidget::QtEventWidget( QWidget * parent, Qt::WFlags f )
	: QWidget ( parent, f ) {
/*
	QPalette p = palette();
	p.setColor(QPalette::Active,QPalette::Window,Qt::white);
	setPalette(p);
	setAutoFillBackground(true);
*/
	QGridLayout * gridLayout = new QGridLayout(this);

	_missedCallLabel = new QtClickableLabel(this);
	_missedCallLabel->setText( tr ("Missed calls") );

	_newMessagesLabel = new QtClickableLabel(this);
	_newMessagesLabel->setText( tr("New messages") );

	gridLayout->addWidget( _missedCallLabel ,0,0 );
	gridLayout->addWidget( _newMessagesLabel,0,1 );

}

void QtEventWidget::addMissedCall(const QString & nickName){

	QtClickableLabel * label = new QtClickableLabel(this);
	label->setText(nickName);
	QGridLayout * gridLayout = dynamic_cast<QGridLayout *>(layout());
	int row = gridLayout->rowCount ();
	gridLayout->addWidget( label, row, 0);
}

void QtEventWidget::addMessage(const QString & nickName){

	QtClickableLabel * label = new QtClickableLabel(this);
	label->setText(nickName);
	QGridLayout * gridLayout = dynamic_cast<QGridLayout *>(layout());
	int row = gridLayout->rowCount ();
	gridLayout->addWidget( label, row, 1);
}

void QtEventWidget::setNewMessages(int count) {
	if( count ) {
		_newMessagesLabel->setText( QString("%1").arg(count) + tr(" new Voice Mail") );
	} else {
		_newMessagesLabel->setText( tr("No new Voice Mail") );
	}
}
