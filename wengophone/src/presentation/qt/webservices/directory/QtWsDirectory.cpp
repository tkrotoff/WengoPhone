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

#include "ui_WsDirectory.h"

#include "QtWsDirectoryContact.h"

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/profile/QtProfileDetails.h>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <model/contactlist/Contact.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/profile/UserProfile.h>

#include <util/Logger.h>

#include <qtutil/WidgetBackgroundImage.h>

#include <QtGui>

#include <cstdio>

QtWsDirectory::QtWsDirectory(CWsDirectory & cWsDirectory)
	: QObjectThreadSafe(NULL),
	_cWsDirectory(cWsDirectory) {

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWsDirectory::initThreadSafe, this));
	postEvent(event);
}

void QtWsDirectory::initThreadSafe() {
	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cWsDirectory.getCWengoPhone().getPresentation();

	_directoryWindow = new QDialog(qtWengoPhone->getWidget());

	_ui = new Ui::WsDirectory();
	_ui->setupUi(_directoryWindow);

	WidgetBackgroundImage::setBackgroundImage(_ui->searchLabel, ":pics/headers/search.png", true);

	_qtWengoPhone = (QtWengoPhone *) _cWsDirectory.getCWengoPhone().getPresentation();
	_cWsDirectory.contactFoundEvent += boost::bind(&QtWsDirectory::contactFoundEventHandler, this, _1, _2, _3);
	connect(_ui->searchButton, SIGNAL(clicked()), SLOT(searchButtonClicked()));

	_qtWengoPhone->setWsDirectory(this);
}

QtWsDirectory::~QtWsDirectory() {
	_qtWengoPhone->setWsDirectory(NULL);
	delete _ui;
}

QWidget * QtWsDirectory::getWidget() const {
	return _directoryWindow;
}

void QtWsDirectory::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWsDirectory::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtWsDirectory::updatePresentationThreadSafe() {
}

void QtWsDirectory::show() {
	_directoryWindow->show();
}

void QtWsDirectory::clear() {
	_ui->listWidget->clear();
}

void QtWsDirectory::searchButtonClicked() {
	QString query = _ui->searchLineEdit->text();
	LOG_DEBUG("query=" + query.toStdString());

	QString criteria = _ui->criteriaComboBox->currentText();
	LOG_DEBUG("criteria=" + criteria.toStdString());

	if (criteria == "Nickname") {
		_cWsDirectory.searchEntry(query.toStdString(), WsDirectory::WengoId);
	} else if (criteria == "First Name") {
		_cWsDirectory.searchEntry(query.toStdString(), WsDirectory::FirstName);
	} else if (criteria == "Last Name") {
		_cWsDirectory.searchEntry(query.toStdString(), WsDirectory::LastName);
	} else if (criteria == "City") {
		_cWsDirectory.searchEntry(query.toStdString(), WsDirectory::City);
	} else if (criteria == "Country") {
		_cWsDirectory.searchEntry(query.toStdString(), WsDirectory::Country);
	} else {
		_cWsDirectory.searchEntry(query.toStdString());
	}

	clear();
	_ui->searchButton->setEnabled(false);
}

void QtWsDirectory::contactFoundEventHandler(WsDirectory & sender, ContactProfile * contact, bool online) {
	typedef PostEvent3<void (WsDirectory &, ContactProfile *, bool), WsDirectory &, ContactProfile *, bool> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWsDirectory::contactFoundEventHandlerThreadSafe,
		this, _1, _2, _3), sender, contact, online);
	postEvent(event);
}

void QtWsDirectory::contactFoundEventHandlerThreadSafe(WsDirectory & sender, ContactProfile * contact, bool online) {
	//if no contact has been found
	if (!contact) {
		_ui->searchButton->setEnabled(true);
		QMessageBox::warning(_directoryWindow, tr("Search Wengo Contacts"), tr("No contact match your query"));
		return;
	}

	QtWsDirectoryContact * qtWsDirectoryContact = new QtWsDirectoryContact(this, contact, online, _ui->listWidget);

	QListWidgetItem * item = new QListWidgetItem(_ui->listWidget);
	//to avoid having a 16 pixels height item
	item->setSizeHint(qtWsDirectoryContact->getWidget()->minimumSizeHint());
	_ui->listWidget->setItemWidget(item, qtWsDirectoryContact->getWidget());
	_ui->searchButton->setEnabled(true);
}

void QtWsDirectory::callContact(const QString & sipAddress) {
	//get the active phone line from the current user contact & make a call
	_cWsDirectory.getCWengoPhone().getCUserProfileHandler().getCUserProfile()->getUserProfile().getActivePhoneLine()->makeCall(sipAddress.toStdString(), true);
}

void QtWsDirectory::addContact(ContactProfile * contact) {
	ContactProfile contactProfile;
	contactProfile.setFirstName(contact->getFirstName());
	contactProfile.setLastName(contact->getLastName());
	contactProfile.setWengoPhoneNumber(contact->getWengoPhoneNumber());
	contactProfile.setStreetAddress(contact->getStreetAddress());
	contactProfile.setWengoPhoneId(contact->getWengoPhoneId());

	QtProfileDetails qtProfileDetails(*_cWsDirectory.getCWengoPhone().getCUserProfileHandler().getCUserProfile(), contactProfile, _directoryWindow);

	if (qtProfileDetails.show()) {
		_cWsDirectory.getCWengoPhone().getCUserProfileHandler().getCUserProfile()->getCContactList().addContact(contactProfile);
	}
}
