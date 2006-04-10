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

#include "QtToaster.h"

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

QtToaster::QtToaster(QWidget * parent , Qt::WFlags f ):
QWidget(parent, Qt::Window | Qt::Popup)
{
	_widget = WidgetFactory::create( ":/forms/toaster/QtToaster.ui", this );
	_widget->setAutoFillBackground(true);
	_closeTimerId = -1 ;
	_closeTimer = 5000;
	_show =   true;

	QGridLayout * layout = new QGridLayout();
	layout->addWidget( _widget );
	layout->setMargin( 0 );
	setLayout( layout );
	setAttribute (Qt::WA_DeleteOnClose,true);
	setupGui();
}

void QtToaster::setupGui(){

	_closeButton =  Object::findChild<QPushButton *>( _widget,"closeButton" );

	_title = Object::findChild<QLabel *>( _widget,"titleLabel" );

	_message = Object::findChild<QLabel *>( _widget,"messageLabel" );

	_button1 = Object::findChild<QPushButton *>( _widget,"actionButton1" );

	_button2 = Object::findChild<QPushButton *>( _widget,"actionButton2" );

	_button3 = Object::findChild<QPushButton *>( _widget,"actionButton3" );

	_pixmapLabel = Object::findChild<QLabel *>( _widget,"pixmapLabel");

	connect ( _closeButton, SIGNAL(clicked()), SLOT(closeToaster()) );
}

void QtToaster::setPixmap(const QPixmap &pixmap){
	_pixmapLabel->setPixmap(pixmap);
}

void QtToaster::setCloseButtonPixmap(const QPixmap & pixmap){
	_closeButton->setIcon(QIcon(pixmap));
}

void QtToaster::setTitle(const QString & title){
	_title->setText(title);
}

void QtToaster::setMessage(const QString & message){
	_message->setText(message);
}

void QtToaster::setButton1Title(const QString & title){
	_button1->setText(title);
}

void QtToaster::setButton1Pixmap(const QPixmap & pixmap){
	_button1->setIcon(QIcon(pixmap));
}

void QtToaster::setButton2Title(const QString & title){
	_button2->setText(title);
}

void QtToaster::setButton2Pixmap(const QPixmap & pixmap){
	_button2->setIcon(QIcon(pixmap));
}

void QtToaster::setButton3Title(const QString & title){
	_button3->setText(title);
}

void QtToaster::setButton3Pixmap(const QPixmap & pixmap){
	_button3->setIcon(QIcon(pixmap));
}

void QtToaster::timerEvent(QTimerEvent *event){
	QDesktopWidget * desktop = QApplication::desktop();
	QRect desktopGeometry = desktop->availableGeometry(desktop->primaryScreen());
	QRect screenGeometry = desktop->screenGeometry(desktop->primaryScreen());

	if ( event->timerId() == _timerId ){
		if ( _show ){
			QPoint p = pos();

			move(p.x(),p.y()-3);

			if ( p.y() < ( desktopGeometry.bottom()  - size().height() - 5 ) ){
				killTimer(_timerId);
				_closeTimerId = startTimer(_closeTimer);
			}
		}
		else{
			QPoint p = pos();

			move(p.x(),p.y()+3);

			if ( p.y() > (screenGeometry.bottom())){
				closeToaster();
			}
		}
	}

	if ( event->timerId() == _closeTimerId ){
		_show = false;
		_timerId = startTimer(20);
	}
}

void QtToaster::showToaster(){
	QDesktopWidget * desktop = QApplication::desktop();


	QRect screenGeometry = desktop->screenGeometry(desktop->primaryScreen());

	_startPosition.setY( screenGeometry.bottom() );
	_startPosition.setX( screenGeometry.right() - size().width() );

	move(_startPosition);

	show();

	_startPosition.setY( screenGeometry.bottom() );
	_startPosition.setX( screenGeometry.right() - size().width() );
	move(_startPosition);

	_timerId = startTimer(20);

}

void QtToaster::hideButton(int num){

	switch (num){
		case 1:
			_button1->hide();
			break;
		case 2:
			_button2->hide();
			break;
		case 3:
			_button3->hide();
		default:
			break;
	}
}

void QtToaster::closeToaster(){
	killTimer(_timerId);
	killTimer(_closeTimerId);
	close();
	closed(this);
}

void QtToaster::setCloseTimer(int timer){
	_closeTimer=timer;
}
