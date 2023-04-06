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

#include "QtCreditWidget.h"

#include "ui_CreditWidget.h"

#include <presentation/qt/QtNoWengoAlert.h>
#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/config/QtWengoConfigDialog.h>

#include <model/account/SipAccount.h>
#include <model/profile/UserProfile.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/webservices/url/WsUrl.h>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>



#include <qtutil/LanguageChangeEventFilter.h>
#include <qtutil/SafeConnect.h>

#include <util/SafeDelete.h>

#include <QtGui/QtGui>

QtCreditWidget::QtCreditWidget(CWengoPhone & cWengoPhone, UserProfile & userProfile, QWidget * parent)
	: QObject(parent),
	_cWengoPhone(cWengoPhone), _userProfile(userProfile) {

	_creditWidget = new QWidget(parent);

	_ui = new Ui::CreditWidget();
	_ui->setupUi(_creditWidget);

	//landlineNumberButton
	//SAFE_CONNECT(_ui->landlineNumberButton, SIGNAL(clicked()), SLOT(landlineNumberClicked()));

	//callForwardButton
	SAFE_CONNECT(_ui->callForwardButton, SIGNAL(clicked()), SLOT(callforwardModeClicked()));

	//buyCreditsButton
	SAFE_CONNECT(_ui->buyCreditsButton, SIGNAL(clicked()), SLOT(buyCreditsClicked()));

	//voiceMailButton
	SAFE_CONNECT(_ui->voiceMailButton, SIGNAL(clicked()), SLOT(voiceMailClicked()));

	LANGUAGE_CHANGE(_creditWidget);
}

QtCreditWidget::~QtCreditWidget() {
	OWSAFE_DELETE(_ui);
}

QWidget * QtCreditWidget::getWidget() const {
	return _creditWidget;
}

void QtCreditWidget::updatePresentation() {
	if (!_callForwardMode.isEmpty()) {
		_ui->callForwardLabel->setText(_callForwardMode);
	}

	/*
	if (!_landlineNumber.isEmpty()) {
		_ui->landlineNumberLabel->setText(_landlineNumber);
	}
	*/
}

void QtCreditWidget::setLandlineNumber(const QString & number) {
	_landlineNumber = number;
	updatePresentation();
}

void QtCreditWidget::setCallForwardMode(const QString & callForwardMode) {
	_callForwardMode = callForwardMode;
	updatePresentation();
}

bool QtCreditWidget::isWengoConnexion() {
	bool result = false;
	CUserProfile * cuserprofile = _cWengoPhone.getCUserProfileHandler().getCUserProfile();
	if (cuserprofile) {
		if (cuserprofile->getUserProfile().hasWengoAccount()) {
			result = true;
		}
	}
	return result;
}

void QtCreditWidget::buyCreditsClicked() {
	
	if (isWengoConnexion()) {
		WsUrl::showWengoBuyWengos();
	} else {
		QtWengoPhone * qtWengoPhone = static_cast<QtWengoPhone*>(_cWengoPhone.getPresentation());
		QtNoWengoAlert noWengoAlert(NULL, *qtWengoPhone,
			"Buy credits", 
			"To buy credit you need to<BR>\
			be logged in with a Wengo account.");
		noWengoAlert.getQDialog()->exec();
	}
}

void QtCreditWidget::callforwardModeClicked() {

	if (isWengoConnexion()) {
		QtWengoConfigDialog* dialog = QtWengoConfigDialog::showInstance(_cWengoPhone);
		dialog->showCallForwardPage();
	} else {
		QtWengoPhone * qtWengoPhone = static_cast<QtWengoPhone*>(_cWengoPhone.getPresentation());
		QtNoWengoAlert noWengoAlert(NULL, *qtWengoPhone,
			"Call forward", 
			"To use call forward you need to<BR>\
			be logged in with a Wengo account.");
		noWengoAlert.getQDialog()->exec();
	}
}

void QtCreditWidget::landlineNumberClicked() {

	if (isWengoConnexion()) {
		WsUrl::showWengoPhoneNumber();
	} else {
		QtWengoPhone * qtWengoPhone = static_cast<QtWengoPhone*>(_cWengoPhone.getPresentation());
		QtNoWengoAlert noWengoAlert(NULL, *qtWengoPhone,
			"Number", 
			"To get a number you need to<BR>\
			be logged in with a Wengo account.");
		noWengoAlert.getQDialog()->exec();
	}
}

void QtCreditWidget::languageChanged() {
	_ui->retranslateUi(_creditWidget);
	updatePresentation();
}

void QtCreditWidget::voiceMailClicked() {
	IPhoneLine * phoneLine = _userProfile.getActivePhoneLine();
	if (phoneLine) {
		phoneLine->makeCall(_userProfile.getSipAccount()->getVoicemailNumber());
	}
}
