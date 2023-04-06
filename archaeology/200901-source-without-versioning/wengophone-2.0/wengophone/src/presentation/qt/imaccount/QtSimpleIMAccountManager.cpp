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

#include "QtSimpleIMAccountManager.h"

#include "ui_SimpleIMAccountManager.h"

#include "QtMSNSettings.h"
#include "QtAIMSettings.h"
#include "QtYahooSettings.h"
#include "QtJabberSettings.h"
#include "QtGoogleTalkSettings.h"

#include <model/profile/UserProfile.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

QtSimpleIMAccountManager::QtSimpleIMAccountManager(UserProfile & userProfile, QWidget * parent)
	: QObject(parent),
	_userProfile(userProfile) {

	_imAccountManagerWidget = new QWidget(NULL);

	_ui = new Ui::SimpleIMAccountManager();
	_ui->setupUi(_imAccountManagerWidget);

	loadIMAccounts();
}

QtSimpleIMAccountManager::~QtSimpleIMAccountManager() {
	OWSAFE_DELETE(_ui);
}

QWidget * QtSimpleIMAccountManager::getWidget() const {
	return _imAccountManagerWidget;
}

void QtSimpleIMAccountManager::loadIMAccounts() {
	/*IMAccountHandler & imAccountHandler = _userProfile.getIMAccountHandler();

	for (IMAccountHandler::iterator it = imAccountHandler.begin(); it != imAccountHandler.end(); it++) {
		IMAccount * imAccount = (IMAccount *) &(*it);

		EnumIMProtocol::IMProtocol imProtocol = imAccount->getProtocol();

		switch (imProtocol) {
		case EnumIMProtocol::IMProtocolWengo:
			_ui->wengoLineEdit->setText(QString::fromStdString(imAccount->getLogin()));
			_ui->wengoPasswordLineEdit->setText(QString::fromStdString(imAccount->getPassword()));
			break;
		case EnumIMProtocol::IMProtocolMSN:
			_ui->msnLineEdit->setText(QString::fromStdString(imAccount->getLogin()));
			_ui->msnPasswordLineEdit->setText(QString::fromStdString(imAccount->getPassword()));
			break;
		case EnumIMProtocol::IMProtocolAIMICQ:
			_ui->aimLineEdit->setText(QString::fromStdString(imAccount->getLogin()));
			_ui->aimPasswordLineEdit->setText(QString::fromStdString(imAccount->getPassword()));
			break;
		case EnumIMProtocol::IMProtocolYahoo:
			_ui->yahooLineEdit->setText(QString::fromStdString(imAccount->getLogin()));
			_ui->yahooPasswordLineEdit->setText(QString::fromStdString(imAccount->getPassword()));
			break;
		case EnumIMProtocol::IMProtocolJabber:
			if (String(imAccount->getLogin()).contains("gmail")) {
				_ui->googleTalkLineEdit->setText(QString::fromStdString(imAccount->getLogin()));
				_ui->googleTalkPasswordLineEdit->setText(QString::fromStdString(imAccount->getPassword()));
			} else {
				_ui->jabberLineEdit->setText(QString::fromStdString(imAccount->getLogin()));
				_ui->jabberPasswordLineEdit->setText(QString::fromStdString(imAccount->getPassword()));
			}
			break;

		case EnumIMProtocol::IMProtocolUnknown:
		case EnumIMProtocol::IMProtocolAll:
		case EnumIMProtocol::IMProtocolSIPSIMPLE:
		case EnumIMProtocol::IMProtocolSIP:
			break;

		default:
			LOG_FATAL("unknown IMProtocol=" + String::fromNumber(imProtocol));
		}
	}*/
}

void QtSimpleIMAccountManager::saveIMAccounts() {
	/*bool msnSaved = false;
	bool aimSaved = false;
	bool yahooSaved = false;
	bool jabberSaved = false;
	bool googleTalkSaved = false;

	IMAccountHandler & imAccountHandler = _userProfile.getIMAccountHandler();

	for (IMAccountHandler::iterator it = imAccountHandler.begin(); it != imAccountHandler.end(); it++) {
		IMAccount * imAccount = (IMAccount *) &(*it);

		EnumIMProtocol::IMProtocol imProtocol = imAccount->getProtocol();

		switch (imProtocol) {
		case EnumIMProtocol::IMProtocolWengo:
			break;

		case EnumIMProtocol::IMProtocolMSN: {
			std::string login = _ui->msnLineEdit->text();
			std::string password = _ui->msnPasswordLineEdit->text();

			if (imAccount->getLogin() == login) {
				QtMSNSettings::save(_userProfile, imAccount, login, password);
				msnSaved = true;
			} else {
				//Keeps the other existing IMAccounts
			}
			break;
		}

		case EnumIMProtocol::IMProtocolAIMICQ: {
			std::string login = _ui->aimLineEdit->text();
			std::string password = _ui->aimPasswordLineEdit->text();

			if (imAccount->getLogin() == login) {
				QtAIMSettings::save(_userProfile, imAccount, login, password);
				aimSaved = true;
			} else {
				//Keeps the other existing IMAccounts
			}
			break;
		}

		case EnumIMProtocol::IMProtocolYahoo: {
			std::string login = _ui->yahooLineEdit->text();
			std::string password = _ui->yahooPasswordLineEdit->text();

			if (imAccount->getLogin() == login) {
				QtYahooSettings::save(_userProfile, imAccount, login, password);
				yahooSaved = true;
			} else {
				//Keeps the other existing IMAccounts
			}
			break;
		}

		case EnumIMProtocol::IMProtocolJabber: {
			std::string login = _ui->jabberLineEdit->text();
			std::string password = _ui->jabberPasswordLineEdit->text();

			if (imAccount->getLogin() == login) {
				QtJabberSettings::save(_userProfile, imAccount, login, password);
				jabberSaved = true;
			} else {
				//Keeps the other existing IMAccounts
			}

			login = _ui->googleTalkLineEdit->text();
			password = _ui->googleTalkPasswordLineEdit->text();

			if (imAccount->getLogin() == login) {
				QtGoogleTalkSettings::save(_userProfile, imAccount, login, password);
				googleTalkSaved = true;
			} else {
				//Keeps the other existing IMAccounts
			}
			break;
		}

		case EnumIMProtocol::IMProtocolUnknown:
		case EnumIMProtocol::IMProtocolAll:
		case EnumIMProtocol::IMProtocolSIPSIMPLE:
		case EnumIMProtocol::IMProtocolSIP:
			break;

		default:
			LOG_FATAL("unknown IMProtocol=" + String::fromNumber(imProtocol));
		}
	}

	std::string login = _ui->msnLineEdit->text();
	std::string password = _ui->msnPasswordLineEdit->text();
	if (!msnSaved && !login.empty()) {
		QtMSNSettings::save(_userProfile, NULL, login, password);
	}

	login = _ui->aimLineEdit->text();
	password = _ui->aimPasswordLineEdit->text();
	if (!aimSaved && !login.empty()) {
		QtAIMSettings::save(_userProfile, NULL, login, password);
	}

	login = _ui->yahooLineEdit->text();
	password = _ui->yahooPasswordLineEdit->text();
	if (!yahooSaved && !login.empty()) {
		QtYahooSettings::save(_userProfile, NULL, login, password);
	}

	login = _ui->jabberLineEdit->text();
	password = _ui->jabberPasswordLineEdit->text();
	if (!jabberSaved && !login.empty()) {
		QtJabberSettings::save(_userProfile, NULL, login, password);
	}

	login = _ui->googleTalkLineEdit->text();
	password = _ui->googleTalkPasswordLineEdit->text();
	if (!googleTalkSaved && !login.empty()) {
		QtGoogleTalkSettings::save(_userProfile, NULL, login, password);
	}*/
}
