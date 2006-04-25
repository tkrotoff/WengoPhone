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

#include <model/webservices/subscribe/Subscribe.h>
#include <control/CWengoPhone.h>
#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/login/QtLogin.h>

#include <util/Logger.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>
#include <qtutil/Widget.h>

#include <QtGui>

QtSubscribe::QtSubscribe(CSubscribe & cSubscribe)
	: QObjectThreadSafe(NULL),
	_cSubscribe(cSubscribe) {

	_sendRequestButtonEnabled = true;
	_errorMessage = "";
	_mailResult = "";
	_passwordResult = "";
	_nicknameResult = "";
	_firstPage = NULL;
	_secondPage = NULL;
	_currentPage = NULL;

	_qtWengoPhone = (QtWengoPhone *) _cSubscribe.getCWengoPhone().getPresentation();

	_cSubscribe.wengoSubscriptionEvent += boost::bind(&QtSubscribe::wengoSubscriptionEventHandler, this, _1, _2, _3, _4, _5);

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSubscribe::initThreadSafe, this));
	postEvent(event);
}

void QtSubscribe::initThreadSafe() {
	_subscribeWindow = Widget::transformToWindow(qobject_cast<QWidget *>(WidgetFactory::create(":/forms/webservices/subscribe/Subscribe.ui", 0)));

	QPushButton * button = Object::findChild<QPushButton *>(_subscribeWindow, "SendRequestPushButton");
	connect(button, SIGNAL(pressed()), SLOT(sendRequest()));

	QPushButton * goToWengoPhoneButton = Object::findChild<QPushButton *>(_subscribeWindow, "GoToWengoPhoneButton");
	connect(goToWengoPhoneButton, SIGNAL(pressed()), SLOT(configureLogin()));

	static QString termsOfServiceLink = "www.wengo.fr";
	static QString termsOfServicePdfLink = "www.wengo.fr";

	QTextBrowser * textBrowser = Object::findChild<QTextBrowser *>(_subscribeWindow, "AcceptTermsBrowser");
	QString acceptTermsMessage = tr("I read and accept the Terms of Service");
	acceptTermsMessage += "<br>&raquo;&nbsp;<a href=\"" + termsOfServiceLink + "\">";
	acceptTermsMessage += tr("Read the Terms of Service");
	acceptTermsMessage += "</a><br>";
	acceptTermsMessage += "&raquo;&nbsp;<a href=\">" + termsOfServicePdfLink + "\">";
	acceptTermsMessage += tr("Download the Terms of Service in PDF format");
	acceptTermsMessage += "</a>";

	textBrowser->insertHtml(acceptTermsMessage);

	_firstPage = Object::findChild<QWidget *>(_subscribeWindow, "FirstPageWidget");
	_secondPage = Object::findChild<QWidget *>(_subscribeWindow, "SecondPageWidget");
	_currentPage = _firstPage;

	updatePresentation();

	_qtWengoPhone->setSubscribe(this);
}

void QtSubscribe::updatePresentationThreadSafe() {
	QPushButton * button = Object::findChild<QPushButton *>(_subscribeWindow, "SendRequestPushButton");
	button->setEnabled(_sendRequestButtonEnabled);

	QLabel * errorLabel = Object::findChild<QLabel *>(_subscribeWindow, "ErrorMessageLabel");
	if( _errorMessage !=  "" ) {
		QPalette palette = QPalette(errorLabel->palette());
		palette.setColor(QPalette::Active, QPalette::Window, QColor(255, 0, 0));
		errorLabel->setPalette(palette);
		errorLabel->setText(_errorMessage);

	} else {
		QPalette palette = QPalette(errorLabel->palette());
		palette.setColor(QPalette::Active, QPalette::Window, QColor(239, 239, 239));
		errorLabel->setPalette(palette);
		errorLabel->setText("");
	}

	QLabel * mailLabel = Object::findChild<QLabel *>(_subscribeWindow, "ResultMailLabel");
	mailLabel->setText(_mailResult);

	QLabel * passwordLabel = Object::findChild<QLabel *>(_subscribeWindow, "ResultPasswordLabel");
	passwordLabel->setText(_passwordResult);

	QLabel * nicknameLabel = Object::findChild<QLabel *>(_subscribeWindow, "ResultNicknameLabel");
	nicknameLabel->setText(_nicknameResult);

	if(_currentPage) {
		QStackedWidget * sw = Object::findChild<QStackedWidget *>(_subscribeWindow, "stackedWidget");
		sw->setCurrentWidget(_currentPage);
	}
}

void QtSubscribe::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSubscribe::updatePresentationThreadSafe, this));
	postEvent(event);
}

QWidget * QtSubscribe::getWidget() const {
	return _subscribeWindow;
}

void QtSubscribe::sendRequest() {

	_sendRequestButtonEnabled = false;
	updatePresentation();

	QLineEdit * nicknameLineEdit = Object::findChild<QLineEdit *>(_subscribeWindow, "NicknameLineEdit");
	QLineEdit * passwordLineEdit = Object::findChild<QLineEdit *>(_subscribeWindow, "PasswordLineEdit");
	QLineEdit * password2LineEdit = Object::findChild<QLineEdit *>(_subscribeWindow, "ConfirmPasswordLineEdit");
	QLineEdit * emailLineEdit = Object::findChild<QLineEdit *>(_subscribeWindow, "EmailLineEdit");

	std::string nickname = nicknameLineEdit->text().toStdString();
	std::string password1 = passwordLineEdit->text().toStdString();
	std::string password2 = password2LineEdit->text().toStdString();
	std::string mail = emailLineEdit->text().toStdString();

	if( password1 != password2 ) {
		//display an error window
		QMessageBox::critical(_subscribeWindow, "Subsciption error", "Password must be the same");
		return;
	}

	//call the ws
	_cSubscribe.subscribe(mail, nickname, "", password1);
}

void QtSubscribe::wengoSubscriptionEventHandler(
	WsWengoSubscribe & sender, int id, WsWengoSubscribe::SubscriptionStatus status,
	const std::string & errorMessage, const std::string & password) {

	_sendRequestButtonEnabled = true;

	_errorMessage = "";
	if( status != WsWengoSubscribe::SubscriptioOk ) {

		switch( status ) {
		case WsWengoSubscribe::SubscriptioOk:
			break;

		case WsWengoSubscribe::SubscriptionBadQuery:
			_errorMessage = "SubscriptionBadQuery";
			break;

		case WsWengoSubscribe::SubscriptionBadVersion:
			_errorMessage = "SubscriptionBadVersion";
			break;

		case WsWengoSubscribe::SubscriptionUnknownError:
			_errorMessage = "SubscriptionUnknownError";
			break;

		case WsWengoSubscribe::SubscriptionMailError:
			_errorMessage = "SubscriptionMailError";
			break;

		case WsWengoSubscribe::SubscriptionNicknameError:
			_errorMessage = "SubscriptionNicknameError";
			break;

		case WsWengoSubscribe::SubscriptionFailed:
			_errorMessage = "SubscriptionFailed";
			break;

		default:
			LOG_FATAL("Unknow WsWengoSubscribe::SubscriptionStatus");
		}
	} else {

		//retrieve mail, nick & pwd for the second page
		QLineEdit * nicknameLineEdit = Object::findChild<QLineEdit *>(_subscribeWindow, "NicknameLineEdit");
		QLineEdit * emailLineEdit = Object::findChild<QLineEdit *>(_subscribeWindow, "EmailLineEdit");

		_mailResult = emailLineEdit->text();
		_passwordResult = QString::fromStdString(password);
		_nicknameResult = nicknameLineEdit->text();

		_currentPage = _secondPage;
	}

	updatePresentation();
}

void QtSubscribe::configureLogin() {
	_qtWengoPhone->getLogin()->setLogin(_mailResult);
	_qtWengoPhone->getLogin()->setPassword(_passwordResult);
	_qtWengoPhone->getLogin()->setAutoLogin(true);
	_qtWengoPhone->getLogin()->getWidget()->exec();
	_subscribeWindow->close();
}

void QtSubscribe::exec() {
	_subscribeWindow->exec();
}
