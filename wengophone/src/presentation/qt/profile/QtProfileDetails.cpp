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
#include <presentation/qt/imcontact/QtSimpleIMContactManager.h>
#include <presentation/qt/imaccount/QtIMAccountManager.h>
#include <presentation/qt/imaccount/QtSimpleIMAccountManager.h>

#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/contactlist/ContactProfile.h>
#include <model/profile/UserProfile.h>

#include <qtutil/PixmapMerging.h>
#include <qtutil/StringListConvert.h>

#include <util/CountryList.h>
#include <util/Logger.h>
#include <util/WebBrowser.h>
#include <util/SafeDelete.h>
#include <cutil/global.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

#ifdef OS_WINDOWS
	#include <windows.h>
#endif

static const char * PNG_FORMAT = "PNG";

QtProfileDetails::QtProfileDetails(CUserProfile & cUserProfile,
	ContactProfile & contactProfile, QWidget * parent, const QString & windowTitle)
	: QObject(parent),
	_cUserProfile(cUserProfile),
	_profile(contactProfile) {

	init(parent);
	_showContact = true;
	_profileDetailsWindow->setWindowTitle(tr("WengoPhone") + " - " + windowTitle);

	//FIXME we should keep in memory the UUID of the group
	std::vector< std::pair<std::string, std::string> > tmp = _cUserProfile.getCContactList().getContactGroups();
	for (std::vector< std::pair<std::string, std::string> >::const_iterator it = tmp.begin();
		it != tmp.end(); ++it) {

		_ui->groupComboBox->addItem(QString::fromUtf8((*it).second.c_str()), QString::fromStdString((*it).first.c_str()));
		std::string str1 = (*it).first;
		std::string str2 = contactProfile.getGroupId();
		if (str1 == str2) {
			_ui->groupComboBox->setCurrentIndex(_ui->groupComboBox->findText(QString::fromUtf8((*it).second.c_str())));
		}
	}

	//QtSimpleIMContactManager
	_qtIMContactManager = new QtSimpleIMContactManager(contactProfile, _cUserProfile, _profileDetailsWindow);
	int index = _ui->imStackedWidget->addWidget(_qtIMContactManager->getWidget());
	_ui->imStackedWidget->setCurrentIndex(index);

	//saveButton
	SAFE_CONNECT(_ui->saveButton, SIGNAL(clicked()), SLOT(saveContact()));

	//avatarPixmapButton
	_ui->avatarPixmapButton->setToolTip("");
	_ui->avatarPixmapButton->setEnabled(false);
	_ui->myProfileLabel->hide();
}

QtProfileDetails::QtProfileDetails(CUserProfile & cUserProfile,
	UserProfile & userProfile, QWidget * parent, const QString & windowTitle)
	: QObject(parent),
	_cUserProfile(cUserProfile),
	_profile(userProfile) {

	init(parent);
	_showContact = false;
	_profileDetailsWindow->setWindowTitle(tr("WengoPhone") + " - " + windowTitle);

	//Not needed for UserProfile
	_ui->groupLabel->hide();
	_ui->groupComboBox->hide();

	//QtSimpleIMAccountManager
	//FIXME no more simple mode
	/*_qtIMAccountManager = new QtSimpleIMAccountManager(userProfile, _profileDetailsWindow);
	int index = _ui->imStackedWidget->addWidget(_qtIMAccountManager->getWidget());
	_ui->imStackedWidget->setCurrentIndex(index);*/

	//QtIMAccountManager
	QtIMAccountManager * qtIMAccountManager =
		new QtIMAccountManager((UserProfile &) _profile, false, _profileDetailsWindow);
	int index = _ui->imStackedWidget->addWidget(qtIMAccountManager->getWidget());
	_ui->imStackedWidget->setCurrentIndex(index);
	_ui->advancedButton->hide();
	/// no more simple mode

	//saveButton
	SAFE_CONNECT(_ui->saveButton, SIGNAL(clicked()), SLOT(saveUserProfile()));

	//avatarPixmapButton
	SAFE_CONNECT(_ui->avatarPixmapButton, SIGNAL(clicked()), SLOT(changeUserProfileAvatar()));
}

void QtProfileDetails::init(QWidget * parent) {
	_profileDetailsWindow = new QDialog(parent);

	_ui = new Ui::ProfileDetails();
	_ui->setupUi(_profileDetailsWindow);

	fullCountryList();

	SAFE_CONNECT(_ui->cancelButton, SIGNAL(clicked()), SLOT(cancelButtonClicked()));
	SAFE_CONNECT(_ui->advancedButton, SIGNAL(clicked()), SLOT(advancedButtonClicked()));
	SAFE_CONNECT(_ui->websiteButton, SIGNAL(clicked()), SLOT(websiteButtonClicked()));
	SAFE_CONNECT(_ui->emailButton, SIGNAL(clicked()), SLOT(emailButtonClicked()));

	_qtIMContactManager = NULL;
	_qtIMAccountManager = NULL;

	readProfile();
}

void QtProfileDetails::fullCountryList() {
	_ui->countryComboBox->addItems(StringListConvert::toQStringList(CountryList::getCountryList()));
}

int QtProfileDetails::show() {
	return _profileDetailsWindow->exec();
}

void QtProfileDetails::readProfile() {
	_ui->firstNameLineEdit->setText(QString::fromUtf8(_profile.getFirstName().c_str()));
	_ui->lastNameLineEdit->setText(QString::fromUtf8(_profile.getLastName().c_str()));
	_ui->genderComboBox->setCurrentIndex((int) _profile.getSex());

	Date date = _profile.getBirthdate();
	_ui->birthDate->setDate(QDate(date.getYear(), date.getMonth(), date.getDay()));

	StreetAddress address = _profile.getStreetAddress();
	_ui->countryComboBox->setCurrentIndex(_ui->countryComboBox->findText(QString::fromUtf8(address.getCountry().c_str())));
	_ui->cityLineEdit->setText(QString::fromUtf8(address.getCity().c_str()));
	_ui->stateLineEdit->setText(QString::fromUtf8(address.getStateProvince().c_str()));

	_ui->mobilePhoneLineEdit->setText(QString::fromStdString(_profile.getMobilePhone()));
	_ui->homePhoneLineEdit->setText(QString::fromStdString(_profile.getHomePhone()));

	_ui->emailLineEdit->setText(QString::fromStdString(_profile.getPersonalEmail()));
	_ui->webLineEdit->setText(QString::fromStdString(_profile.getWebsite()));

	_ui->notesEdit->setPlainText(QString::fromUtf8(_profile.getNotes().c_str()));

	readProfileAvatar();
}

void QtProfileDetails::readProfileAvatar() {
	std::string backgroundPixmapFilename = ":/pics/avatar_background.png";
	std::string foregroundPixmapData = _profile.getIcon().getData();

	_ui->avatarPixmapButton->setIcon(PixmapMerging::merge(foregroundPixmapData, backgroundPixmapFilename));
}

void QtProfileDetails::saveProfile() {
	_profile.setFirstName(_ui->firstNameLineEdit->text().toUtf8().data());

	_profile.setLastName(_ui->lastNameLineEdit->text().toUtf8().data());

	QDate date = _ui->birthDate->date();
	_profile.setBirthdate(Date(date.day(), date.month(), date.year()));

	_profile.setSex((EnumSex::Sex) _ui->genderComboBox->currentIndex());

	StreetAddress address;
	address.setCountry(_ui->countryComboBox->currentText().toUtf8().data());
	address.setStateProvince(_ui->stateLineEdit->text().toUtf8().data());
	address.setCity(_ui->cityLineEdit->text().toUtf8().data());

	_profile.setStreetAddress(address);

	_profile.setMobilePhone(_ui->mobilePhoneLineEdit->text().toStdString());
	_profile.setHomePhone(_ui->homePhoneLineEdit->text().toStdString());

	_profile.setPersonalEmail(_ui->emailLineEdit->text().toStdString());
	_profile.setWebsite(_ui->webLineEdit->text().toStdString());

	_profile.setNotes(_ui->notesEdit->toPlainText().toUtf8().constData());
}

void QtProfileDetails::saveContact() {
	if (_qtIMAccountManager) {
		_qtIMAccountManager->saveIMAccounts();
	} else if (_qtIMContactManager) {
		_qtIMContactManager->saveIMContacts();
	}

	saveProfile();

	ContactProfile & contactProfile = (ContactProfile &) _profile;

	if (_ui->groupComboBox->currentText().isEmpty()) {
		QMessageBox::warning(_profileDetailsWindow,
			tr("WengoPhone - No Group Selected"),
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

	if (!file.isEmpty()) {
		UserProfile & userProfile = dynamic_cast<UserProfile &>(_profile);
		setProfileAvatarFileName(userProfile, file);
		readProfileAvatar();
	}

#ifdef OS_WINDOWS
	BringWindowToTop(_profileDetailsWindow->winId());
#endif
}

void QtProfileDetails::setProfileAvatarFileName(UserProfile & userProfile, const QString & fileName) {
	if (!fileName.isEmpty()) {
		//TODO check the size of the file

		//QImage is optimised for I/O manipulations
		QImage image(fileName);
		//Size of Wengo avatars
		QSize size(96, 96);
		//Image is scaled to not save a big picture in userprofile.xml
		image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		QBuffer buffer;
		buffer.open(QIODevice::ReadWrite);
		image.save(&buffer, PNG_FORMAT);
		buffer.close();

		QByteArray byteArray = buffer.data();
		std::string data(byteArray.data(), byteArray.size());

		OWPicture picture = OWPicture::pictureFromData(data);
		picture.setFilename(fileName.toStdString());

		userProfile.setIcon(picture, NULL);
	}
}

void QtProfileDetails::advancedButtonClicked() {
	bool advancedMode;

	//IMContact
	if (_showContact) {
		if (_qtIMContactManager) {
			//Simple mode -> advanced mode
			advancedMode = true;

			//QtIMContactManager
			QtIMContactManager * qtIMContactManager =
					new QtIMContactManager((ContactProfile &) _profile, _cUserProfile, _profileDetailsWindow);

			_ui->imStackedWidget->removeWidget(_qtIMContactManager->getWidget());
			int index = _ui->imStackedWidget->addWidget(qtIMContactManager->getWidget());
			_ui->imStackedWidget->setCurrentIndex(index);

			OWSAFE_DELETE(_qtIMContactManager);
		} else {
			//Advanced mode -> simple mode
			advancedMode = false;

			//QtSimpleIMContactManager
			_qtIMContactManager = new QtSimpleIMContactManager((ContactProfile &) _profile, _cUserProfile, _profileDetailsWindow);

			int index = _ui->imStackedWidget->addWidget(_qtIMContactManager->getWidget());
			_ui->imStackedWidget->setCurrentIndex(index);
		}
	}

	//IMAccount
	else {
		if (_qtIMAccountManager) {
			//Simple mode -> advanced mode
			advancedMode = true;

			//QtIMAccountManager
			QtIMAccountManager * qtIMAccountManager =
					new QtIMAccountManager((UserProfile &) _profile, false, _profileDetailsWindow);

			_ui->imStackedWidget->removeWidget(_qtIMAccountManager->getWidget());
			int index = _ui->imStackedWidget->addWidget(qtIMAccountManager->getWidget());
			_ui->imStackedWidget->setCurrentIndex(index);

			OWSAFE_DELETE(_qtIMAccountManager);
		} else {
			//Advanced mode -> simple mode
			advancedMode = false;

			//QtSimpleIMAccountManager
			_qtIMAccountManager = new QtSimpleIMAccountManager((UserProfile &) _profile, _profileDetailsWindow);

			int index = _ui->imStackedWidget->addWidget(_qtIMAccountManager->getWidget());
			_ui->imStackedWidget->setCurrentIndex(index);
		}
	}

	if (advancedMode) {
		_ui->advancedButton->setText(tr("<< Simple"));
	} else {
		_ui->advancedButton->setText(tr("Advanced >>"));
	}
}

void QtProfileDetails::websiteButtonClicked() {
	WebBrowser::openUrl(_ui->webLineEdit->text().toStdString());
}

void QtProfileDetails::emailButtonClicked() {
	WebBrowser::openUrl("mailto:" + _ui->emailLineEdit->text().toStdString());
}

void QtProfileDetails::setWengoName(const QString & wengoName) {
	_qtIMContactManager->setWengoName(wengoName);
}

void QtProfileDetails::setGroup(const QString & group) {
	_ui->groupComboBox->setEditText(group);
}

void QtProfileDetails::setFirstName(const QString & firstName) {
	_ui->firstNameLineEdit->setText(firstName);
}

void QtProfileDetails::setLastName(const QString & lastName) {
	_ui->lastNameLineEdit->setText(lastName);
}

void QtProfileDetails::setCountry(const QString & country) {
	_ui->countryComboBox->setEditText(country);
}

void QtProfileDetails::setCity(const QString & city) {
	_ui->cityLineEdit->setText(city);
}

void QtProfileDetails::setState(const QString & state) {
	_ui->stateLineEdit->setText(state);
}

void QtProfileDetails::setWebsite(const QString & website) {
	_ui->webLineEdit->setText(website);
}
