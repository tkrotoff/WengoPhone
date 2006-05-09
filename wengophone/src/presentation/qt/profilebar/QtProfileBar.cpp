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

#include <model/account/wengo/WengoAccount.h>
#include <model/profile/UserProfile.h>
#include <model/presence/PresenceHandler.h>
#include <control/CWengoPhone.h>

#include <util/Logger.h>
#include <imwrapper/IMAccount.h>
#include <qtutil/QtWengoStyleLabel.h>

QtProfileBar::QtProfileBar(CWengoPhone & cWengoPhone, CUserProfile & cUserProfile,
	ConnectHandler & connectHandler, QWidget * parent, Qt::WFlags f)
: QWidget (parent,f), _cUserProfile(cUserProfile),
_cWengoPhone(cWengoPhone), _connectHandler(connectHandler) {

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
	_statusLabel = new QtWengoStyleLabel(this);
	_statusLabel->setMinimumSize(QSize(46,65));
	_statusLabel->setMaximumSize(QSize(46,65));
	_statusLabel->setPixmaps(QPixmap(":/pics/profilebar/bar_start_status_green.png"),
	                QPixmap(), // no end
	                QPixmap(), // no fill

	                QPixmap(":/pics/profilebar/bar_on_start_status_green.png"),
	                QPixmap(),  // no end
	                QPixmap()
	                ); // no fill

	// Nickname label
	_nicknameLabel = new QtWengoStyleLabel(this);
	_nicknameLabel->setMinimumSize(QSize(46,65));
	_nicknameLabel->setMaximumSize(QSize(1000,65));
	_nicknameLabel->setPixmaps(
					QPixmap(), // no start
	                QPixmap(":/pics/profilebar/bar_separator.png"), //  end
	                QPixmap(":/pics/profilebar/bar_fill.png"),
	                QPixmap(),  // no start
	                QPixmap(":/pics/profilebar/bar_on_separator_left.png"),  // end
	                QPixmap(":/pics/profilebar/bar_on_fill.png") // fill
	                );


	// The events label
	_eventsLabel = new QtWengoStyleLabel(this);
	_eventsLabel->setMinimumSize(QSize(46,65));
	_eventsLabel->setMaximumSize(QSize(1000,65));
	_eventsLabel->setPixmaps(QPixmap(), // no start
	                QPixmap(":/pics/profilebar/bar_separator.png"), //  end
	                QPixmap(":/pics/profilebar/bar_fill.png"),
	                QPixmap(),  // no start
	                QPixmap(":/pics/profilebar/bar_on_separator_left.png"),  // end
	                QPixmap(":/pics/profilebar/bar_on_fill.png")
	                );

	// The credit label
	_creditLabel = new QtWengoStyleLabel(this);
	_creditLabel->setMinimumSize(QSize(46,65));
	_creditLabel->setMaximumSize(QSize(1000,65));
	_creditLabel->setPixmaps(QPixmap(), // no start
	                QPixmap(":/pics/profilebar/bar_end.png"),
	                QPixmap(":/pics/profilebar/bar_fill.png"),
	                QPixmap(),  // no start
	                QPixmap(":/pics/profilebar/bar_on_end.png"),
	                QPixmap(":/pics/profilebar/bar_on_fill.png")
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
	_nickNameWidget = new QtNickNameWidget(_userProfile, _cWengoPhone, this);
	_nickNameWidget->setVisible(false);

	_eventWidget = new QtEventWidget(_cWengoPhone, _userProfile, this);
	_eventWidget->setVisible(false);

	_creditWidget = new QtCreditWidget(this);
	_creditWidget->setCWengoPhone(&_cWengoPhone);
	_creditWidget->setVisible(false);

	_cUserProfile.getUserProfile().wsInfoCreatedEvent +=
		boost::bind(&QtProfileBar::wsInfoCreatedEventHandler, this, _1, _2);
	_cUserProfile.getUserProfile().phoneLineCreatedEvent +=
		boost::bind(&QtProfileBar::phoneLineCreatedEventHandler, this, _1, _2);

    _connectHandler.connectedEvent +=
        boost::bind(&QtProfileBar::connectedEventHandler,this,_1,_2);

    _connectHandler.disconnectedEvent +=
        boost::bind(&QtProfileBar::disconnectedEventHandler,this,_1,_2);

    PresenceHandler & presence = _cUserProfile.getUserProfile().getPresenceHandler() ;

    presence.myPresenceStatusEvent +=
        boost::bind(&QtProfileBar::myPresenceStatusEventHandler,this,_1,_2,_3);

    connect(this,SIGNAL(connectEventSignal(IMAccount *)),_nickNameWidget,SLOT(connected(IMAccount *)));
    connect(this,SIGNAL(disconnectedEventSignal(IMAccount *)),_nickNameWidget,SLOT(disconnected(IMAccount *)));

    if ( !connect(this,SIGNAL(myPresenceStatusEventSignal(QVariant  )),
            this,SLOT  (myPresenceStatusEventSlot( QVariant ))) ) {
        LOG_FATAL("Signal / slot connection error");
    }
}

// Called in the model thread
void QtProfileBar::connectedEventHandler(ConnectHandler & sender, IMAccount & imAccount) {
    IMAccount * pImAccount = &imAccount;
    connectEventSignal(pImAccount);
}

// Called in the model thread
void QtProfileBar::disconnectedEventHandler (ConnectHandler & sender, IMAccount & imAccount) {
    IMAccount * pImAccount = &imAccount;
    disconnectedEventSignal(pImAccount);
}

void QtProfileBar::myPresenceStatusEventHandler(PresenceHandler & sender, const IMAccount & imAccount,
		                                        EnumPresenceState::MyPresenceStatus status) {
    EnumPresenceState::MyPresenceStatus * pstatus = new EnumPresenceState::MyPresenceStatus;
    QVariant v(status);
    myPresenceStatusEventSignal(v);
}

void QtProfileBar::myPresenceStatusEventSlot(QVariant status) {

    if ( status.toInt() == (int)EnumPresenceState::MyPresenceStatusOk)

    switch ( _userProfile.getPresenceState() ) {
        case EnumPresenceState::PresenceStateAway:
            setAway();
            break;
        case EnumPresenceState::PresenceStateOnline:
            setOnline();
            break;
        case EnumPresenceState::PresenceStateInvisible:
            setInvisible();
            break;
        case EnumPresenceState::PresenceStateDoNotDisturb:
            setDND();
            break;
        default:
            LOG_DEBUG("Change presence state display to -- Not yet handled\n");
            break;
    }
}

void QtProfileBar::statusClicked() {
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

void QtProfileBar::showNickNameWidget() {

	removeEventsWidget();
	removeCreditWidget();

	if ( ! _nickNameWidgetVisible )
	{
		_widgetLayout->addWidget(_nickNameWidget, 1, 0 );
		_nickNameWidget->setVisible(true);
		_nickNameWidgetVisible=true;
		setOpen(true);
	}
}

void QtProfileBar::removeNickNameWidget() {

	if ( _nickNameWidgetVisible ){
		_nickNameWidget->setVisible(false);
		_widgetLayout->removeWidget ( _nickNameWidget );

		setMinimumSize(QSize(0,0));
		resize(QSize(0,0));
		_nickNameWidgetVisible=false;
		_nicknameLabel->setSelected(false);
		setOpen(false);
	}
}

void QtProfileBar::eventsClicked() {

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

void QtProfileBar::showEventsWidget() {

	removeNickNameWidget();
	removeCreditWidget();
	if ( !_eventsWidgetVisible ){
		_widgetLayout->addWidget(_eventWidget, 1, 0);
		_eventWidget->setVisible(true);
		_eventsWidgetVisible=true;
		setOpen(true);
	}
}

void QtProfileBar::removeEventsWidget() {
	if ( _eventsWidgetVisible ){
		_eventWidget->setVisible(false);
		_widgetLayout->removeWidget( _eventWidget );
		setMinimumSize(QSize(0,0));
		resize(QSize(0,0));
		_eventsWidgetVisible=false;
		_eventsLabel->setSelected(false);
		setOpen(false);
	}
}

void QtProfileBar::creditClicked() {
	if ( _crediWidgetVisible )
		removeCreditWidget();
	else{
		_nicknameLabel->setSelected(false);
		_eventsLabel->setSelected(false);
		_creditLabel->setSelected(true);
		showCreditWidget();
	}
}

void QtProfileBar::showCreditWidget() {
	removeNickNameWidget();
	removeEventsWidget();
	if ( ! _crediWidgetVisible ){
		_widgetLayout->addWidget(_creditWidget, 1, 0);
		_creditWidget->setVisible(true);
		_crediWidgetVisible=true;
		setOpen(true);
	}
}

void QtProfileBar::removeCreditWidget() {

	if ( _crediWidgetVisible ){
		_creditWidget->setVisible(false);
		_widgetLayout->removeWidget( _creditWidget );
		setMinimumSize(QSize(0,0));
		resize(QSize(0,0));
		_crediWidgetVisible=false;
		_creditLabel->setSelected(false);
		setOpen(false);
	}
}

void QtProfileBar::createStatusMenu() {
	if (_statusMenu)
		delete _statusMenu;
	_statusMenu = new QMenu(this);


	QAction * action;

	action = _statusMenu->addAction(QIcon(":/pics/status/online.png"),tr( "Online" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( onlineClicked(bool) ) );

	action = _statusMenu->addAction(QIcon(":/pics/status/donotdisturb.png"), tr( "DND" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( dndClicked(bool) ) );

	action = _statusMenu->addAction(QIcon(":/pics/status/offline.png"), tr( "Invisible" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( invisibleClicked(bool) ) );

	action = _statusMenu->addAction(QIcon(":/pics/status/away.png"), tr( "Away" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( awayClicked(bool) ) );
/*
	action = _statusMenu->addAction(QIcon(":/pics/status/forward.png"), tr( "Forward to cellphone" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( forwardClicked(bool) ) );
*/
	QPoint p = _statusLabel->pos();

	p.setY( p.y() + _statusLabel->rect().bottom() - 18 );

	_statusMenu->setWindowOpacity(0.95);

	_statusMenu->popup(mapToGlobal(p));
}

void QtProfileBar::onlineClicked(bool) {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateOnline, NULL);

}

void QtProfileBar::setOnline() {
    setStatusLabel(":/pics/profilebar/bar_start_status_green.png",
                   ":/pics/profilebar/bar_on_start_status_green.png");
}

void QtProfileBar::dndClicked(bool) {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateDoNotDisturb, NULL);

}

void QtProfileBar::setDND() {
	setStatusLabel(":/pics/profilebar/bar_start_status_red.png",
	               ":/pics/profilebar/bar_on_start_status_red.png");
}

void QtProfileBar::invisibleClicked(bool) {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateInvisible, NULL);
}

void QtProfileBar::setInvisible() {
	setStatusLabel(":/pics/profilebar/bar_start_status_gray.png",
	               ":/pics/profilebar/bar_on_start_status_gray.png");
}

void QtProfileBar::awayClicked(bool) {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateAway, NULL);

}

void QtProfileBar::setAway() {
	setStatusLabel(":/pics/profilebar/bar_start_status_orange.png",
	               ":/pics/profilebar/bar_on_start_status_orange.png");
}

void QtProfileBar::forwardClicked(bool) {
	// We should switch to forward only on a Wengo account
	//_userProfile.setPresenceState(EnumPresenceState::PresenceStateForward, NULL);
}

void QtProfileBar::wsInfoCreatedEventHandler(UserProfile & sender, WsInfo & wsInfo) {
	wsInfo.wsInfoWengosEvent += boost::bind(&QtProfileBar::wsInfoWengosEventHandler, this, _1, _2, _3, _4);
	wsInfo.wsInfoVoiceMailEvent += boost::bind(&QtProfileBar::wsInfoVoiceMailEventHandler, this, _1, _2, _3, _4);
	wsInfo.wsInfoPtsnNumberEvent += boost::bind(&QtProfileBar::wsInfoPtsnNumberEventHandler, this, _1, _2, _3, _4);
	
	wsInfo.wsCallForwardInfoEvent += boost::bind(&QtProfileBar::wsCallForwardInfoEventHandler, this, _1, _2, _3, _4, _5, _6, _7, _8);
	
	wsInfo.getWengosCount(true);
	wsInfo.getUnreadVoiceMail(true);
	wsInfo.getCallForwardInfo(true);
	wsInfo.getPstnNumber(true);
	wsInfo.execute();
}

void QtProfileBar::wsInfoWengosEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status, float wengos) {
	if( status == WsInfo::WsInfoStatusOk ) {
		// 0x20ac is the unicode code for the euros currency symbol
		_creditLabel->setText(QString() + QChar(0x20ac) + QString(" %1").arg(wengos) );
	}
}

void QtProfileBar::wsInfoVoiceMailEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status, int voicemail) {
	if( status == WsInfo::WsInfoStatusOk ) {
		_eventWidget->setVoiceMail(voicemail);
	}
}

void QtProfileBar::wsInfoPtsnNumberEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status, std::string number) {
	if( status == WsInfo::WsInfoStatusOk ) {
		_creditWidget->setPstnNumber(QString::fromStdString(number));
	}
}

void QtProfileBar::wsCallForwardInfoEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status,
	WsInfo::WsInfoCallForwardMode mode, bool voicemail, std::string dest1, std::string dest2, std::string dest3) {
	
	if( status == WsInfo::WsInfoStatusOk) {
		
		switch( mode ) {
			case WsInfo::WsInfoCallForwardModeVoicemail:
				_creditWidget->setCallForwardMode(tr("voicemail"));
				break;
			case WsInfo::WsInfoCallForwardModeNumber:
				_creditWidget->setCallForwardMode(tr("active"));
				break;
			case WsInfo::WsInfoCallForwardMode_Disabled:
				_creditWidget->setCallForwardMode(tr("inactive"));
				break;
			case WsInfo::WsInfoCallForwardMode_Unauthorized:
				_creditWidget->setCallForwardMode(tr("Unauthorized"));
				break;
		}
	}
}

void QtProfileBar::setOpen(bool status) {
    if (status)
    {
        QColor background = QColor(189,189,189);
        QPalette p = palette();
        p.setColor(QPalette::Window, background );
        _statusLabel->setBackgroundColor(background);
        _nicknameLabel->setBackgroundColor(background);
        _eventsLabel->setBackgroundColor(background);
        _creditLabel->setBackgroundColor(background);
        setPalette(p);

    }
    else{
        QPalette p = (dynamic_cast<QWidget *>( parent()))->palette();
        setPalette(p);
        QColor background = p.color(QPalette::Window);
        _statusLabel->setBackgroundColor(background);
        _nicknameLabel->setBackgroundColor(background);
        _eventsLabel->setBackgroundColor(background);
        _creditLabel->setBackgroundColor(background);
    }
}

void QtProfileBar::setStatusLabel(const QString & on, const QString & off) {

	_statusLabel->setPixmaps(QPixmap(on),
	                QPixmap(), // no end
	                QPixmap(), // no fill

	                QPixmap(off),
	                QPixmap(),  // no end
	                QPixmap()
	                ); // no fill
    _statusLabel->update();
}

void QtProfileBar::phoneLineCreatedEventHandler(UserProfile & sender, IPhoneLine & phoneLine) {
	_nicknameLabel->setText(QString::fromStdString(_cUserProfile.getUserProfile().getWengoAccount()->getIdentity()));
}

void QtProfileBar::userProfileUpdated() {
	_nickNameWidget->userProfileUpdated();
}

