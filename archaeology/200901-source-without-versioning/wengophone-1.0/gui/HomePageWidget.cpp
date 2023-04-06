/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "HomePageWidget.h"

#include "MyWidgetFactory.h"
#include "Softphone.h"
#include "login/User.h"
#include "MainWindow.h"
#include "WebBrowser.h"
#include "contact/ContactWindow.h"
#include "contact/ContactListWidget.h"
#include "config/Config.h"

#include "softphone-config.h"

#include <qwidget.h>
#include <qaxwidget.h>
#include <qstring.h>
#include <qt.h>
#include <qapplication.h>
#include <qdir.h>

#include <iostream>
using namespace std;

const QString HomePageWidget::ANCHOR_CONTACTLIST = "openwengo_phonebook";
const QString HomePageWidget::ANCHOR_HISTORY = "openwengo_log";
const QString HomePageWidget::ANCHOR_CONFIGURATION = "openwengo_configuration";
const QString HomePageWidget::ANCHOR_DIALPAD = "openwengo_dial";
const QString HomePageWidget::ANCHOR_ADDCONTACT = "openwengo_addcontact";
const QString HomePageWidget::ANCHOR_SELFCARE = "openwengo_selfcare";
const QString HomePageWidget::ANCHOR_FORUM = "openwengo_forum";

HomePageWidget::HomePageWidget(QWidget * parent) : QObject(parent){
	_homePageWidget = MyWidgetFactory::create("HomePageWidgetForm.ui", this, parent);
	_ieActiveX = (QAxWidget*)_homePageWidget->child("ieActiveX", "QAxWidget");
	
	connect( _ieActiveX, SIGNAL( BeforeNavigate(const QString&,int,const QString&,const QVariant&,const QString&,bool&))
	, SLOT(BeforeNavigateSlot(const QString&,int,const QString&,const QVariant&,const QString&,bool&)));
	
	Config & conf = Config::getInstance();
	const QString langCode = conf.getLanguageCode();
	
	QString htmlFile;
	if( langCode == "eng" ) {
		htmlFile = "connecting_en.htm";
	} else
	if( langCode == "fra" ) {
		htmlFile = "connecting_fr.htm";
	} else {
		htmlFile = "connecting_en.htm";
	}
	
	QString homePage = qApp->applicationDirPath() + QDir::separator() + "connecting" + QDir::separator() + htmlFile;
	_ieActiveX->dynamicCall("Navigate(const QString&)",homePage);
}

HomePageWidget::~HomePageWidget(){
}

void HomePageWidget::setUrl(const QString & url){
	_ieActiveX->dynamicCall("Navigate(const QString&)", url);
}

void HomePageWidget::goToSoftphoneWeb(bool Switch){
	Softphone & softphone = Softphone::getInstance();
	const User & user = softphone.getUser();
	
	QString login = user.getLogin();
	QString password = user.getPassword();
	
	Config & conf = Config::getInstance();
	const QString langCode = conf.getLanguageCode();
	const QString & data = "?login=" + login + "&password=" + password + "&lang=" + langCode + "&wl=" + QString(WL_TAG) + "&page=softphone-web";
	
	setUrl( Softphone::URL_WENGO_ACCOUNT_AUTH + data );
	MainWindow & mainWindow = softphone.getMainWindow();
	/*
	if( Switch ) {
		mainWindow.raiseHomePage();
	}
	*/
}

void HomePageWidget::BeforeNavigateSlot(const QString & s,int,const QString &,const QVariant&,const QString&,bool&){
	Softphone & softphone = Softphone::getInstance();
	MainWindow & mainWindow = softphone.getMainWindow();
	Config & conf = Config::getInstance();
	const QString langCode = conf.getLanguageCode();
	
	QString anchor = getAnchor(s);
	if( anchor == ANCHOR_CONTACTLIST ){
		mainWindow.raiseContactList();
	}
	else if( anchor == ANCHOR_HISTORY ){
		mainWindow.raiseHistory();
	}
	else if( anchor == ANCHOR_CONFIGURATION ){
		mainWindow.showConfigurationWindow();
	}
	else if( anchor == ANCHOR_DIALPAD ){
		mainWindow.showDialpad();
	}
	else if( anchor == ANCHOR_ADDCONTACT ){
		mainWindow.showAddContact();
	}
	else if( anchor == ANCHOR_SELFCARE ){
		mainWindow.showMyWengoAccount();
	}
	else if( anchor == ANCHOR_FORUM ){
		mainWindow.showForum();
	}
}

QString HomePageWidget::getAnchor(QString url){
	int sharpPos = url.find('#');
	if( sharpPos != -1 ){
		return url.right( url.length() - sharpPos - 1 );
	}
	return "";
}
