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

#include "SearchPageWidget.h"

#include "MyWidgetFactory.h"
#include "Softphone.h"
#include "login/User.h"
#include "MainWindow.h"
#include "contact/ContactWindow.h"
#include "contact/ContactWidget.h"
#include "contact/Contact.h"
#include "contact/ContactListWidget.h"
#include "contact/StreetAddress.h"
#include "sip/SipAddress.h"
#include "config/Config.h"
#include "AudioCallManager.h"
#include "softphone-config.h"

#include <qwidget.h>
#include <qaxwidget.h>
#include <qstring.h>
#include <qt.h>
#include <qapplication.h>
#include <qdir.h>
#include <qurl.h> 

#include <Exdisp.h>

#include <iostream>
using namespace std;

/**
 * Search buddies widget
 * Integrate a Internet Explorer ActiveX
 *
 * @author Mathieu Stute
 */

const QString SearchPageWidget::ANCHOR_ADDCONTACT = "addcontact";
const QString SearchPageWidget::ANCHOR_CALLCONTACT = "callto:";

SearchPageWidget::SearchPageWidget(QWidget * parent) : QObject(parent) {
	_searchPageWidget = MyWidgetFactory::create("SearchPageWidgetForm.ui", this, parent);
	_ieActiveX = (QAxWidget*)_searchPageWidget->child("ieActiveX", "QAxWidget");
	_ieActiveX->setFocusPolicy(QWidget::NoFocus);
	
	Config & config = Config::getInstance();
	config.addObserver(*this);
	
	connect( _ieActiveX, SIGNAL( BeforeNavigate(const QString&,int,const QString&,const QVariant&,const QString&,bool&))
	, SLOT(BeforeNavigateSlot(const QString&,int,const QString&,const QVariant&,const QString&,bool&)));
	
	reloadPage();
	/*
	Config & conf = Config::getInstance();
	const QString langCode = conf.getLanguageCode();
	_ieActiveX->dynamicCall("Navigate(const QString&)", Softphone::URL_SMART_DIRECTORY + "&lang=" + langCode);
	*/
	
	_searchPageWidget->resize(707, 646);
	_searchPageWidget->setMinimumWidth(550);
	_ieActiveX->setMinimumWidth(550);
}

SearchPageWidget::~SearchPageWidget() {
	delete _ieActiveX;
}

void SearchPageWidget::update(const Subject &, const Event &) {
	cout << "SearchPageWidget: coucou\n";
	reloadPage();
}

void SearchPageWidget::reloadPage() {
	Config & conf = Config::getInstance();
	const QString langCode = conf.getLanguageCode();
	setUrl(Softphone::URL_SMART_DIRECTORY + "&lang=" + langCode);
}

void SearchPageWidget::setUrl(const QString & url) {
	_ieActiveX->dynamicCall("Navigate(const QString&)", url);
}

void SearchPageWidget::BeforeNavigateSlot(const QString & s,int,const QString &,const QVariant&,const QString&,bool&) {
	
	QString & decoded_url = (QString &)s;
	QUrl::decode(decoded_url);
	QString anchor = getAnchor(decoded_url);
	
	if( isAddContactAnchor(anchor) ) {
		MainWindow & mainWindow = Softphone::getInstance().getMainWindow();
		ContactListWidget * contactListWidget = mainWindow.getContactListWidget();
		ContactWindow * contactWindow = contactListWidget->getContactWindow();
		ContactWidget * contactWidget = contactWindow->getContactWidget();
		Contact *contact = new Contact();
		
		QStringList l = QStringList::split(":", anchor, true);
		//prevents from bad web values
		if( l.size() == 7) {
			QStringList::iterator it;
			for( it = l.begin(); it != l.end(); it++) {
				if(!(*it)) {
					*it = "";
				}
			}
			/*
			// debug trace
			for( it = l.begin(); it != l.end(); it++) {
				cout << *it << endl;
			}
			*/
			contact->setWengoPhone(l[1]);
			contact->setLastName(l[2]);
			contact->setFirstName(l[3]);
			StreetAddress add;
			add.setCity(l[5]);
			add.setCountry(l[6]);
			contact->setStreetAddress(add);
		}
		
		contactWidget->setContact(*contact);
		contactWindow->execShowContact();
	} else if( isCallToAnchor(anchor) ) {
		AudioCallManager::getInstance().createAudioCall(SipAddress::fromPhoneNumber(extractContactFromAnchor(anchor)));
	}
}

QString SearchPageWidget::getAnchor(QString url) {
	int sharpPos = url.find('#');
	if( sharpPos != -1 ) {
		return url.right( url.length() - sharpPos - 1 );
	}
	return "";
}

bool SearchPageWidget::isCallToAnchor(QString anchor) {
	if(anchor.length() >= QString(ANCHOR_CALLCONTACT).length()) {
		QString temp = anchor.left(QString(ANCHOR_CALLCONTACT).length());
		if( temp == ANCHOR_CALLCONTACT ){
			return true;
		}
	}
	return false;
}

bool SearchPageWidget::isAddContactAnchor(QString anchor) {
	if(anchor.length() >= QString(ANCHOR_ADDCONTACT).length()) {
		QString temp = anchor.left(QString(ANCHOR_ADDCONTACT).length());
		if( temp == ANCHOR_ADDCONTACT ){
			return true;
		}
	}
	return false;
}

QString SearchPageWidget::extractContactFromAnchor(QString anchor) {
	if(anchor.length() >= QString(ANCHOR_CALLCONTACT).length()) {
		QString temp = anchor.right(anchor.length() - QString(ANCHOR_CALLCONTACT).length());
		return temp;
	}
	return QString::null;
}
