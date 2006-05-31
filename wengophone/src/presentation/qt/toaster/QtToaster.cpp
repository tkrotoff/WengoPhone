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
#include <qtutil/QtWengoStyleLabel.h>
QtToaster::QtToaster(QWidget * parent , Qt::WFlags f ):
QWidget(parent, Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint )
{
	setupGui();
}

void QtToaster::setupGui(){
	_widget = WidgetFactory::create( ":/forms/toaster/QtToaster.ui", this );

	_closeTimerId = -1 ;
	_closeTimer = 5000;
	_show =   true;

	QGridLayout * layout = new QGridLayout();
	layout->addWidget( _widget );
	layout->setMargin( 0 );
	setLayout( layout );
	setAttribute (Qt::WA_DeleteOnClose,true);

	_closeButton =  Object::findChild<QPushButton *>( _widget,"closeButton" );

	_title = Object::findChild<QLabel *>( _widget,"titleLabel" );

	_message = Object::findChild<QLabel *>( _widget,"messageLabel" );

	_button1 = Object::findChild<QPushButton *>( _widget,"actionButton1" );


	_pixmapLabel = Object::findChild<QLabel *>( _widget,"pixmapLabel");

	connect ( _closeButton, SIGNAL(clicked()), SLOT(closeToaster()) );

	connect ( _button1,SIGNAL(clicked()),SLOT(button1Slot()));

	QFrame * frame = Object::findChild<QFrame *>( _widget,"frame_2" );

    QRect r = frame->rect();

    QLinearGradient lg( QPointF( 1, r.top() ), QPointF( 1, r.bottom() ) );
    lg.setColorAt ( 0, qApp->palette().color(QPalette::Window) );

    QColor dest = qApp->palette().color(QPalette::Window);
    float red = ((float )dest.red()) / 1.3f;
    float blue = ((float )dest.blue()) / 1.3f;
    float green = ((float )dest.green()) / 1.3f;
    dest = QColor((int)red,(int)green,(int)blue);
    lg.setColorAt ( 1, dest  );

	QPalette palette = frame->palette();
	palette.setBrush(frame->backgroundRole(), QBrush(lg));
	frame->setPalette(palette);

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
    _button1->setMinimumSize(pixmap.size());
    _button1->setMaximumSize(pixmap.size());
    _button1->setIconSize(pixmap.size());
    _button1->setFlat(true);
	_button1->setIcon(QIcon(pixmap));
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

			break;
		case 3:

		default:
			break;
	}
}

void QtToaster::closeToaster(){
	killTimer(_timerId);
	killTimer(_closeTimerId);
	close();
}

void QtToaster::setCloseTimer(int timer){
	_closeTimer=timer;
}

void QtToaster::button1Slot(){
    button1Clicked();
    close();
}

