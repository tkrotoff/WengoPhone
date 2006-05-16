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

#include "QtSubscribe.h"

#include "ui_SubscribeWengo1.h"

#include "QtSubscribe2.h"

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/login/QtLogin.h>

#include <control/CWengoPhone.h>

#include <model/webservices/subscribe/Subscribe.h>

#include <util/Logger.h>

#include <QtGui>

const char * QtSubscribe::TERMSOFSERVICE_LINK = "http://www.wengo.fr";
const char * QtSubscribe::TERMSOFSERVICEPDF_LINK = "http://www.wengo.fr";

QtSubscribe::QtSubscribe(CSubscribe & cSubscribe)
	: QObjectThreadSafe(NULL),
	_cSubscribe(cSubscribe) {

	_subscribeButtonEnabled = false;

	_qtWengoPhone = (QtWengoPhone *) _cSubscribe.getCWengoPhone().getPresentation();

	_cSubscribe.wengoSubscriptionEvent += boost::bind(&QtSubscribe::wengoSubscriptionEventHandler, this, _1, _2, _3, _4, _5);

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSubscribe::initThreadSafe, this));
	postEvent(event);
}

QtSubscribe::~QtSubscribe() {
	delete _ui;
}

void QtSubscribe::initThreadSafe() {
	_subscribeWindow = new QDialog(_qtWengoPhone->getWidget());

	_ui = new Ui::SubscribeWengo1();
	_ui->setupUi(_subscribeWindow);

	connect(_ui->subscribeButton, SIGNAL(clicked()), SLOT(sendRequest()));

	showErrorMessage(QString::null);
	_qtWengoPhone->setSubscribe(this);
}

void QtSubscribe::showErrorMessage(const QString & errorMessage) {
	_ui->subscribeButton->setEnabled(_subscribeButtonEnabled);

	if (!errorMessage.isEmpty()) {
		QPalette palette = _ui->errorMessageLabel->palette();
		palette.setColor(QPalette::Active, QPalette::Window, Qt::red);
		_ui->errorMessageLabel->setPalette(palette);
	} else {
		const QColor & color = qApp->style()->standardPalette().color(QPalette::Window);
		QPalette palette = QPalette(_ui->errorMessageLabel->palette());
		palette.setColor(QPalette::Window, color);
		_ui->errorMessageLabel->setPalette(palette);
	}
	_ui->errorMessageLabel->setText(errorMessage);
}

QWidget * QtSubscribe::getWidget() const {
	return _subscribeWindow;
}

void QtSubscribe::show() {
	_subscribeWindow->exec();
}

void QtSubscribe::sendRequest() {
	_subscribeButtonEnabled = false;
	showErrorMessage(QString::null);

	std::string nickname = _ui->nicknameLineEdit->text().toStdString();
	std::string password1 = _ui->passwordLineEdit->text().toStdString();
	std::string password2 = _ui->confirmPasswordLineEdit->text().toStdString();
	std::string mail = _ui->emailLineEdit->text().toStdString();

	if (password1 != password2) {
		//Display an error window
		QString errorMessage = tr("Confirm password does not matched password");
		_subscribeButtonEnabled = true;
		showErrorMessage(errorMessage);
	} else {
		//Call the ws
		_cSubscribe.subscribe(mail, nickname, "", password1);
	}
}

void QtSubscribe::wengoSubscriptionEventHandler(
	WsWengoSubscribe & sender, int id, WsWengoSubscribe::SubscriptionStatus status,
	const std::string & errorMessage, const std::string & password) {

	typedef PostEvent2<void (WsWengoSubscribe::SubscriptionStatus, const std::string &), WsWengoSubscribe::SubscriptionStatus, const std::string &> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSubscribe::wengoSubscriptionEventHandlerThreadSafe, this, _1, _2), status, password);
	postEvent(event);
}

void QtSubscribe::wengoSubscriptionEventHandlerThreadSafe(WsWengoSubscribe::SubscriptionStatus status, const std::string & password) {
	_subscribeButtonEnabled = true;
	QString errorMessage;

	switch (status) {
	case WsWengoSubscribe::SubscriptioOk:
		//Next page
		QtSubscribe2(_ui->nicknameLineEdit->text(), QString::fromStdString(password),
				_ui->emailLineEdit->text(), _subscribeWindow);
		break;

	case WsWengoSubscribe::SubscriptionBadQuery:
		errorMessage = tr("SubscriptionBadQuery");
		break;

	case WsWengoSubscribe::SubscriptionBadVersion:
		errorMessage = tr("SubscriptionBadVersion");
		break;

	case WsWengoSubscribe::SubscriptionUnknownError:
		errorMessage = tr("SubscriptionUnknownError");
		break;

	case WsWengoSubscribe::SubscriptionMailError:
		errorMessage = tr("SubscriptionMailError");
		break;

	case WsWengoSubscribe::SubscriptionNicknameError:
		errorMessage = tr("SubscriptionNicknameError");
		break;

	case WsWengoSubscribe::SubscriptionFailed:
		errorMessage = tr("SubscriptionFailed");
		break;

	default:
		LOG_FATAL("Unknow SubscriptionStatus=" + String::fromNumber(status));
	}

	showErrorMessage(errorMessage);
}
