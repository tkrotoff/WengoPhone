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

#include <model/profile/UserProfile.h>
#include <model/WengoPhone.h>

#include <qtutil/Object.h>
#include <qtutil/WidgetFactory.h>

#include <util/Date.h>
#include <util/Logger.h>

#include "QtEditMyProfile.h"
#include "QtProtocolSettings.h"

QtEditMyProfile::QtEditMyProfile(WengoPhone & wengoPhone, QWidget * parent, Qt::WFlags f )
: QDialog( parent, f ), _wengoPhone(wengoPhone) {

	_widget = qobject_cast<QWidget *>( WidgetFactory::create( ":/forms/login/profileWindow.ui", this ) );
	layout = new QGridLayout( this );
	layout->addWidget( _widget );

	resize( QSize( 619, 572 ) );
	init();
	hideAccountWidgets();
	readFromConfig();
	connect( _addIMAccount, SIGNAL( clicked() ), this, SLOT( imAccountAdded() ) );
	connect( _saveChange, SIGNAL( clicked() ), this, SLOT( saveClicked() ) );
	connect( _cancelChange, SIGNAL( clicked() ), this, SLOT( cancelClicked() ) );
}

void QtEditMyProfile::saveClicked() {
	writeToConfig();
	accept();
}

void QtEditMyProfile::cancelClicked() {
	reject();
}

// FIXME : Just a test
void QtEditMyProfile::imAccountAdded() {

	for ( int i = 0; i < _imAccountLineEdit.size(); i++ ) {
			if ( _imAccountLineEdit[ i ] ->isVisible() == false ) {
					_imAccountLineEdit[ i ] ->setVisible( true );
					break;
				}
		}

	for ( int i = 0; i < _imAccountsPic.size(); i++ ) {
			if ( _imAccountsPic[ i ] ->isVisible() == false ) {
					_imAccountsPic[ i ] ->setVisible( false );
					break;
				}
		}

}

void QtEditMyProfile::hideAccountWidgets() {
	for ( int i = 0; i < _imAccountLineEdit.size(); i++ ) {
			_imAccountLineEdit[ i ] ->setVisible( false );
			_imAccountLineEdit[ i ] ->setReadOnly(true);
		}

	for ( int i = 0; i < _imAccountsPic.size(); i++ ) {
			_imAccountsPic[ i ] ->setVisible( false );
		}

}

void QtEditMyProfile::writeToConfig() {
	UserProfile & userProfile = _wengoPhone.getCurrentUserProfile();

	// Setting name and wengo id
	userProfile.setWengoPhoneId(_wengoNickName->text().toStdString());
	userProfile.setFirstName(_firstName->text().toStdString());
	userProfile.setLastName(_lastName->text().toStdString());
	////

	// Setting birthday
	QDate date = _birthDate->date();
	userProfile.setBirthdate(Date(date.day(), date.month(), date.year()));
	////

	// Setting sex
	userProfile.setSex((EnumSex::Sex) _gender->currentIndex());
	////

	// Setting address
	StreetAddress streetAddress;
	//streetAddress.setCountry(_country->currentText().toStdString());
	streetAddress.setCity(_city->text().toStdString());
	//streetAdress.setStateProvince(_state->currentText().toStdString());
	userProfile.setStreetAddress(streetAddress);
	////

	// Setting IMAccount
	/*
	config.set( config.PROFILE_IMEMAIL1, _imAccountLineEdit[ 0 ] ->text().toStdString() );
	config.set( config.PROFILE_IMEMAIL2, _imAccountLineEdit[ 1 ] ->text().toStdString() );
	config.set( config.PROFILE_IMEMAIL3, _imAccountLineEdit[ 2 ] ->text().toStdString() );
	config.set( config.PROFILE_IMEMAIL4, _imAccountLineEdit[ 3 ] ->text().toStdString() );
	config.set( config.PROFILE_IMEMAIL5, _imAccountLineEdit[ 4 ] ->text().toStdString() );
	config.set( config.PROFILE_IMEMAIL6, _imAccountLineEdit[ 5 ] ->text().toStdString() );

	config.set( config.PROFILE_IMPIC1, _imAccountsPicPath[ 0 ].toStdString() );
	config.set( config.PROFILE_IMPIC2, _imAccountsPicPath[ 1 ].toStdString() );
	config.set( config.PROFILE_IMPIC3, _imAccountsPicPath[ 2 ].toStdString() );
	config.set( config.PROFILE_IMPIC4, _imAccountsPicPath[ 3 ].toStdString() );
	config.set( config.PROFILE_IMPIC5, _imAccountsPicPath[ 4 ].toStdString() );
	config.set( config.PROFILE_IMPIC6, _imAccountsPicPath[ 5 ].toStdString() );
	*/
	////

	// Setting Phone numbers
	userProfile.setMobilePhone(_cellphone->text().toStdString());
	userProfile.setHomePhone(_homePhone->text().toStdString());
	userProfile.setWengoPhoneNumber(_wengoPhoneNumber->text().toStdString());
	userProfile.setWorkPhone(_workPhone->text().toStdString());
	////

	// Setting emails
	userProfile.setPersonalEmail(_email->text().toStdString());
	////

	// Setting websites
	userProfile.setWebsite(_web->text().toStdString());
	//config.set( config.PROFILE_BLOG, _blog->text().toStdString() );
	////

	// Setting avatar
	//config.set( config.PROFILE_AVATAR, _avatarPath.toStdString() );
	////
}

void QtEditMyProfile::readFromConfig() {
	UserProfile & userProfile = _wengoPhone.getCurrentUserProfile();

	// Setting wengo id
	_wengoNickName->setText(QString::fromStdString(userProfile.getWengoPhoneId()));
	////

	// Setting name
	_firstName->setText(QString::fromStdString(userProfile.getFirstName()));
	_lastName->setText(QString::fromStdString(userProfile.getLastName()));
	_gender->setCurrentIndex((int) userProfile.getSex());
	////

	// Setting birthday
	//_birthDate->setDate(QDate::fromString( QString().fromStdString( config.getProfileBirthDate() ) ) );
	////

	// Setting address
	StreetAddress address = userProfile.getStreetAddress();
	_city->setText(QString::fromStdString(address.getCity()));
	//_country->setCurrentIndex(_country->findText( QString().fromStdString( config.getProfileCountry() ) ) );
	//_state->setCurrentIndex( _state->findText( QString().fromStdString( config.getProfileState() ) ) );
	////

	// Setting IMAccounts
	/*
	_imAccountLineEdit[ 0 ] ->setText( QString().fromStdString( config.getProfileIMEmail1() ) );
	_imAccountLineEdit[ 1 ] ->setText( QString().fromStdString( config.getProfileIMEmail2() ) );
	_imAccountLineEdit[ 2 ] ->setText( QString().fromStdString( config.getProfileIMEmail3() ) );
	_imAccountLineEdit[ 3 ] ->setText( QString().fromStdString( config.getProfileIMEmail4() ) );
	_imAccountLineEdit[ 4 ] ->setText( QString().fromStdString( config.getProfileIMEmail5() ) );
	_imAccountLineEdit[ 5 ] ->setText( QString().fromStdString( config.getProfileIMEmail6() ) );

	_imAccountsPic[ 0 ] ->setPixmap( QPixmap( QString().fromStdString( config.getProfileIMPic1() ) ) );
	_imAccountsPicPath << QString().fromStdString( config.getProfileIMPic1() );
	_imAccountsPic[ 1 ] ->setPixmap( QPixmap( QString().fromStdString( config.getProfileIMPic2() ) ) );
	_imAccountsPicPath << QString().fromStdString( config.getProfileIMPic2() );
	_imAccountsPic[ 2 ] ->setPixmap( QPixmap( QString().fromStdString( config.getProfileIMPic3() ) ) );
	_imAccountsPicPath << QString().fromStdString( config.getProfileIMPic3() );
	_imAccountsPic[ 3 ] ->setPixmap( QPixmap( QString().fromStdString( config.getProfileIMPic4() ) ) );
	_imAccountsPicPath << QString().fromStdString( config.getProfileIMPic4() );
	_imAccountsPic[ 4 ] ->setPixmap( QPixmap( QString().fromStdString( config.getProfileIMPic5() ) ) );
	_imAccountsPicPath << QString().fromStdString( config.getProfileIMPic5() );
	_imAccountsPic[ 5 ] ->setPixmap( QPixmap( QString().fromStdString( config.getProfileIMPic6() ) ) );
	_imAccountsPicPath << QString().fromStdString( config.getProfileIMPic6() );
	*/
	////

	// Setting phone numbers
	_cellphone->setText(QString::fromStdString(userProfile.getMobilePhone()));
	_homePhone->setText(QString::fromStdString(userProfile.getHomePhone()));
	_wengoPhoneNumber->setText(QString::fromStdString(userProfile.getWengoPhoneNumber()));
	_workPhone->setText(QString::fromStdString(userProfile.getWorkPhone()));
	////

	// Setting email
	_email->setText(QString::fromStdString(userProfile.getPersonalEmail()));
	////

	// Setting websites
	//_blog->setText( QString().fromStdString( config.getProfileBlog() ) );
	_web->setText(QString::fromStdString(userProfile.getWebsite()));
	////

	// Setting avatar
	//_avatarPath = QString().fromStdString( config.getProfileAvatar() );
	//_avatar->setPixmap( QPixmap( _avatarPath ) );
	////
}

void QtEditMyProfile::init() {
	_wengoNickName = Object::findChild<QLineEdit *>( _widget, "wengoNickName" );
	_changePassword = Object::findChild<QPushButton *>( _widget, "changePassword" );
	_firstName = Object::findChild<QLineEdit *>( _widget, "firstName" );
	_lastName = Object::findChild<QLineEdit *>( _widget, "lastName" );
	_birthDate = Object::findChild<QDateEdit *>( _widget, "birthDate" );
	_city = Object::findChild<QLineEdit *>( _widget, "city" );

	_gender = Object::findChild<QComboBox *>( _widget, "gender" );
	_country = Object::findChild<QComboBox *>( _widget, "country" );
	_state = Object::findChild<QComboBox *>( _widget, "state" );

	_cellphone = Object::findChild<QLineEdit *>( _widget, "cellPhone" );
	_wengoPhoneNumber = Object::findChild<QLineEdit *>( _widget, "wengoPhone" );
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

	_addIMAccount = Object::findChild<QPushButton *>( _widget, "addIMAccount" );
	_modifyIMAccount = Object::findChild<QPushButton *>( _widget, "modityIMAccount" );

	_avatar = Object::findChild<QLabel *>( _widget, "avatar" );
}

void QtEditMyProfile::changeGroupBoxStat( QGroupBox * box, bool stat ) {
	QList<QWidget *> allWidgets = box->findChildren<QWidget *>();

	for ( int i = 0;i < allWidgets.size();i++ ) {
			allWidgets[ i ] ->setEnabled( stat );
		}
}

