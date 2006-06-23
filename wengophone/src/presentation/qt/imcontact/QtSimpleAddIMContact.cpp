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

#include "QtSimpleAddIMContact.h"

#include <qtutil/WidgetBackgroundImage.h>

#include <presentation/qt/profile/QtProfileDetails.h>

#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/contactlist/ContactProfile.h>
#include <model/profile/UserProfile.h>

#include <QtGui>

static const QString IM_PROTOCOL_WENGO = "Wengo";

QtSimpleAddIMContact::QtSimpleAddIMContact(CUserProfile & cUserProfile,
	ContactProfile & contactProfile,
	QWidget * parent) : _cUserProfile(cUserProfile), _contactProfile(contactProfile), QDialog(parent) {

	_ui.setupUi(this);

	QPixmap pixmap = QPixmap(":pics/headers/login.png");

	WidgetBackgroundImage::setBackgroundImage(_ui.accountLabel, ":pics/headers/login.png", true);

	setMinimumSize(QSize(pixmap.width()+8,0));
	setMaximumSize(QSize(pixmap.width()+8,0));

	ContactGroupVector tmp = _cUserProfile.getCContactList().getContactGroups();
	ContactGroupVector::const_iterator it;
	for ( it = tmp.begin(); it != tmp.end(); ++it) {
		_ui.groupComboBox->addItem(QString::fromUtf8((*it).second.c_str()),
			QString::fromStdString((*it).first.c_str()));

		std::string str1 = (*it).first;
		std::string str2 = contactProfile.getGroupId();

		if (str1 == str2) {
			_ui.groupComboBox->setCurrentIndex(_ui.groupComboBox->findText(QString::fromUtf8((*it).second.c_str())));
		}
	}

	_ui.contactTypeComboBox->addItem(QIcon(":pics/protocols/wengo.png"), IM_PROTOCOL_WENGO);
	_ui.contactTypeComboBox->addItem(QIcon(":pics/protocols/msn.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolMSN)));
	_ui.contactTypeComboBox->addItem(QIcon(":pics/protocols/aim.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolAIMICQ)));
	_ui.contactTypeComboBox->addItem(QIcon(":pics/protocols/yahoo.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolYahoo)));
	_ui.contactTypeComboBox->addItem(QIcon(":pics/protocols/jabber.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolJabber)));
	_ui.contactTypeComboBox->addItem(QIcon(":pics/protocols/sip.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolSIPSIMPLE)));
	connect(_ui.okButton, SIGNAL(clicked()), SLOT(saveContact()));
	connect(_ui.cancelButton, SIGNAL(clicked()), SLOT(reject()));
	connect(_ui.advancedConfigPushButton, SIGNAL(clicked()), SLOT(advanced()));
	connect(_ui.contactTypeComboBox, SIGNAL(currentIndexChanged(const QString &)),
			SLOT(currentIndexChanged(const QString &)));
}

QtSimpleAddIMContact::~QtSimpleAddIMContact() {

}

void QtSimpleAddIMContact::saveContact() {
	std::string contactId = _ui.contactIdLineEdit->text().toStdString();
	QString protocolName = _ui.contactTypeComboBox->currentText();
	if (protocolName == IM_PROTOCOL_WENGO) {
		_contactProfile.setWengoPhoneId(contactId);
	} else {
		EnumIMProtocol::IMProtocol imProtocol = EnumIMProtocol::toIMProtocol(protocolName.toStdString());
		IMContact imContact(imProtocol, contactId);
		IMAccount * imAccount = *_cUserProfile.getIMAccountsOfProtocol(imProtocol).begin();
		imContact.setIMAccount(imAccount);
		_contactProfile.addIMContact(imContact);
	}
	int index = _ui.groupComboBox->findText(_ui.groupComboBox->currentText());
	QVariant groupId;
	groupId = _ui.groupComboBox->itemData(index);
	//If the group does not exist
	if (groupId.isNull()) {
		std::string groupName = std::string(_ui.groupComboBox->currentText().toUtf8().data());
		_cUserProfile.getCContactList().addContactGroup(groupName);
		groupId = QString::fromStdString(_cUserProfile.getCContactList().getContactGroupIdFromName(groupName));
	}
	_contactProfile.setGroupId(groupId.toString().toStdString());
	_contactProfile.setFirstName(_ui.nameLineEdit->text().toUtf8().data());
	accept();
}

void QtSimpleAddIMContact::advanced() {
	QtProfileDetails qtProfileDetails(_cUserProfile,_contactProfile, this);
	hide();
	if (qtProfileDetails.show()) {
		accept();
		return;
	}
	reject();
}

bool QtSimpleAddIMContact::haveAccount(EnumIMProtocol::IMProtocol imProtocol) const {
	std::set<IMAccount *> imAccounts = _cUserProfile.getIMAccountsOfProtocol(imProtocol);
	if (imAccounts.begin() == imAccounts.end()) {
		return false;
	}
	return true;
}

void QtSimpleAddIMContact::currentIndexChanged (const QString & text) {
	EnumIMProtocol::IMProtocol imProtocol;

	if (text == IM_PROTOCOL_WENGO) {
		return;
	} else {
		imProtocol = EnumIMProtocol::toIMProtocol(text.toStdString());
		if (!haveAccount(imProtocol)) {
			QMessageBox msgBox( tr("WengoPhone - Bad account"),
			tr("Yout are not logged to this network\n"
			"Use \"the Tools / IM accounts\" menu to login to this network"),
			QMessageBox::Information,
			QMessageBox::Ok, QMessageBox::NoButton,
			QMessageBox::NoButton,this);

			msgBox.exec();
		}
	}
}
