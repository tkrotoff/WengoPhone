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

#include "QtProfileDetails.h"

#include "ui_ProfileDetails.h"

#include <presentation/qt/imcontact/QtIMContactManager.h>
#include <presentation/qt/imaccount/QtIMAccountManager.h>

#include <control/CWengoPhone.h>
#include <control/contactlist/CContactList.h>

#include <model/profile/UserProfile.h>
#include <model/contactlist/Contact.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <qtutil/StringListConvert.h>

#include <QtGui>

static const char * PNG_FORMAT = "PNG";

QtProfileDetails::QtProfileDetails(CWengoPhone & cWengoPhone, Contact & contact, QWidget * parent)
	: QObject(parent),
	_cWengoPhone(cWengoPhone),
	_profile(contact) {

	init(parent);
	QStringList tmp = StringListConvert::toQStringList(_cWengoPhone.getCContactList().getContactGroupStringList());
	_ui->groupComboBox->addItems(tmp);

	_profileDetailsWindow->setWindowTitle(tr("WengoPhone - Contact details"));

	QtIMContactManager * qtIMContactManager = new QtIMContactManager(contact, _profileDetailsWindow);
	int index = _ui->imStackedWidget->addWidget(qtIMContactManager->getWidget());
	_ui->imStackedWidget->setCurrentIndex(index);

	connect(_ui->saveButton, SIGNAL(clicked()), SLOT(saveContact()));

	show();
}

QtProfileDetails::QtProfileDetails(CWengoPhone & cWengoPhone, UserProfile & userProfile, QWidget * parent)
	: QObject(parent),
	_cWengoPhone(cWengoPhone),
	_profile(userProfile) {

	init(parent);
	_profileDetailsWindow->setWindowTitle(tr("WengoPhone - My profile details"));

	//Not needed for UserProfile
	_ui->groupLabel->hide();
	_ui->groupComboBox->hide();

	QtIMAccountManager * qtIMAccountManager = new QtIMAccountManager(userProfile, _cWengoPhone,
						false, _profileDetailsWindow);
	int index = _ui->imStackedWidget->addWidget(qtIMAccountManager->getWidget());
	_ui->imStackedWidget->setCurrentIndex(index);

	connect(_ui->saveButton, SIGNAL(clicked()), SLOT(saveUserProfile()));
	connect(_ui->avatarPixmapButton, SIGNAL(clicked()), SLOT(changeUserProfileAvatar()));

	show();
}

int QtProfileDetails::show() {
	return _profileDetailsWindow->exec();
}

void QtProfileDetails::init(QWidget * parent) {
	_profileDetailsWindow = new QDialog(parent);

	_ui = new Ui::ProfileDetails();
	_ui->setupUi(_profileDetailsWindow);

	connect(_ui->cancelButton, SIGNAL(clicked()), SLOT(cancelButtonClicked()));

	readProfile();
}

void QtProfileDetails::readProfile() {
	_ui->firstNameLineEdit->setText(QString::fromStdString(_profile.getFirstName()));
	_ui->lastNameLineEdit->setText(QString::fromStdString(_profile.getLastName()));
	//_wengoNickName->setText(QString::fromStdString(_profile.getWengoPhoneId()));
	_ui->genderComboBox->setCurrentIndex((int) _profile.getSex());

	Date date = _profile.getBirthdate();
	_ui->birthDate->setDate(QDate(date.getYear(), date.getMonth(), date.getDay()));

	StreetAddress address = _profile.getStreetAddress();
	_ui->cityLineEdit->setText(QString::fromStdString(address.getCity()));
	//_ui->countryComboBox->setCurrentIndex(_ui->countryComboBox->findText(QString::fromStdString(address.getCountry())));
	_ui->stateLineEdit->setText(QString::fromStdString(address.getStateProvince()));

	_ui->mobilePhoneLineEdit->setText(QString::fromStdString(_profile.getMobilePhone()));
	_ui->homePhoneLineEdit->setText(QString::fromStdString(_profile.getHomePhone()));
	_ui->wengoPhoneLineEdit->setText(QString::fromStdString(_profile.getWengoPhoneNumber()));
	_ui->workPhoneLineEdit->setText(QString::fromStdString(_profile.getWorkPhone()));

	_ui->emailLineEdit->setText(QString::fromStdString(_profile.getPersonalEmail()));
	_ui->webLineEdit->setText(QString::fromStdString(_profile.getWebsite()));

	readProfileAvatar();
}

void QtProfileDetails::readProfileAvatar() {
	std::string data = _profile.getIcon().getData();
	if (!data.empty()) {
		QPixmap pixmap;
		pixmap.loadFromData((uchar *) data.c_str(), data.size());
		_ui->avatarPixmapButton->setIcon(pixmap.scaled(_ui->avatarPixmapButton->rect().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
		_ui->avatarPixmapButton->setIconSize(_ui->avatarPixmapButton->rect().size());
	}
}

void QtProfileDetails::saveProfile() {
	_profile.setFirstName(_ui->firstNameLineEdit->text().toStdString());
	_profile.setLastName(_ui->lastNameLineEdit->text().toStdString());
	//_profile.setWengoPhoneId(_wengoNickName->text().toStdString());

	QDate date = _ui->birthDate->date();
	_profile.setBirthdate(Date(date.day(), date.month(), date.year()));

	_profile.setSex((EnumSex::Sex) _ui->genderComboBox->currentIndex());

	StreetAddress address;
	//address.setCountry(_ui->countryComboBox->currentText().toStdString());
	address.setStateProvince(_ui->stateLineEdit->text().toStdString());
	address.setCity(_ui->cityLineEdit->text().toStdString());
	_profile.setStreetAddress(address);

	_profile.setMobilePhone(_ui->mobilePhoneLineEdit->text().toStdString());
	_profile.setHomePhone(_ui->homePhoneLineEdit->text().toStdString());
	_profile.setWengoPhoneNumber(_ui->wengoPhoneLineEdit->text().toStdString());
	_profile.setWorkPhone(_ui->workPhoneLineEdit->text().toStdString());

	_profile.setPersonalEmail(_ui->emailLineEdit->text().toStdString());
	_profile.setWebsite(_ui->webLineEdit->text().toStdString());
}

void QtProfileDetails::saveContact() {
	saveProfile();
	Contact & contact = dynamic_cast<Contact &>(_profile);
	contact.addToContactGroup(_ui->groupComboBox->currentText().toStdString());

	_profileDetailsWindow->accept();
}

void QtProfileDetails::saveUserProfile() {
	saveProfile();
	UserProfile & userProfile = dynamic_cast<UserProfile &>(_profile);

	_profileDetailsWindow->accept();
}

void QtProfileDetails::cancelButtonClicked() {
	_profileDetailsWindow->reject();
}

void QtProfileDetails::changeUserProfileAvatar() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	QString file = QFileDialog::getOpenFileName(_profileDetailsWindow,
				tr("Choose an avatar"),
				QString::fromStdString(config.getResourcesDir()) + "pics/avatars",
				tr("Images") + "(*.png *.xpm *.jpg *.gif)");

	if (!file.isNull()) {
		//TODO check the size of the file

		//QImage is optimised for I/O manipulations
		QImage image(file);
		image.scaled(_ui->avatarPixmapButton->rect().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

		QBuffer buffer;
		buffer.open(QIODevice::ReadWrite);
		image.save(&buffer, PNG_FORMAT);
		buffer.close();

		QByteArray byteArray = buffer.data();
		std::string data(byteArray.data(), byteArray.size());

		Picture picture = Picture::pictureFromData(data);
		picture.setFilename(file.toStdString());

		UserProfile & userProfile = dynamic_cast<UserProfile &>(_profile);
		userProfile.setIcon(picture, NULL);

		readProfileAvatar();
	}
}