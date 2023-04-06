/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include "IMContactDialog.h"

#include "ui_IMContactDialog.h"

#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <vector>

IMContactDialog::IMContactDialog(QWidget * parent)
	: QDialog(parent) {

	_ui = new Ui::IMContactDialog();
	_ui->setupUi(this);

	initializeWidgets();
}

IMContactDialog::~IMContactDialog() {
	OWSAFE_DELETE(_ui);
}

void IMContactDialog::setContactId(const QString & contactId) {
	_ui->contactId->setText(contactId);
}

QString IMContactDialog::contactId() const {
	return _ui->contactId->text();
}

void IMContactDialog::setAccountType(EnumAccountType::AccountType accountType) {
	QString protocolName = QString::fromStdString(EnumAccountType::toString(accountType));
	int index = _ui->accountType->findText(protocolName);
	if (index != -1) {
		_ui->accountType->setCurrentIndex(index);
	} else {
		LOG_FATAL("accountType not found in ComboBox");
	}
}

EnumAccountType::AccountType IMContactDialog::accountType() const {
	QString protocolName = _ui->accountType->currentText();
	return EnumAccountType::toAccountType(protocolName.toStdString());
}

void IMContactDialog::initializeWidgets() {
	// Initialize ComboBox
	std::vector<EnumAccountType::AccountType> protocolList;
	protocolList.push_back(EnumAccountType::AccountTypeMSN);
	protocolList.push_back(EnumAccountType::AccountTypeYahoo);
	protocolList.push_back(EnumAccountType::AccountTypeAIM);
	protocolList.push_back(EnumAccountType::AccountTypeICQ);
	protocolList.push_back(EnumAccountType::AccountTypeGTalk);
	protocolList.push_back(EnumAccountType::AccountTypeJabber);
	protocolList.push_back(EnumAccountType::AccountTypeSIP);
	protocolList.push_back(EnumAccountType::AccountTypeWengo);
	protocolList.push_back(EnumAccountType::AccountTypeIAX);

	_ui->accountType->clear();

	for (std::vector<EnumAccountType::AccountType>::const_iterator it = protocolList.begin();
		it != protocolList.end();
		++it) {
		_ui->accountType->addItem(QString::fromStdString(EnumAccountType::toString(*it)));
	}
	////
}
