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

#include "QtProfileBar.h"

#include <control/CWengoPhone.h>
#include <model/profile/UserProfile.h>

QtProfileBar::QtProfileBar(CWengoPhone & cWengoPhone, UserProfile & userProfile, QWidget * parent , Qt::WFlags f )
: QWidget (parent,f), _userProfile(userProfile), _cWengoPhone(cWengoPhone) {

	_statusMenu = NULL;

	_nickNameWidgetVisible = false;
	_eventsWidgetVisible = false;
	_crediWidgetVisible = false;

	_widgetLayout = new QGridLayout(this);
	_widgetLayout->setMargin(0);
	_widgetLayout->setSpacing(0);

	_gridlayout = new QGridLayout();
	_gridlayout->setMargin(0);
	_gridlayout->setSpacing(0);

	_widgetLayout->addLayout(_gridlayout,0,0);

	// The status widget
	_statusLabel = new QtProfileLabel(this);
	_statusLabel->setMinimumSize(QSize(46,65));
	_statusLabel->setMaximumSize(QSize(46,65));
	_statusLabel->setPixmaps(QPixmap(":/pics/profilebar/bar_start_status_green.png"),
	                QPixmap(), // no end
	                QPixmap(), // no fill
	                QPixmap(":/pics/profilebar/bar_start_status_green.png"),
	                QPixmap(),  // no end
	                QPixmap()
	                ); // no fill

	// Nickname label
	_nicknameLabel = new QtProfileLabel(this);
	_nicknameLabel->setMinimumSize(QSize(46,65));
	_nicknameLabel->setMaximumSize(QSize(1000,65));
	_nicknameLabel->setPixmaps(
					QPixmap(), // no start
	                QPixmap(":/pics/profilebar/bar_separator.png"), //  end
	                QPixmap(":/pics/profilebar/bar_fill.png"),
	                QPixmap(),  // no start
	                QPixmap(":/pics/profilebar/bar_separator.png"),  // end
	                QPixmap(":/pics/profilebar/bar_fill.png")
	                );


	// The events label
	_eventsLabel = new QtProfileLabel(this);
	_eventsLabel->setMinimumSize(QSize(46,65));
	_eventsLabel->setMaximumSize(QSize(1000,65));
	_eventsLabel->setPixmaps(QPixmap(), // no start
	                QPixmap(":/pics/profilebar/bar_separator.png"), //  end
	                QPixmap(":/pics/profilebar/bar_fill.png"),
	                QPixmap(),  // no start
	                QPixmap(":/pics/profilebar/bar_separator.png"),  // end
	                QPixmap(":/pics/profilebar/bar_fill.png")
	                );

	// The credit label
	_creditLabel = new QtProfileLabel(this);
	_creditLabel->setMinimumSize(QSize(46,65));
	_creditLabel->setMaximumSize(QSize(1000,65));
	_creditLabel->setPixmaps(QPixmap(), // no start
	                QPixmap(":/pics/profilebar/bar_end.png"),
	                QPixmap(":/pics/profilebar/bar_fill.png"),
	                QPixmap(),  // no start
	                QPixmap(":/pics/profilebar/bar_end.png"),
	                QPixmap(":/pics/profilebar/bar_fill.png")
	                );

	// Add the labels to the gridlayout
	_gridlayout->addWidget( _statusLabel   , 0, 0 );
	_gridlayout->addWidget( _nicknameLabel , 0, 1 );
	_gridlayout->addWidget( _eventsLabel   , 0, 2 );
	_gridlayout->addWidget( _creditLabel   , 0, 3 );

	//_statusLabel->setText("S");
	_nicknameLabel->setText("NickName");
	_nicknameLabel->setTextColor(Qt::white);

	_eventsLabel->setText("events");
	_eventsLabel->setTextColor(Qt::white);


	_creditLabel->setText(	QString() + QChar(0x20ac)+ QString(" 0.00"));
	_creditLabel->setTextColor(Qt::white);

	// Objects connection
	connect(_statusLabel,SIGNAL(clicked()),SLOT(statusClicked()));
	connect(_nicknameLabel,SIGNAL(clicked()),SLOT(nicknameClicked()));
	connect(_eventsLabel,SIGNAL(clicked()),SLOT(eventsClicked()));
	connect(_creditLabel,SIGNAL(clicked()),SLOT(creditClicked()));

	// resize
	setMinimumSize(QSize(120,65));


	// create internal widgets
	_nickNameWidget = new QtNickNameWidget(_userProfile, this);
	_nickNameWidget->setVisible(false);

	_eventWidget = new QtEventWidget(this);
	_eventWidget->setVisible(false);

	_creditWidget = new QtCreditWidget(this);
	_creditWidget->setCWengoPhone(&_cWengoPhone);
	_creditWidget->setVisible(false);


	_userProfile.wsInfoCreatedEvent += boost::bind(&QtProfileBar::wsInfoCreatedEventHandler, this, _1, _2);
}


void QtProfileBar::statusClicked(){
	createStatusMenu();
}

void QtProfileBar::nicknameClicked(){
	if ( _nickNameWidgetVisible ){
		removeNickNameWidget();
	}
	else{
		_nicknameLabel->setSelected(true);
		_eventsLabel->setSelected(false);
		_creditLabel->setSelected(false);
		showNickNameWidget();
	}
}

void QtProfileBar::showNickNameWidget(){

	removeEventsWidget();
	removeCreditWidget();

	if ( ! _nickNameWidgetVisible )
	{
//		_nickNameWidget = new QtNickNameWidget(_userProfile, this);
		_widgetLayout->addWidget(_nickNameWidget, 1, 0 );
		_nickNameWidget->setVisible(true);
		_nickNameWidgetVisible=true;
	}
}

void QtProfileBar::removeNickNameWidget(){

	if ( _nickNameWidgetVisible ){
		_nickNameWidget->setVisible(false);
		_widgetLayout->removeWidget ( _nickNameWidget );

		setMinimumSize(QSize(0,0));
		resize(QSize(0,0));
//		delete _nickNameWidget;
		_nickNameWidgetVisible=false;
		_nicknameLabel->setSelected(false);
	}
}

void QtProfileBar::eventsClicked(){

	if ( _eventsWidgetVisible )
		removeEventsWidget();
	else
	{
		_nicknameLabel->setSelected(false);
		_eventsLabel->setSelected(true);
		_creditLabel->setSelected(false);
		showEventsWidget();
	}
}

void QtProfileBar::showEventsWidget(){

	removeNickNameWidget();
	removeCreditWidget();
	if ( !_eventsWidgetVisible ){
//		_eventWidget = new QtEventWidget(this);
		_widgetLayout->addWidget(_eventWidget, 1, 0);
		_eventWidget->setVisible(true);
		_eventsWidgetVisible=true;
	}
}

void QtProfileBar::removeEventsWidget(){
	if ( _eventsWidgetVisible ){
		_eventWidget->setVisible(false);
		_widgetLayout->removeWidget( _eventWidget );
		setMinimumSize(QSize(0,0));
		resize(QSize(0,0));
//		delete _eventWidget;
		_eventsWidgetVisible=false;
		_eventsLabel->setSelected(false);
	}
}

void QtProfileBar::creditClicked(){
	if ( _crediWidgetVisible )
		removeCreditWidget();
	else{
		_nicknameLabel->setSelected(false);
		_eventsLabel->setSelected(false);
		_creditLabel->setSelected(true);
		showCreditWidget();
	}
}

void QtProfileBar::showCreditWidget(){
	removeNickNameWidget();
	removeEventsWidget();
	if ( ! _crediWidgetVisible ){
//		_creditWidget = new QtCreditWidget(this);
//		_creditWidget->setCWengoPhone(&_cWengoPhone);
		_widgetLayout->addWidget(_creditWidget, 1, 0);
		_creditWidget->setVisible(true);
		_crediWidgetVisible=true;
	}
}

void QtProfileBar::removeCreditWidget(){

	if ( _crediWidgetVisible ){
		_creditWidget->setVisible(false);
		_widgetLayout->removeWidget( _creditWidget );
		setMinimumSize(QSize(0,0));
		resize(QSize(0,0));
//		delete _creditWidget;
		_crediWidgetVisible=false;
		_creditLabel->setSelected(false);
	}
}


void QtProfileBar::createStatusMenu(){
	if (_statusMenu)
		delete _statusMenu;
	_statusMenu = new QMenu(this);


	QAction * action;

	action = _statusMenu->addAction( tr( "Online" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( onlineClicked(bool) ) );

	action = _statusMenu->addAction( tr( "DND" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( dndClicked(bool) ) );

	action = _statusMenu->addAction( tr( "Invisible" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( invisibleClicked(bool) ) );

	action = _statusMenu->addAction( tr( "Away" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( awayClicked(bool) ) );

	action = _statusMenu->addAction( tr( "Forward to cellphone" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( forwardClicked(bool) ) );

	QPoint p = _statusLabel->pos();

	p.setY( p.y() + _statusLabel->rect().bottom() - 18 );

	_statusMenu->setWindowOpacity(0.95);

	_statusMenu->popup(mapToGlobal(p));
}

void QtProfileBar::onlineClicked(bool){
	_userProfile.setPresenceState(EnumPresenceState::PresenceStateOnline, NULL);
}

void QtProfileBar::dndClicked(bool){
	_userProfile.setPresenceState(EnumPresenceState::PresenceStateDoNotDisturb, NULL);
}

void QtProfileBar::invisibleClicked(bool){
	_userProfile.setPresenceState(EnumPresenceState::PresenceStateInvisible, NULL);
}

void QtProfileBar::awayClicked(bool){
	_userProfile.setPresenceState(EnumPresenceState::PresenceStateAway, NULL);
}

void QtProfileBar::forwardClicked(bool){
	// We should switch to forward only on a Wengo account
	//_userProfile.setPresenceState(EnumPresenceState::PresenceStateForward, NULL);
}

void QtProfileBar::setWengos ( float wengos ){
	// 0x20ac is the unicode code for the euros currency symbol
	_creditLabel->setText(QString() + QChar(0x20ac) + QString(" %1").arg(wengos) );
}

void QtProfileBar::wsInfoCreatedEventHandler(UserProfile & sender, WsInfo & wsInfo) {
	wsInfo.wsInfoWengosEvent += boost::bind(&QtProfileBar::wsInfoWengosEventHandler, this, _1, _2, _3, _4);
	wsInfo.wsInfoVoiceMailEvent+= boost::bind(&QtProfileBar::wsInfoVoiceMailEventHandler, this, _1, _2, _3, _4);
	wsInfo.getWengosCount(true);
	wsInfo.getUnreadVoiceMail(true);
	wsInfo.execute();
}

void QtProfileBar::wsInfoWengosEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status, float wengos) {
	//TODO: add error check
	setWengos(wengos);
}

void QtProfileBar::wsInfoVoiceMailEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status, int voicemail) {
	//TODO: add error check
	_eventWidget->setNewMessages(voicemail);
}
