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

#include "QtNickNameWidget.h"

#include <presentation/qt/login/QtEditMyProfile.h>

#include <model/profile/UserProfile.h>

#include <QFileDialog>

#include <string>

using namespace std;

QtNickNameWidget::QtNickNameWidget(UserProfile & userProfile, QWidget * parent , Qt::WFlags f )
: QWidget(parent,f), _userProfile(userProfile) {

	QPalette p = palette();
	p.setColor(QPalette::Active,QPalette::Window,Qt::white);
	setPalette(p);
	setAutoFillBackground(true);

	_msnIMAccountMenu = NULL;
	_yahooIMAccountMenu = NULL;
	_jabberIMAccountMenu = NULL;
	_wengoIMAccountMenu = NULL;
	_aimIMAccountMenu = NULL;

	// Layouts
	_widgetLayout = new QGridLayout( this );
	_widgetLayout->setSpacing(0);
	_widgetLayout->setMargin(0);

	_protocolLayout = new QGridLayout();
	_protocolLayout->setSpacing(10);
	_protocolLayout->setMargin(0);

	// Child widgets

	_msnLabel = new QtClickableLabel( this );
	_yahooLabel = new QtClickableLabel( this );
	_wengoLabel = new QtClickableLabel( this );
	_aimLabel = new QtClickableLabel( this );
	_avatarLabel = new QtClickableLabel( this );
	_jabberLabel = new QtClickableLabel ( this );

	_nickNameEdit = new QLineEdit ( this );

	_avatarLabel->setMinimumSize( QSize( 80,80 ) );
	_avatarLabel->setMaximumSize( QSize( 80,80 ) );

	// Add widgets to the layouts

	_widgetLayout->addWidget( _avatarLabel,   0 , 0 );

	_widgetLayout->addWidget( _nickNameEdit,  0 , 1 );
	_widgetLayout->addLayout( _protocolLayout,1 , 1 );

	_protocolLayout->addWidget(_msnLabel     ,0 , 0 );
	_protocolLayout->addWidget(_yahooLabel   ,0 , 1 );
	_protocolLayout->addWidget(_wengoLabel   ,0 , 2 );
	_protocolLayout->addWidget(_aimLabel     ,0 , 3 );
	_protocolLayout->addWidget(_jabberLabel  ,0 , 4 );

	_msnLabel->setText("msn");
	_yahooLabel->setText("yahoo");
	_wengoLabel->setText("wengo");
	_aimLabel->setText("aim");
	_jabberLabel->setText("jabber");

	// Widget connections
	connect ( _msnLabel,SIGNAL( clicked() ),SLOT( msnClicked() ) );
	connect ( _yahooLabel, SIGNAL( clicked() ), SLOT ( yahooClicked() ) );
	connect ( _wengoLabel, SIGNAL( clicked() ), SLOT ( wengoClicked() ) );
	connect ( _aimLabel, SIGNAL ( clicked() ), SLOT ( aimClicked() ) );
	connect ( _avatarLabel, SIGNAL ( clicked() ), SLOT ( avatarClicked() ) );
	connect ( _jabberLabel, SIGNAL ( clicked() ), SLOT ( jabberClicked() ) );
	connect ( _nickNameEdit, SIGNAL(returnPressed ()), SLOT(nicknameChanged()));

	// Widget initialization
	init();
}

void QtNickNameWidget::msnClicked(){
	showMsnMenu();
}

void QtNickNameWidget::yahooClicked(){
	showYahooMenu();
}

void QtNickNameWidget::wengoClicked(){
	showWengoMenu();
}

void QtNickNameWidget::aimClicked(){
	showAimMenu();
}

void QtNickNameWidget::jabberClicked(){
	showJabberMenu();
}

void QtNickNameWidget::avatarClicked(){
	QtEditMyProfile profile(_userProfile, this);
	if (profile.exec()) {
		init();
	}
}

void QtNickNameWidget::nicknameChanged() {
	_userProfile.setAlias(_nickNameEdit->text().toStdString());
}

// Menus

void QtNickNameWidget::showMsnMenu(){
	if ( _msnIMAccountMenu )
		delete _msnIMAccountMenu;

	_msnIMAccountMenu = new QMenu(this);

	QMenu * msnMenu = _msnIMAccountMenu->addMenu( tr( "Example account" ) );

	QAction * action;

	action = msnMenu->addAction( tr( "Online" ) );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( msnOnlineClicked(bool) ) );

	action = msnMenu->addAction( tr( "DND" ) );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( msnDNDClicked(bool) ) );

	action = msnMenu->addAction( tr( "Be right back" ) );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( msnBRBClicked(bool) ) );

	action = msnMenu->addAction( tr( "Away" ) );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( msnAwayClicked(bool) ) );

	action = msnMenu->addAction( tr( "Phone" ) );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( msnOnPhoneClicked(bool) ) );

	action = msnMenu->addAction( tr( "Lunch" ) );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( msnLunchClicked(bool) ) );

	action = msnMenu->addAction( tr( "Not connected" ) );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( msnOffLineClicked(bool) ) );

	msnMenu->addSeparator ();

	action = msnMenu->addAction( tr( "Disconnect" ) );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( msnDisconnectClicked(bool) ) );

	action = msnMenu->addAction( tr( "Reconnect" ) );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( msnReconnectClicked(bool) ) );

	QPoint p = _msnLabel->pos();
	p.setY( p.y() + _msnLabel->rect().bottom() );

	_msnIMAccountMenu->setWindowOpacity(0.95);
	msnMenu->setWindowOpacity(0.95);

	_msnIMAccountMenu->popup(mapToGlobal(p));
}

void QtNickNameWidget::showYahooMenu(){
	if ( _yahooIMAccountMenu )
		delete _yahooIMAccountMenu;

	_yahooIMAccountMenu = new QMenu(this);

	QMenu * yahooMenu = _yahooIMAccountMenu->addMenu( tr( "Example menu" ) );

	QAction * action;

	action = yahooMenu->addAction( tr ("Disponible") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( yahooAvailableClicked(bool) ) );

	action = yahooMenu->addAction( tr ("OccupÈ") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( yahooBusyClicked(bool) ) );

	action = yahooMenu->addAction( tr ("Pas l‡") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( yahooAwayClicked(bool) ) );

	action = yahooMenu->addAction( tr ("De retour dans 5 minutes") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( yahooBRBClicked(bool) ) );

	action = yahooMenu->addAction( tr ("Pas devant l'Ècran") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( yahooNotHereClicked(bool) ) );

	action = yahooMenu->addAction( tr ("Au tÈlÈphone") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( yahooOnPhoneClicked(bool) ) );

	action = yahooMenu->addAction( tr ("Insisible pour tous") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( yahooHiddenClicked(bool) ) );

	yahooMenu->addSeparator ();

	action = yahooMenu->addAction( tr ("DÈconnecter") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( yahooDisconnectClicked(bool) ) );

	action = yahooMenu->addAction( tr ("Re-connecter") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( yahooReconnectClicked(bool) ) );

	QPoint p = _yahooLabel->pos();
	p.setY( p.y() + _yahooLabel->rect().bottom() );

	_yahooIMAccountMenu->setWindowOpacity(0.95);
	yahooMenu->setWindowOpacity(0.95);

	_yahooIMAccountMenu->popup(mapToGlobal(p));

}

void QtNickNameWidget::showWengoMenu(){

	if ( _wengoIMAccountMenu )
		delete _wengoIMAccountMenu;

	_wengoIMAccountMenu = new QMenu(this);

	QMenu * wengoMenu = _wengoIMAccountMenu->addMenu( "Example account" );

	QAction * action;

	action = wengoMenu->addAction( tr( "Online" ) );
	connect ( action, SIGNAL( triggered(bool) ),SLOT( wengoOnlineClicked(bool) ) );

	action = wengoMenu->addAction( tr( "DND" ) );
	connect ( action, SIGNAL( triggered(bool) ),SLOT( wengoDNDClicked(bool) ) );

	action = wengoMenu->addAction( tr( "Invisible" ) );
	connect ( action, SIGNAL( triggered(bool) ),SLOT( wengoInvisibleClicked(bool) ) );

	action = wengoMenu->addAction( tr( "Be right back" ) );
	connect ( action, SIGNAL( triggered(bool) ),SLOT( wengoBRBClicked(bool) ) );

	action = wengoMenu->addAction( tr( "Away" ) );
	connect ( action, SIGNAL( triggered(bool) ),SLOT( wengoAwayClicked(bool) ) );

	action = wengoMenu->addAction( tr( "Not available" ) );
	connect ( action, SIGNAL( triggered(bool) ),SLOT( wengoNotAvailableClicked(bool) ) );

	action = wengoMenu->addAction( tr( "Forward to cellphone" ) );
	connect ( action, SIGNAL( triggered(bool) ),SLOT( wengoForwardClicked(bool) ) );

	QPoint p = _wengoLabel->pos();

	p.setY( p.y() + _wengoLabel->rect().bottom() );

	_wengoIMAccountMenu->setWindowOpacity(0.95);
	wengoMenu->setWindowOpacity(0.95);

	_wengoIMAccountMenu->popup(mapToGlobal(p));

}

void QtNickNameWidget::showAimMenu(){
	if ( _aimIMAccountMenu )
		delete _aimIMAccountMenu;

	_aimIMAccountMenu = new QMenu(this);

	QMenu * aimMenu = _aimIMAccountMenu->addMenu( "Example account" );

	QAction * action;

	action = aimMenu->addAction( tr ("Available") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( aimAvailableClicked(bool) ) );

	action = aimMenu->addAction( tr ("Away") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( aimAwayClicked(bool) ) );

	action = aimMenu->addAction( tr ("N/A") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( aimNAClicked(bool) ) );

	action = aimMenu->addAction( tr ("Privacy") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( aimPrivacyClicked(bool) ) );

	action = aimMenu->addAction( tr ("Offline") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( aimOffLineClicked(bool) ) );

	aimMenu->addSeparator ();

	action = aimMenu->addAction( tr ("Disconnect") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( aimDisconnectClicked(bool) ) );

	action = aimMenu->addAction( tr ("Re-connect") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( aimReconnectClicked(bool) ) );

	QPoint p = _aimLabel->pos();
	p.setY( p.y() + _aimLabel->rect().bottom() );

	_aimIMAccountMenu->setWindowOpacity(0.95);
	aimMenu->setWindowOpacity(0.95);

	_aimIMAccountMenu->popup(mapToGlobal(p));

}

void QtNickNameWidget::showJabberMenu(){
	if ( _jabberIMAccountMenu)
		delete _jabberIMAccountMenu;

	_jabberIMAccountMenu = new QMenu(this);

	QMenu * jabberMenu = _jabberIMAccountMenu->addMenu( tr( "Example account" ) );


	QAction * action;

	action = jabberMenu->addAction( tr ("Available") );
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( jabberAvailableClicked(bool) ) );

	action = jabberMenu->addAction( tr ("Away") ) ;
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( jabberAwayClicked(bool) ) );

	action = jabberMenu->addAction( tr ("Chat") ) ;
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( jabberChatClicked(bool) ) );

	action = jabberMenu->addAction( tr ("DND") ) ;
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( jabberDNDClicked(bool) ) );

	action = jabberMenu->addAction( tr ("N/A") ) ;
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( jabberNAClicked(bool) ) );

	jabberMenu->addSeparator ();

	action = jabberMenu->addAction( tr ("Disconnect") ) ;
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( jabberDisconnectClicked(bool) ) );

	action = jabberMenu->addAction( tr ("Reconnect") ) ;
	connect ( action, SIGNAL( triggered(bool) ), SLOT ( jabberReconnectClicked(bool) ) );


	QPoint p = _aimLabel->pos();
	p.setY( p.y() + _aimLabel->rect().bottom() );

	_jabberIMAccountMenu->setWindowOpacity(0.95);
	jabberMenu->setWindowOpacity(0.95);

	_jabberIMAccountMenu->popup(mapToGlobal(p));

}
// Menus actions
// Wengo
void QtNickNameWidget::wengoOnlineClicked(bool){
}
void QtNickNameWidget::wengoDNDClicked(bool){
}
void QtNickNameWidget::wengoInvisibleClicked(bool){
}
void QtNickNameWidget::wengoBRBClicked(bool){
}
void QtNickNameWidget::wengoAwayClicked(bool){
}
void QtNickNameWidget::wengoNotAvailableClicked(bool){
}
void QtNickNameWidget::wengoForwardClicked(bool){
}
// Yahoo
void QtNickNameWidget::yahooAvailableClicked(bool ){
}
void QtNickNameWidget::yahooBusyClicked(bool ){
}
void QtNickNameWidget::yahooAwayClicked(bool ){
}
void QtNickNameWidget::yahooNotHereClicked(bool ){
}
void QtNickNameWidget::yahooOnPhoneClicked(bool ){
}
void QtNickNameWidget::yahooHiddenClicked(bool ){
}
void QtNickNameWidget::yahooDisconnectClicked(bool ){
}
void QtNickNameWidget::yahooReconnectClicked(bool ){
}
void QtNickNameWidget::yahooBRBClicked(bool ){
}
// MSN
void QtNickNameWidget::msnOnlineClicked(bool ){
}
void QtNickNameWidget::msnDNDClicked(bool ){
}
void QtNickNameWidget::msnBRBClicked(bool ){
}
void QtNickNameWidget::msnAwayClicked(bool ){
}
void QtNickNameWidget::msnOnPhoneClicked(bool ){
}
void QtNickNameWidget::msnLunchClicked(bool ){
}
void QtNickNameWidget::msnOffLineClicked(bool ){
}
void QtNickNameWidget::msnDisconnectClicked(bool ){
}
void QtNickNameWidget::msnReconnectClicked(bool ){
}

// ICQ / AIM
void QtNickNameWidget::aimAvailableClicked(bool){
}
void QtNickNameWidget::aimAwayClicked(bool){
}
void QtNickNameWidget::aimNAClicked(bool){
}
void QtNickNameWidget::aimPrivacyClicked(bool){
}
void QtNickNameWidget::aimOffLineClicked(bool){
}
void QtNickNameWidget::aimDisconnectClicked(bool){
}
void QtNickNameWidget::aimReconnectClicked(bool){
}

// GTalk / Jabber

void QtNickNameWidget::jabberAvailableClicked(bool ){
}
void QtNickNameWidget::jabberAwayClicked(bool ){
}
void QtNickNameWidget::jabberChatClicked(bool ){
}
void QtNickNameWidget::jabberDNDClicked(bool ){
}
void QtNickNameWidget::jabberNAClicked(bool ){
}
void QtNickNameWidget::jabberDisconnectClicked(bool ){
}
void QtNickNameWidget::jabberReconnectClicked(bool ){
}

void QtNickNameWidget::init() {
	_nickNameEdit->setText(QString::fromStdString(_userProfile.getAlias()));

	// Setting avatar
	QPixmap pixmap;
	string myData = _userProfile.getIcon().getData();
	pixmap.loadFromData((uchar *)myData.c_str(), myData.size());
	_avatarLabel->setPixmap(pixmap.scaled(_avatarLabel->rect().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	////
}
