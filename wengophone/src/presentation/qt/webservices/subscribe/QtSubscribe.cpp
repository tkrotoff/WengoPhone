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
#include <QUrl>

const QString QtSubscribe::_termsOfServiceLink = "http://www.wengo.fr";
const QString QtSubscribe::_termsOfServicePdfLink = "http://www.wengo.fr";

QtSubscribe::QtSubscribe(CSubscribe & cSubscribe)
	: QObjectThreadSafe(NULL),
	_cSubscribe(cSubscribe) {

	_sendRequestButtonEnabled =false;
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

QWidget * QtSubscribe::getWidget() const {
	return _subscribeWindow;
}

void QtSubscribe::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSubscribe::updatePresentationThreadSafe, this));
	postEvent(event);
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

void QtSubscribe::initThreadSafe() {
	_subscribeWindow = Widget::transformToWindow(qobject_cast<QWidget *>(WidgetFactory::create(":/forms/webservices/subscribe/Subscribe.ui", 0)));

	_nicknameLineEdit = Object::findChild<QLineEdit *>(_subscribeWindow, "NicknameLineEdit");
	_passwordLineEdit = Object::findChild<QLineEdit *>(_subscribeWindow, "PasswordLineEdit");
	_password2LineEdit = Object::findChild<QLineEdit *>(_subscribeWindow, "ConfirmPasswordLineEdit");
	_emailLineEdit = Object::findChild<QLineEdit *>(_subscribeWindow, "EmailLineEdit");
	_mailLabel = Object::findChild<QLabel *>(_subscribeWindow, "ResultMailLabel");
	_passwordLabel = Object::findChild<QLabel *>(_subscribeWindow, "ResultPasswordLabel");
	_nicknameLabel = Object::findChild<QLabel *>(_subscribeWindow, "ResultNicknameLabel");
	_errorLabel = Object::findChild<QLabel *>(_subscribeWindow, "ErrorMessageLabel");
	_sendRequestButton = Object::findChild<QPushButton *>(_subscribeWindow, "SendRequestPushButton");
	_goToWengoPhoneButton = Object::findChild<QPushButton *>(_subscribeWindow, "GoToWengoPhoneButton");
	_textBrowser = Object::findChild<QTextBrowser *>(_subscribeWindow, "AcceptTermsBrowser");
	_firstPage = Object::findChild<QWidget *>(_subscribeWindow, "FirstPageWidget");
	_secondPage = Object::findChild<QWidget *>(_subscribeWindow, "SecondPageWidget");
	_currentPage = _firstPage;

	connect(_sendRequestButton, SIGNAL(pressed()), SLOT(sendRequest()));
	connect(_goToWengoPhoneButton, SIGNAL(pressed()), SLOT(configureLogin()));
	connect (_textBrowser,SIGNAL(anchorClicked(const QUrl &)),this,SLOT(urlClicked(const QUrl & )));

	//html string for the QTextBrowser
	QString acceptTermsMessage = tr("I read and accept the Terms of Service");
	acceptTermsMessage += "<br>&raquo;&nbsp;<a href=\"" + _termsOfServiceLink + "\">";
	acceptTermsMessage += tr("Read the Terms of Service");
	acceptTermsMessage += "</a><br>";
	acceptTermsMessage += "&raquo;&nbsp;<a href=\"" + _termsOfServicePdfLink + "\">";
	acceptTermsMessage += tr("Download the Terms of Service in PDF format");
	acceptTermsMessage += "</a>";
	_textBrowser->setHtml(acceptTermsMessage);
	
	//set the backgound color of the QTextBrowser
	const QColor & color = qApp->style()->standardPalette().color(QPalette::Window);
	QPalette palette = QPalette(_textBrowser->palette());
	palette.setColor(QPalette::Base, color);
	_textBrowser->setPalette(palette);

	updatePresentation();
	_qtWengoPhone->setSubscribe(this);
}

void QtSubscribe::updatePresentationThreadSafe() {
	_sendRequestButton->setEnabled(_sendRequestButtonEnabled);

	if( _errorMessage !=  "" ) {
		QPalette palette = _errorLabel->palette();
		palette.setColor(QPalette::Active, QPalette::Window, Qt::red);
		_errorLabel->setPalette(palette);
		_errorLabel->setText(_errorMessage);

	} else {
		const QColor & color = qApp->style()->standardPalette().color(QPalette::Window);
		QPalette palette = QPalette(_errorLabel->palette());
		palette.setColor(QPalette::Window, color);
		_errorLabel->setPalette(palette);
		_errorLabel->setText("");
	}

	//update the second page labels
	_mailLabel->setText(_mailResult);
	_passwordLabel->setText(_passwordResult);
	_nicknameLabel->setText(_nicknameResult);

	//change page
	if(_currentPage) {
		QStackedWidget * sw = Object::findChild<QStackedWidget *>(_subscribeWindow, "stackedWidget");
		sw->setCurrentWidget(_currentPage);
	}
}

void QtSubscribe::sendRequest() {

	_sendRequestButtonEnabled = false;
	updatePresentation();

	std::string nickname = _nicknameLineEdit->text().toStdString();
	std::string password1 = _passwordLineEdit->text().toStdString();
	std::string password2 = _password2LineEdit->text().toStdString();
	std::string mail = _emailLineEdit->text().toStdString();

	if( password1 != password2 ) {
		//display an error window
		_errorMessage = tr("Password must be the same");
		_sendRequestButtonEnabled = true;
		updatePresentation();
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

	switch( status ) {
	case WsWengoSubscribe::SubscriptioOk:
		//retrieve mail, nick & pwd for the second page
		_mailResult = _emailLineEdit->text();
		_passwordResult = QString::fromStdString(password);
		_nicknameResult = _nicknameLineEdit->text();
		_currentPage = _secondPage;
		break;

	case WsWengoSubscribe::SubscriptionBadQuery:
		_errorMessage = tr("SubscriptionBadQuery");
		break;

	case WsWengoSubscribe::SubscriptionBadVersion:
		_errorMessage = tr("SubscriptionBadVersion");
		break;

	case WsWengoSubscribe::SubscriptionUnknownError:
		_errorMessage = tr("SubscriptionUnknownError");
		break;

	case WsWengoSubscribe::SubscriptionMailError:
		_errorMessage = tr("SubscriptionMailError");
		break;

	case WsWengoSubscribe::SubscriptionNicknameError:
		_errorMessage = tr("SubscriptionNicknameError");
		break;

	case WsWengoSubscribe::SubscriptionFailed:
		_errorMessage = tr("SubscriptionFailed");
		break;

	default:
		LOG_FATAL("Unknow WsWengoSubscribe::SubscriptionStatus");
	}

	updatePresentation();
}

void QtSubscribe::urlClicked(const QUrl & link){
	_textBrowser->setSource(QUrl(""));
	QStringList args;
	args << link.toString();
	QProcess::startDetached ("explorer.exe",args );
}
