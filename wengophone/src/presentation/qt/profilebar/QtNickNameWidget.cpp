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

#include "QtIMMenu.h"

#include <presentation/qt/login/QtEditMyProfile.h>

#include <model/profile/UserProfile.h>

#include <QFileDialog>

#include <string>

using namespace std;

QtNickNameWidget::QtNickNameWidget(UserProfile & userProfile, QWidget * parent , Qt::WFlags f )
: QWidget(parent,f), _userProfile(userProfile) {
/*
	QPalette p = palette();
	p.setColor(QPalette::Active,QPalette::Window,Qt::white);
	setPalette(p);
	setAutoFillBackground(true);
*/
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

	_msnLabel->setPixmap(QPixmap(":pics/protocol_msn.png"));
	_yahooLabel->setPixmap(QPixmap(":pics/protocol_yahoo.png"));
	_wengoLabel->setPixmap(QPixmap(":pics/protocol_wengo.png"));
	_aimLabel->setPixmap(QPixmap(":pics/protocol_aim.png"));
	_jabberLabel->setPixmap(QPixmap(":pics/protocol_jabber.png"));

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
	_userProfile.setAlias(_nickNameEdit->text().toStdString(), NULL);
}

// Menus

void QtNickNameWidget::showMsnMenu(){
	if (_msnIMAccountMenu)
		delete _msnIMAccountMenu;

	_msnIMAccountMenu = new QMenu(this);
	_msnIMAccountMenu->setWindowOpacity(0.95);

	set<IMAccount *> list;
	list = _userProfile.getIMAccountHandler().getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolMSN);

	for (set<IMAccount *>::const_iterator it = list.begin();
		it != list.end();
		++it) {
		QtIMMenu * menu = new QtIMMenu(_userProfile, *(*it), _msnIMAccountMenu);
		menu->setWindowOpacity(0.95);
		_msnIMAccountMenu->addMenu(menu);
	}

	QPoint p = _msnLabel->pos();
	p.setY(p.y() + _msnLabel->rect().bottom());

	_msnIMAccountMenu->popup(mapToGlobal(p));
}

void QtNickNameWidget::showYahooMenu(){
	if ( _yahooIMAccountMenu )
		delete _yahooIMAccountMenu;

	_yahooIMAccountMenu = new QMenu(this);
	_yahooIMAccountMenu->setWindowOpacity(0.95);

	set<IMAccount *> list;
	list = _userProfile.getIMAccountHandler().getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolYahoo);

	for (set<IMAccount *>::const_iterator it = list.begin();
		it != list.end();
		++it) {
		QtIMMenu * menu = new QtIMMenu(_userProfile, *(*it), _yahooIMAccountMenu);
		menu->setWindowOpacity(0.95);
		_yahooIMAccountMenu->addMenu(menu);
	}

	QPoint p = _yahooLabel->pos();
	p.setY(p.y() + _yahooLabel->rect().bottom());

	_yahooIMAccountMenu->popup(mapToGlobal(p));
}

void QtNickNameWidget::showWengoMenu(){
	if ( _wengoIMAccountMenu )
		delete _wengoIMAccountMenu;

	_wengoIMAccountMenu = new QMenu(this);
	_wengoIMAccountMenu->setWindowOpacity(0.95);

	set<IMAccount *> list;
	list = _userProfile.getIMAccountHandler().getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolSIPSIMPLE);

	for (set<IMAccount *>::const_iterator it = list.begin();
		it != list.end();
		++it) {
		QtIMMenu * menu = new QtIMMenu(_userProfile, *(*it), _wengoIMAccountMenu);
		menu->setWindowOpacity(0.95);
		_wengoIMAccountMenu->addMenu(menu);
	}

	QPoint p = _wengoLabel->pos();
	p.setY(p.y() + _wengoLabel->rect().bottom());

	_wengoIMAccountMenu->popup(mapToGlobal(p));
}

void QtNickNameWidget::showAimMenu(){
	if ( _aimIMAccountMenu )
		delete _aimIMAccountMenu;

	_aimIMAccountMenu = new QMenu(this);
	_aimIMAccountMenu->setWindowOpacity(0.95);

	set<IMAccount *> list;
	list = _userProfile.getIMAccountHandler().getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolAIMICQ);

	for (set<IMAccount *>::const_iterator it = list.begin();
		it != list.end();
		++it) {
		QtIMMenu * menu = new QtIMMenu(_userProfile, *(*it), _aimIMAccountMenu);
		menu->setWindowOpacity(0.95);
		_aimIMAccountMenu->addMenu(menu);
	}

	QPoint p = _aimLabel->pos();
	p.setY(p.y() + _aimLabel->rect().bottom());

	_aimIMAccountMenu->popup(mapToGlobal(p));
}

void QtNickNameWidget::showJabberMenu(){
	if ( _jabberIMAccountMenu)
		delete _jabberIMAccountMenu;

	_jabberIMAccountMenu = new QMenu(this);
	_jabberIMAccountMenu->setWindowOpacity(0.95);

	set<IMAccount *> list;
	list = _userProfile.getIMAccountHandler().getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolAIMICQ);

	for (set<IMAccount *>::const_iterator it = list.begin();
		it != list.end();
		++it) {
		QtIMMenu * menu = new QtIMMenu(_userProfile, *(*it), _jabberIMAccountMenu);
		menu->setWindowOpacity(0.95);
		_jabberIMAccountMenu->addMenu(menu);
	}

	QPoint p = _jabberLabel->pos();
	p.setY(p.y() + _jabberLabel->rect().bottom());

	_jabberIMAccountMenu->popup(mapToGlobal(p));
}

void QtNickNameWidget::init() {
	_nickNameEdit->setText(QString::fromStdString(_userProfile.getAlias()));

	// Setting avatar
	QPixmap pixmap;
	QPixmap background = QPixmap(":/pics/fond_avatar.png");
	string myData = _userProfile.getIcon().getData();
	pixmap.loadFromData((uchar *)myData.c_str(), myData.size());

	if ( pixmap.isNull())
        return;

	QPainter painter( & background );
	painter.drawPixmap(2,2,pixmap.scaled(66,66));
	painter.end;

	// _avatarLabel->setPixmap(pixmap.scaled(_avatarLabel->rect().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	_avatarLabel->setPixmap(background.scaled(_avatarLabel->rect().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

	////
}
