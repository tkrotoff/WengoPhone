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

#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/contactlist/ContactProfile.h>
#include <model/profile/UserProfile.h>

#include <qtutil/StringListConvert.h>

#include <util/Logger.h>
#include <cutil/global.h>

#include <QtGui>

#ifdef OS_WINDOWS
	#include <windows.h>
#endif

static const char * PNG_FORMAT = "PNG";

QtProfileDetails::QtProfileDetails(CUserProfile & cUserProfile, ContactProfile & contactProfile, QWidget * parent)
	: QObject(parent),
	_cUserProfile(cUserProfile),
	_profile(contactProfile) {

	init(parent);

	//FIXME we should keep in memory the UUID of the group
	std::vector< std::pair<std::string, std::string> > tmp = _cUserProfile.getCContactList().getContactGroups();
	for (std::vector< std::pair<std::string, std::string> >::const_iterator it = tmp.begin();
		it != tmp.end();
		++it) {

		_ui->groupComboBox->addItem(QString::fromUtf8((*it).second.c_str()), QString::fromStdString((*it).first.c_str()));
		std::string str1 = (*it).first;
		std::string str2 = contactProfile.getGroupId();
		if (str1 == str2) {
			_ui->groupComboBox->setCurrentIndex(_ui->groupComboBox->findText(QString::fromUtf8((*it).second.c_str())));
		}
	}

	_profileDetailsWindow->setWindowTitle(tr("WengoPhone - Contact details"));

	QtIMContactManager * qtIMContactManager = new QtIMContactManager(contactProfile,
		_cUserProfile, _profileDetailsWindow);

	int index = _ui->imStackedWidget->addWidget(qtIMContactManager->getWidget());
	_ui->imStackedWidget->setCurrentIndex(index);

	connect(_ui->saveButton, SIGNAL(clicked()), SLOT(saveContact()));
}

QtProfileDetails::QtProfileDetails(CUserProfile & cUserProfile, UserProfile & userProfile, QWidget * parent)
	: QObject(parent),
	_cUserProfile(cUserProfile),
	_profile(userProfile) {

	init(parent);
	_profileDetailsWindow->setWindowTitle(tr("WengoPhone - My profile details"));

	//Not needed for UserProfile
	_ui->groupLabel->hide();
	_ui->groupComboBox->hide();

	QtIMAccountManager * qtIMAccountManager =
		new QtIMAccountManager(userProfile, false, _profileDetailsWindow);
	int index = _ui->imStackedWidget->addWidget(qtIMAccountManager->getWidget());
	_ui->imStackedWidget->setCurrentIndex(index);

	connect(_ui->saveButton, SIGNAL(clicked()), SLOT(saveUserProfile()));
	connect(_ui->avatarPixmapButton, SIGNAL(clicked()), SLOT(changeUserProfileAvatar()));
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
	_ui->firstNameLineEdit->setText(QString::fromUtf8(_profile.getFirstName().c_str()));
	_ui->lastNameLineEdit->setText(QString::fromUtf8(_profile.getLastName().c_str()));
	_ui->genderComboBox->setCurrentIndex((int) _profile.getSex());

	Date date = _profile.getBirthdate();
	_ui->birthDate->setDate(QDate(date.getYear(), date.getMonth(), date.getDay()));

	StreetAddress address = _profile.getStreetAddress();
	_ui->cityLineEdit->setText(QString::fromUtf8(address.getCity().c_str()));
	_ui->stateLineEdit->setText(QString::fromUtf8(address.getStateProvince().c_str()));

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
	_profile.setFirstName(_ui->firstNameLineEdit->text().toUtf8().data());

	_profile.setLastName(_ui->lastNameLineEdit->text().toUtf8().data());

	QDate date = _ui->birthDate->date();
	_profile.setBirthdate(Date(date.day(), date.month(), date.year()));

	_profile.setSex((EnumSex::Sex) _ui->genderComboBox->currentIndex());

	StreetAddress address;
	//address.setCountry(_ui->countryComboBox->currentText().toStdString());
	address.setStateProvince(_ui->stateLineEdit->text().toUtf8().data());

	address.setCity(_ui->cityLineEdit->text().toUtf8().data());

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

	ContactProfile & contactProfile = dynamic_cast<ContactProfile &>(_profile);

	if (_ui->groupComboBox->currentText().isEmpty()) {
		QMessageBox::warning(_profileDetailsWindow,
			tr("WengoPhone -- No group selected"),
			tr("A contact must have a group, please set a group."),
			QMessageBox::NoButton,
			QMessageBox::NoButton,
			QMessageBox::Ok);

		return;
	}

	int index = _ui->groupComboBox->findText(_ui->groupComboBox->currentText());
	QVariant groupId;
	groupId = _ui->groupComboBox->itemData(index);
	//If the group does not exist
	if (groupId.isNull()) {
		std::string groupName = std::string(_ui->groupComboBox->currentText().toUtf8().data());
		_cUserProfile.getCContactList().addContactGroup(groupName);
		groupId = QString::fromStdString(_cUserProfile.getCContactList().getContactGroupIdFromName(groupName));
	}

	contactProfile.setGroupId(groupId.toString().toStdString());

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
#ifdef OS_WINDOWS
	BringWindowToTop(_profileDetailsWindow->winId());
#endif
}
