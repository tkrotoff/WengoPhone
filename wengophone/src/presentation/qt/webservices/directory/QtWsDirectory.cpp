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

#include "QtWsDirectory.h"

#include "QtWsDirectoryContact.h"

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/profile/QtProfileDetails.h>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>

#include <model/contactlist/Contact.h>
#include <model/phoneline/IPhoneLine.h>

#include <util/Logger.h>

#include <QtGui>

#include <cstdio>

QtWsDirectory::QtWsDirectory(CWsDirectory & cWsDirectory, QObject * parent)
	: QObjectThreadSafe(parent), _cWsDirectory(cWsDirectory) {

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWsDirectory::initThreadSafe, this));
	postEvent(event);
}

QtWsDirectory::~QtWsDirectory() {
}

void QtWsDirectory::initThreadSafe() {
	_widget = new QWidget();
	ui.setupUi(_widget);
	_widget->setWindowTitle(tr("Search Wengo Contacts"));

	_qtWengoPhone = (QtWengoPhone *) _cWsDirectory.getCWengoPhone().getPresentation();
	_cWsDirectory.contactFoundEvent += boost::bind(&QtWsDirectory::contactFoundEventHandler, this, _1, _2, _3);
	connect(ui.searchPushButton, SIGNAL(clicked()), SLOT(searchButtonClicked()));

	_qtWengoPhone->setWsDirectory(this);
}

void QtWsDirectory::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWsDirectory::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtWsDirectory::updatePresentationThreadSafe() {
}

void QtWsDirectory::show() {
	_widget->show();
}

void QtWsDirectory::clear() {
	ui.listWidget->clear();
}

QWidget * QtWsDirectory::getWidget() {
	return _widget;
}

void QtWsDirectory::searchButtonClicked() {
	QString query = ui.searchLineEdit->text();
	printf("query : %s\n", query.toStdString().c_str());

	QString criteria = ui.criteriaComboBox->currentText();
	printf("using criteria: %s\n", criteria.toStdString().c_str());

	if( criteria == "Nickname") {
		_cWsDirectory.searchEntry(query.toStdString(), WsDirectory::wengoid);
	} else if( criteria == "First Name" ) {
		_cWsDirectory.searchEntry(query.toStdString(), WsDirectory::fname);
	} else if( criteria == "Last Name" ) {
		_cWsDirectory.searchEntry(query.toStdString(), WsDirectory::lname);
	} else if( criteria == "City" ) {
		_cWsDirectory.searchEntry(query.toStdString(), WsDirectory::city);
	} else if( criteria == "Country" ) {
		_cWsDirectory.searchEntry(query.toStdString(), WsDirectory::country);
	} else {
		_cWsDirectory.searchEntry(query.toStdString());
	}

	clear();
	ui.searchPushButton->setEnabled(false);
}

void QtWsDirectory::contactFoundEventHandler(WsDirectory & sender, Profile * profile, bool online) {
	typedef PostEvent3<void (WsDirectory &, Profile *, bool), WsDirectory &, Profile *, bool> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWsDirectory::contactFoundEventHandlerThreadSafe,
		this, _1, _2, _3), sender, profile, online);
	postEvent(event);
}

void QtWsDirectory::contactFoundEventHandlerThreadSafe(WsDirectory & sender, Profile * profile, bool online) {
	//if no contact has been found
	if( !profile ) {
		ui.searchPushButton->setEnabled(true);
		QMessageBox::warning(_widget, tr("Search Wengo Contacts"), tr("No contact match your query"));
		return;
	}

	QtWsDirectoryContact * contact = new QtWsDirectoryContact(this, profile, online, 0);

	QListWidgetItem * item = new QListWidgetItem(ui.listWidget);
	//to avoid having a 16 pixels height item
	item->setSizeHint(contact->minimumSizeHint());
	ui.listWidget->setItemWidget(item, contact);
	ui.searchPushButton->setEnabled(true);
}

void QtWsDirectory::callContact(const QString & sipAddress) {
	//get the active phone line from the current user profile & make a call
	_cWsDirectory.getCWengoPhone().getCUserProfile()->getUserProfile().getActivePhoneLine()->makeCall(sipAddress.toStdString(), 1);
}

void QtWsDirectory::addContact(Profile * profile) {
	ContactProfile contactProfile;
	contactProfile.setFirstName(profile->getFirstName());
	contactProfile.setLastName(profile->getLastName());
	contactProfile.setWengoPhoneNumber(profile->getWengoPhoneNumber());
	contactProfile.setStreetAddress(profile->getStreetAddress());
	contactProfile.setWengoPhoneId(profile->getWengoPhoneId());

	QtProfileDetails qtProfileDetails(_cWsDirectory.getCWengoPhone(), 
		*_cWsDirectory.getCWengoPhone().getCUserProfile(), contactProfile, _widget);

	if (qtProfileDetails.show()) {
		_cWsDirectory.getCWengoPhone().getCUserProfile()->getCContactList().addContact(contactProfile);
	}
}
