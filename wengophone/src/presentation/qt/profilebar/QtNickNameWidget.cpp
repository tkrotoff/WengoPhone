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

#include "QtAvatarChooser.h"
#include "QtIMMenu.h"

#include <model/profile/UserProfile.h>

#include <control/profile/CUserProfile.h>
#include <control/CWengoPhone.h>

#include <presentation/qt/profile/QtProfileDetails.h>
#include <presentation/qt/imaccount/QtIMAccountManager.h>

#include <util/Logger.h>

#include <QFileDialog>

#include <string>
using namespace std;

const QString QtNickNameWidget::PICS_MSN_ON = ":pics/protocols/msn.png";
const QString QtNickNameWidget::PICS_MSN_OFF = ":pics/protocols/msn_off.png";
const QString QtNickNameWidget::PICS_YAHOO_ON = ":pics/protocols/yahoo.png";
const QString QtNickNameWidget::PICS_YAHOO_OFF = ":pics/protocols/yahoo_off.png";
const QString QtNickNameWidget::PICS_WENGO_ON = ":pics/protocols/wengo.png";
const QString QtNickNameWidget::PICS_WENGO_OFF = ":pics/protocols/wengo_off.png";
const QString QtNickNameWidget::PICS_AIM_ON = ":pics/protocols/aim.png";
const QString QtNickNameWidget::PICS_AIM_OFF = ":pics/protocols/aim_off.png";
const QString QtNickNameWidget::PICS_JABBER_ON = ":pics/protocols/jabber.png";
const QString QtNickNameWidget::PICS_JABBER_OFF = ":pics/protocols/jabber_off.png";
const double QtNickNameWidget::POPUPMENU_OPACITY = 0.95;

QtNickNameWidget::QtNickNameWidget(CUserProfile & cUserProfile, CWengoPhone & cWengoPhone, QWidget * parent , Qt::WFlags f )
: QWidget(parent,f), _cUserProfile(cUserProfile), _cWengoPhone(cWengoPhone) {

	_msnIMAccountMenu = NULL;
	_yahooIMAccountMenu = NULL;
	_jabberIMAccountMenu = NULL;
	_wengoIMAccountMenu = NULL;
	_aimIMAccountMenu = NULL;

	// Layouts
	_widgetLayout = new QGridLayout( this );
	_widgetLayout->setSpacing(5);
	_widgetLayout->setMargin(5);

	_protocolLayout = new QGridLayout();
	_protocolLayout->setSpacing(5);
	_protocolLayout->setMargin(0);

	// Child widgets
	_msnLabel = new QtClickableLabel(this);
	_yahooLabel = new QtClickableLabel(this);
	_wengoLabel = new QtClickableLabel(this);
	_aimLabel = new QtClickableLabel(this);
	_avatarLabel = new QtClickableLabel(this);
	_jabberLabel = new QtClickableLabel(this);
	_nickNameEdit = new QLineEdit(this);

	_avatarLabel->setMinimumSize( QSize( 70,70 ) );
	_avatarLabel->setMaximumSize( QSize( 70,70 ) );

	// Add widgets to the layouts
    QVBoxLayout * vboxLayout = new QVBoxLayout();
	_widgetLayout->addWidget( _avatarLabel,   0 , 0 );
	//_widgetLayout->addWidget( _nickNameEdit,  0 , 1 );
	vboxLayout->addWidget(_nickNameEdit, 0);
    vboxLayout->addLayout(_protocolLayout,1);
    _widgetLayout->addLayout(vboxLayout,0,1);
	//_widgetLayout->addLayout( _protocolLayout,1 , 1 );

	_protocolLayout->setColumnStretch(5, 1);
	_protocolLayout->addWidget(_msnLabel     ,0 , 0 );
	_protocolLayout->addWidget(_yahooLabel   ,0 , 1 );
	_protocolLayout->addWidget(_wengoLabel   ,0 , 2 );
	_protocolLayout->addWidget(_aimLabel     ,0 , 3 );
	_protocolLayout->addWidget(_jabberLabel  ,0 , 4 );

	_msnLabel->setPixmap(QPixmap(PICS_MSN_OFF));
	_msnLabel->setToolTip(tr("MSN"));
	_yahooLabel->setPixmap(QPixmap(PICS_YAHOO_OFF));
	_yahooLabel->setToolTip(tr("Yahoo!"));
	_wengoLabel->setPixmap(QPixmap(PICS_WENGO_OFF));
	_wengoLabel->setToolTip(tr("Wengo"));
	_aimLabel->setPixmap(QPixmap(PICS_AIM_OFF));
	_aimLabel->setToolTip(tr("AIM / ICQ"));
	_jabberLabel->setPixmap(QPixmap(PICS_JABBER_OFF));
	_jabberLabel->setToolTip(tr("Jabber / GoogleTalk"));

	// Widget connections
	connect ( _msnLabel,SIGNAL( clicked() ),SLOT( msnClicked() ) );
	connect ( _yahooLabel, SIGNAL( clicked() ), SLOT ( yahooClicked() ) );
	connect ( _wengoLabel, SIGNAL( clicked() ), SLOT ( wengoClicked() ) );
	connect ( _aimLabel, SIGNAL ( clicked() ), SLOT ( aimClicked() ) );
	connect ( _jabberLabel, SIGNAL ( clicked() ), SLOT ( jabberClicked() ) );
	connect ( _nickNameEdit, SIGNAL(returnPressed ()), SLOT(nicknameChanged()));
	connect ( _avatarLabel, SIGNAL ( clicked() ), SLOT ( avatarClicked() ) );
	connect ( _avatarLabel, SIGNAL ( rightClicked() ), SLOT ( avatarRightClicked() ));

	// Widget initialization
	init();
}

void QtNickNameWidget::connected(IMAccount * pImAccount) {
	EnumIMProtocol::IMProtocol imProtocol = pImAccount->getProtocol();

	switch (imProtocol) {
	case EnumIMProtocol::IMProtocolMSN:
		_msnLabel->setPixmap(QPixmap(PICS_MSN_ON));
		break;

	case EnumIMProtocol::IMProtocolSIPSIMPLE:
		_wengoLabel->setPixmap(QPixmap(PICS_WENGO_ON));
		break;

	case EnumIMProtocol::IMProtocolYahoo:
		_yahooLabel->setPixmap(QPixmap(PICS_YAHOO_ON));
		break;

	case EnumIMProtocol::IMProtocolAIMICQ:
		_aimLabel->setPixmap(QPixmap(PICS_AIM_ON));
		break;

	case EnumIMProtocol::IMProtocolJabber:
		_jabberLabel->setPixmap(QPixmap(PICS_JABBER_ON));
		break;

	default:
		LOG_FATAL("unknown IM protocol=" + String::fromNumber(imProtocol));
	}
}

void QtNickNameWidget::disconnected(IMAccount * pImAccount) {
	EnumIMProtocol::IMProtocol imProtocol = pImAccount->getProtocol();

	switch (pImAccount->getProtocol()){
		case EnumIMProtocol::IMProtocolMSN:
			_msnLabel->setPixmap(QPixmap(PICS_MSN_OFF));
            break;

		case EnumIMProtocol::IMProtocolSIPSIMPLE:
			_wengoLabel->setPixmap(QPixmap(PICS_WENGO_OFF));
            break;

		case EnumIMProtocol::IMProtocolYahoo:
			_yahooLabel->setPixmap(QPixmap(PICS_YAHOO_OFF));
            break;

		case EnumIMProtocol::IMProtocolAIMICQ:
			_aimLabel->setPixmap(QPixmap(PICS_AIM_OFF));
            break;

		case EnumIMProtocol::IMProtocolJabber:
			_jabberLabel->setPixmap(QPixmap(PICS_JABBER_OFF));
            break;

		default:
			LOG_FATAL("unknown IM protocol=" + String::fromNumber(imProtocol));
    }
}

void QtNickNameWidget::msnClicked(){
	if( QVariant(QPixmap(PICS_MSN_ON)) ==  QVariant(*_wengoLabel->pixmap()) ) {
		showMsnMenu();
	} else {
		showImAccountManager();
	}
}

void QtNickNameWidget::yahooClicked(){
	if( QVariant(QPixmap(PICS_YAHOO_ON)) ==  QVariant(*_wengoLabel->pixmap()) ) {
		showYahooMenu();
	} else {
		showImAccountManager();
	}

}

void QtNickNameWidget::wengoClicked() {
	if( QVariant(QPixmap(PICS_WENGO_ON)) ==  QVariant(*_wengoLabel->pixmap()) ) {
		showWengoMenu();
	} else {
		showImAccountManager();
	}
}

void QtNickNameWidget::aimClicked() {
	if( QVariant(QPixmap(PICS_AIM_ON)) ==  QVariant(*_wengoLabel->pixmap()) ) {
		showAimMenu();
	} else {
		showImAccountManager();
	}
}

void QtNickNameWidget::jabberClicked() {
	if( QVariant(QPixmap(PICS_JABBER_ON)) ==  QVariant(*_wengoLabel->pixmap()) ) {
		showJabberMenu();
	} else {
		showImAccountManager();
	}
}

void QtNickNameWidget::avatarClicked() {
	QtProfileDetails qtProfileDetails(_cWengoPhone, _cUserProfile.getUserProfile(), this);
	//TODO: UserProfile must be updated if QtProfileDetails was accepted
	qtProfileDetails.show();
	qtProfileDetails.changeUserProfileAvatar();
	updateAvatar();
}

void QtNickNameWidget::avatarRightClicked() {
	QtProfileDetails qtProfileDetails(_cWengoPhone, _cUserProfile.getUserProfile(), this);
	//TODO: UserProfile must be updated if QtProfileDetails was accepted
	qtProfileDetails.show();
	updateAvatar();
}

void QtNickNameWidget::nicknameChanged() {
	_cUserProfile.getUserProfile().setAlias(_nickNameEdit->text().toStdString(), NULL);
}

void QtNickNameWidget::showMsnMenu() {

	if(_msnIMAccountMenu) {
		delete _msnIMAccountMenu;
	}

	_msnIMAccountMenu = new QMenu(this);
	_msnIMAccountMenu->setWindowOpacity(POPUPMENU_OPACITY);

	set<IMAccount *> list;
	list = _cUserProfile.getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolMSN);

	for (set<IMAccount *>::const_iterator it = list.begin();
		it != list.end();
		++it) {
		QtIMMenu * menu = new QtIMMenu(_cUserProfile, *(*it), _msnIMAccountMenu);
		menu->setWindowOpacity(POPUPMENU_OPACITY);
		_msnIMAccountMenu->addMenu(menu);
	}

	QPoint p = _msnLabel->pos();
	p.setY(p.y() + _msnLabel->rect().bottom());

	_msnIMAccountMenu->popup(mapToGlobal(p));
}

void QtNickNameWidget::showYahooMenu() {

	if(_yahooIMAccountMenu) {
		delete _yahooIMAccountMenu;
	}

	_yahooIMAccountMenu = new QMenu(this);
	_yahooIMAccountMenu->setWindowOpacity(POPUPMENU_OPACITY);

	set<IMAccount *> list;
	list = _cUserProfile.getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolYahoo);

	for (set<IMAccount *>::const_iterator it = list.begin();
		it != list.end();
		++it) {
		QtIMMenu * menu = new QtIMMenu(_cUserProfile, *(*it), _yahooIMAccountMenu);
		menu->setWindowOpacity(POPUPMENU_OPACITY);
		_yahooIMAccountMenu->addMenu(menu);
	}

	QPoint p = _yahooLabel->pos();
	p.setY(p.y() + _yahooLabel->rect().bottom());

	_yahooIMAccountMenu->popup(mapToGlobal(p));
}

void QtNickNameWidget::showWengoMenu() {

	if(_wengoIMAccountMenu) {
		delete _wengoIMAccountMenu;
	}

	_wengoIMAccountMenu = new QMenu(this);
	_wengoIMAccountMenu->setWindowOpacity(POPUPMENU_OPACITY);

	set<IMAccount *> list;
	list = _cUserProfile.getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolSIPSIMPLE);

	for (set<IMAccount *>::const_iterator it = list.begin();
		it != list.end();
		++it) {
		QtIMMenu * menu = new QtIMMenu(_cUserProfile, *(*it), _wengoIMAccountMenu);
		menu->setWindowOpacity(POPUPMENU_OPACITY);
		_wengoIMAccountMenu->addMenu(menu);
	}

	QPoint p = _wengoLabel->pos();
	p.setY(p.y() + _wengoLabel->rect().bottom());

	_wengoIMAccountMenu->popup(mapToGlobal(p));
}

void QtNickNameWidget::showAimMenu() {

	if(_aimIMAccountMenu) {
		delete _aimIMAccountMenu;
	}

	_aimIMAccountMenu = new QMenu(this);
	_aimIMAccountMenu->setWindowOpacity(POPUPMENU_OPACITY);

	set<IMAccount *> list;
	list = _cUserProfile.getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolAIMICQ);

	for (set<IMAccount *>::const_iterator it = list.begin();
		it != list.end();
		++it) {
		QtIMMenu * menu = new QtIMMenu(_cUserProfile, *(*it), _aimIMAccountMenu);
		menu->setWindowOpacity(POPUPMENU_OPACITY);
		_aimIMAccountMenu->addMenu(menu);
	}

	QPoint p = _aimLabel->pos();
	p.setY(p.y() + _aimLabel->rect().bottom());

	_aimIMAccountMenu->popup(mapToGlobal(p));
}

void QtNickNameWidget::showJabberMenu() {

	if(_jabberIMAccountMenu) {
		delete _jabberIMAccountMenu;
	}

	_jabberIMAccountMenu = new QMenu(this);
	_jabberIMAccountMenu->setWindowOpacity(POPUPMENU_OPACITY);

	set<IMAccount *> list = _cUserProfile.getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolJabber);
	set<IMAccount *>::const_iterator it;

	for (it = list.begin(); it != list.end(); ++it) {
		QtIMMenu * menu = new QtIMMenu(_cUserProfile, *(*it), _jabberIMAccountMenu);
		menu->setWindowOpacity(POPUPMENU_OPACITY);
		_jabberIMAccountMenu->addMenu(menu);
	}

	QPoint p = _jabberLabel->pos();
	p.setY(p.y() + _jabberLabel->rect().bottom());

	_jabberIMAccountMenu->popup(mapToGlobal(p));
}

void QtNickNameWidget::init() {
	_nickNameEdit->setText(QString::fromStdString(_cUserProfile.getUserProfile().getAlias()));
	updateAvatar();
}

void QtNickNameWidget::userProfileUpdated() {
	init();
}

void QtNickNameWidget::updateAvatar() {

	QPixmap pixmap;
	QPixmap background = QPixmap(":/pics/fond_avatar.png");
	string myData = _cUserProfile.getUserProfile().getIcon().getData();
	pixmap.loadFromData((uchar *)myData.c_str(), myData.size());

	if ( !pixmap.isNull()) {
		QPainter painter( & background );
		painter.drawPixmap(5,5,pixmap.scaled(60,60));
		painter.end();
		_avatarLabel->setPixmap(background);
	}
	else {
		_avatarLabel->setPixmap(background);
	}
}

void QtNickNameWidget::showImAccountManager() {

	QtIMAccountManager imAccountManager(_cWengoPhone.getCUserProfile()->getUserProfile(),
		_cWengoPhone, true, this);
}
