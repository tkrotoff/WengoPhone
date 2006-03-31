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

#include <control/contactlist/CContact.h>

#include <model/contactlist/Contact.h>
#include <model/profile/StreetAddress.h>

#include <qtutil/Object.h>
#include <qtutil/WidgetFactory.h>

#include <util/Logger.h>
#include <util/Date.h>

#include "QtEditContactProfile.h"

QtEditContactProfile::QtEditContactProfile(CContact & cContact, QWidget * parent, Qt::WFlags f ) 
: QDialog( parent, f ), _cContact(cContact) {
	_widget = qobject_cast<QWidget *>( WidgetFactory::create( ":/forms/login/contactWindow.ui", this ) );
	layout = new QGridLayout( this );
	layout->addWidget( _widget );

	resize( QSize( 619, 572 ) );
	init();
	hideAccountWidgets();
	readFromConfig();

	connect( _saveChange, SIGNAL( clicked() ), this, SLOT( saveClicked() ) );
	connect( _cancelChange, SIGNAL( clicked() ), this, SLOT( cancelClicked() ) );
}

void QtEditContactProfile::saveClicked() {
	writeToConfig();
	accept();
}

void QtEditContactProfile::cancelClicked() {
	reject();
}

void QtEditContactProfile::hideAccountWidgets() {
	for ( int i = 1; i < _imAccountLineEdit.size(); i++ ) {
		_imAccountLineEdit[ i ] ->setVisible( false );
		_imAccountLineEdit[ i ] ->setReadOnly( true );
	}

	for ( int i = 1; i < _imAccountsPic.size(); i++ ) {
		_imAccountsPic[ i ] ->setVisible( false );
	}
}

void QtEditContactProfile::writeToConfig() {
	Contact & c = _cContact.getContact();

	// Setting wengo id
	c.setWengoPhoneId(_alias->text().toStdString());
	////

	// Setting name
	c.setFirstName( _firstName->text().toStdString() );
	c.setLastName( _lastName->text().toStdString() );
	////

	// Setting sex
	c.setSex((EnumSex::Sex) _gender->currentIndex());
	////

	// Setting birthday
	QDate qdate = _birthDate->date();
	Date date;
	date.setDay( qdate.day() );
	date.setMonth( qdate.month() );
	date.setYear( qdate.year() );
	c.setBirthdate( date );
	////

	// Setting address
	StreetAddress address;
	//address.setStateProvince(_state->text().toStdString());
	//address.setCountry(_country->text().toStdString());
	address.setCity(_city->text().toStdString());
	c.setStreetAddress(address);
	////

	// Setting IMAccounts
	////

	// Setting phone numbers
	c.setMobilePhone(_cellPhone->text().toStdString());
	c.setWorkPhone(_workPhone->text().toStdString());
	c.setHomePhone(_homePhone->text().toStdString());
	c.setWengoPhoneNumber(_wengoPhone->text().toStdString());
	////

	// Setting emails
	c.setPersonalEmail(_email->text().toStdString());
	////

	// Settings websites
	c.setWebsite(_web->text().toStdString());
	////
}

void QtEditContactProfile::readFromConfig() {
	Contact & c = _cContact.getContact();

	// Setting wengo id
	_alias->setText(QString::fromStdString(c.getWengoPhoneId()));
	////

	// Setting name
	_firstName->setText(QString::fromStdString(c.getFirstName()));
	_lastName->setText(QString::fromStdString(c.getLastName()));
	////

	// Setting sex
	_gender->setCurrentIndex((int) c.getSex());
	////

	// Setting birthday
	Date date = c.getBirthdate();
	_birthDate->setDate(QDate(date.getYear(), date.getMonth(), date.getDay()));
	////

	// Setting address
	StreetAddress address = c.getStreetAddress();
	//_state->setText(QString::fromStdString(address.getStateProvince()));
	//_country->setText(QString::fromStdString(address.getCountry()));
	_city->setText(QString::fromStdString(address.getCity()));
	////

	// Setting IMAccounts
	////

	// Setting phone numbers
	_cellPhone->setText(QString::fromStdString(c.getMobilePhone()));
	_workPhone->setText(QString::fromStdString(c.getWorkPhone()));
	_homePhone->setText(QString::fromStdString(c.getHomePhone()));
	_wengoPhone->setText(QString::fromStdString(c.getWengoPhoneNumber()));
	////

	// Setting emails
	_email->setText(QString::fromStdString(c.getPersonalEmail()));
	////

	// Settings websites
	_web->setText(QString::fromStdString(c.getWebsite()));
	////
}

void QtEditContactProfile::init() {
	_alias = Object::findChild<QLineEdit *>( _widget, "alias" );
	_firstName = Object::findChild<QLineEdit *>( _widget, "firstName" );
	_lastName = Object::findChild<QLineEdit *>( _widget, "lastName" );
	_birthDate = Object::findChild<QDateEdit *>( _widget, "birthDate" );
	_gender = Object::findChild<QComboBox *>( _widget, "gender" );

	_country = Object::findChild<QComboBox *>( _widget, "country" );
	_state = Object::findChild<QComboBox *>( _widget, "state" );
	_city = Object::findChild<QLineEdit *>( _widget, "city" );

	_cellPhone = Object::findChild<QLineEdit *>( _widget, "cellPhone" );
	_wengoPhone = Object::findChild<QLineEdit *>( _widget, "wengoPhone" );
	_homePhone = Object::findChild<QLineEdit *>( _widget, "homePhone" );
	_workPhone = Object::findChild<QLineEdit *>( _widget, "workPhone" );

	// Advanced details
	_email = Object::findChild<QLineEdit *>( _widget, "email" );
	_blog = Object::findChild<QLineEdit *>( _widget, "blog" );
	_web = Object::findChild<QLineEdit *>( _widget, "web" );

	_saveChange = Object::findChild<QPushButton *>( _widget, "saveChange" );
	_cancelChange = Object::findChild<QPushButton *>( _widget, "cancelChange" );

	// IM accounts pics
	QLabel * imAccountsPic;
	imAccountsPic = Object::findChild<QLabel *>( _widget, "imPic1" );
	_imAccountsPic << imAccountsPic;
	imAccountsPic = Object::findChild<QLabel *>( _widget, "imPic2" );
	_imAccountsPic << imAccountsPic;
	imAccountsPic = Object::findChild<QLabel *>( _widget, "imPic3" );
	_imAccountsPic << imAccountsPic;
	imAccountsPic = Object::findChild<QLabel *>( _widget, "imPic4" );
	_imAccountsPic << imAccountsPic;
	imAccountsPic = Object::findChild<QLabel *>( _widget, "imPic5" );
	_imAccountsPic << imAccountsPic;
	imAccountsPic = Object::findChild<QLabel *>( _widget, "imPic6" );
	_imAccountsPic << imAccountsPic;
	// IM accounts data
	QLineEdit * imAccountData;
	imAccountData = Object::findChild<QLineEdit *>( _widget, "imAccount1" );
	_imAccountLineEdit << imAccountData;
	imAccountData = Object::findChild<QLineEdit *>( _widget, "imAccount2" );
	_imAccountLineEdit << imAccountData;
	imAccountData = Object::findChild<QLineEdit *>( _widget, "imAccount3" );
	_imAccountLineEdit << imAccountData;
	imAccountData = Object::findChild<QLineEdit *>( _widget, "imAccount4" );
	_imAccountLineEdit << imAccountData;
	imAccountData = Object::findChild<QLineEdit *>( _widget, "imAccount5" );
	_imAccountLineEdit << imAccountData;
	imAccountData = Object::findChild<QLineEdit *>( _widget, "imAccount6" );
	_imAccountLineEdit << imAccountData;

	_avatar = Object::findChild<QLabel *>( _widget, "avatar" );
}

void QtEditContactProfile::changeGroupBoxStat( QGroupBox * box, bool stat ) {
	QList<QWidget *> allWidgets = box->findChildren<QWidget *>();

	for ( int i = 0;i < allWidgets.size();i++ ) {
		allWidgets[ i ] ->setEnabled( stat );
	}
}

