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

#include "HttpRequestWindow.h"

#include "ui_HttpRequestWindow.h"

#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/SafeConnect.h>

#include <QtGui/QtGui>

HttpRequestWindow::HttpRequestWindow(QWidget * parent)
	: QMainWindow(parent) {

	_ui = new Ui::HttpRequestWindow();
	_ui->setupUi(this);

	_httpRequest = new HttpRequest();

	SAFE_CONNECT(_ui->connectButton, SIGNAL(clicked()),
		SLOT(connectButtonClickedSlot()));

	SAFE_CONNECT(_httpRequest, SIGNAL(answerReceivedSignal(int, std::string, IHttpRequest::Error)),
		SLOT(answerReceivedSlot(int, std::string, IHttpRequest::Error)));
}

HttpRequestWindow::~HttpRequestWindow() {
	OWSAFE_DELETE(_httpRequest);
	OWSAFE_DELETE(_ui);
}

void HttpRequestWindow::connectButtonClickedSlot() {
	std::string login = _ui->loginLineEdit->text().replace("@", "%40").toStdString();
	std::string password = _ui->passwordLineEdit->text().toStdString();

	//http://ws.wengo.fr/softphone-sso/sso2.php?lang=en&wl=wengo&login=MYLOGIN&password=MYPASSWORD
	_httpRequest->sendRequest(true, "ws.wengo.fr", 443,
		"/softphone-sso/sso2.php",
		"lang=en&wl=wengo&login=" + login + "&password=" + password,
		false);
}

void HttpRequestWindow::answerReceivedSlot(int requestId, std::string answer, IHttpRequest::Error error) {
	LOG_DEBUG("answer=" + answer);
	_ui->answerTextBrowser->setPlainText("answer=" + QString::fromStdString(answer));
}
