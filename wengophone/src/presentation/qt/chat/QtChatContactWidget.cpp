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

#include "QtChatContactWidget.h"

QtChatContactWidget::QtChatContactWidget(IMChatSession * imChatSession,
										 CChatHandler & cChatHandler,
										 QWidget * parent , Qt::WFlags f ):
QLabel (parent,f),_cChatHandler(cChatHandler){
	_imChatSession = imChatSession;
	updateDisplay();
}

QtChatContactWidget::getId(){
	return _imChatSession->getId();
}

void QtChatContactWidget::updateDisplay(){

	QMutexLocker locker(&_mutex);

	QList <QPixmap> pixmapList;
	int totalSize = 0;
	IMContactSet contactSet =_imChatSession->getIMContactSet();

	IMContactSet::iterator it;

	// Fill the pixmap list
	for (it = contactSet.begin(); it != contactSet.end(); it++){
		Picture picture = _cChatHandler.getUserProfile().getPresenceHandler().getContactIcon( (*it) );
		QString contactId = QString::fromStdString( (*it).getContactId() );
		std::string data = picture.getData();
		QPixmap pixmap;
		if ( ! data.empty() ){
			pixmap.loadFromData((uchar *)data.c_str(), data.size());
		}else{
			pixmap.load(":pics/contact_picture.png");
		}
		// Scale the pixmap to 48x48

		pixmap = createContactPixmap(contactId,pixmap);

		pixmapList.append(pixmap);
		totalSize += 48;
		totalSize += 5;
	}

	// Construct the display pixma
	createGroupPixmap(totalSize);

	fillDisplay(pixmapList);

	setPixmap(_display);

	update();
}
QPixmap QtChatContactWidget::createContactPixmap(const QString & displayName,QPixmap  pixmap ){

	QPixmap ret = QPixmap(QSize(48,64));

	QPainter painter ( &ret );

	painter.fillRect(ret.rect(),Qt::black);

	painter.drawPixmap(0,0,pixmap.scaled(48, 48, Qt::IgnoreAspectRatio));

	painter.end();

	setDisplayName(displayName,&ret);

	return ret;
}


void QtChatContactWidget::drawBorder(QPainter * painter, QRect rect, const QColor & color){

	rect.adjust(0,0,-1,-1);
	QPen pen;

	pen.setColor( color );

	pen.setJoinStyle(Qt::RoundJoin);
	pen.setCapStyle(Qt::RoundCap);

	painter->setPen(pen);
	painter->drawRect(rect);

}

void QtChatContactWidget::createGroupPixmap(int width){

	QPixmap pixmap = QPixmap( QSize(width+10, 70) );

	QPainter painter( &pixmap );

	painter.fillRect(pixmap.rect(),QBrush(QColor(Qt::black)));

	painter.end();

	_display = pixmap;
}

void QtChatContactWidget::fillDisplay(QList <QPixmap> pixList){

	QList <QPixmap>::iterator pixIt;
	int size = 0 ;
	QPainter painter( &_display );

	for ( pixIt = pixList.begin(); pixIt != pixList.end(); pixIt++){
		size +=48 + 5;
	}


	int x =  ( (_display.width() - size ) / 2 ) ;

	for ( pixIt = pixList.begin(); pixIt != pixList.end(); pixIt++){
		painter.drawPixmap(x, 5, (*pixIt) );
		x+=48 + 5;
	}

	// Draw the border
	drawBorder(&painter, _display.rect(), Qt::red);
	painter.end();

}

void QtChatContactWidget::setDisplayName(QString displayName, QPixmap * pixmap){

	QPainter painter( pixmap );
	QFont font;

	font.setPointSize(8);

	painter.setFont(font);

	painter.setPen(QPen(Qt::white));

	QRect rect = pixmap->rect();

	rect.adjust( 0,rect.bottom() - 10 ,0,0 );

	painter.drawText ( rect , Qt::AlignLeft, displayName, &rect);

	painter.end();

}
