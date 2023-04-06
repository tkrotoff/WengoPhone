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

#include "QtLoginDialog.h"

#include "QtAddAccount.h"
#include "QtAddWengoAccount.h"
#include "QtAddSIPAccount.h"
#include "QtLogAccount.h"

#include <presentation/qt/QtWengoPhone.h>

#include <control/profile/CUserProfileHandler.h>

#include <model/account/SipAccount.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <qtutil/SafeConnect.h>

#include <util/Logger.h>


#include <util/String.h>

#include <QtGui/QtGui>

QtLoginDialog::QtLoginDialog(QtWengoPhone * qtWengoPhone, CUserProfileHandler & cUserProfileHandler)
	:QDialog(qtWengoPhone->getWidget()),
	_qtWengoPhone(qtWengoPhone) {

	_centralWidget = new QStackedWidget(this);
	_loginPage.push_back(new QtAddAccount(this, _centralWidget, cUserProfileHandler));
	_loginPage.push_back(new QtAddWengoAccount(this, _centralWidget, cUserProfileHandler));
	_loginPage.push_back(new QtAddSIPAccount(this, _centralWidget, cUserProfileHandler));
	_loginPage.push_back(new QtLogAccount(this, _centralWidget, cUserProfileHandler));

    _centralWidget->addWidget(_loginPage[AddAccount]);
    _centralWidget->addWidget(_loginPage[AddWengoAccount]);
    _centralWidget->addWidget(_loginPage[AddSIPAccount]);
	_centralWidget->addWidget(_loginPage[LogAccount]);
	
	setWindowTitle(tr("@product@ - Connexion"));

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(_centralWidget);
	
	changePage(LogAccount);
}

QtLoginDialog::~QtLoginDialog() {
}

void QtLoginDialog::changePage(int newPage, std::string sipAccountName) {
	if ( (newPage >= 0) && (newPage < _centralWidget->count())) {
		_centralWidget->setCurrentIndex(newPage);
		_loginPage[newPage]->initPage();
		if (!sipAccountName.empty()) {
			_loginPage[newPage]->load(sipAccountName);
		}
	}
}

ILogin* QtLoginDialog::currentPage() const {
	return _loginPage[_centralWidget->currentIndex()];
}

void QtLoginDialog::setInvalidAccount(const SipAccount & sipAccount) {
	if (sipAccount.getType() ==  SipAccount::SipAccountTypeWengo) {
		//wengo account
		changePage(AddWengoAccount);
		_loginPage[AddWengoAccount]->load(sipAccount);
		_loginPage[AddWengoAccount]->setErrorMessage(tr("Wrong email/password entered"));
	} else {
		//"generic" sip account
		changePage(AddSIPAccount);
		_loginPage[AddSIPAccount]->load(sipAccount);
		_loginPage[AddSIPAccount]->setErrorMessage(tr("Wrong email/password entered"));
	}
}

void QtLoginDialog::setValidAccount(SipAccount sipAccount) {
	changePage(LogAccount);
	_loginPage[LogAccount]->load(sipAccount);
	_loginPage[LogAccount]->setInfoMessage();
}

void QtLoginDialog::setInfoMessage() {
	_loginPage[_centralWidget->currentIndex()]->setInfoMessage();
}

void QtLoginDialog::setErrorMessage(const QString & message) {
	_loginPage[_centralWidget->currentIndex()]->setErrorMessage(message);
}
