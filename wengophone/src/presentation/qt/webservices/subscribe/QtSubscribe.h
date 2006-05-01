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

#ifndef QTSUBSCRIBE_H
#define QTSUBSCRIBE_H

#include <presentation/PSubscribe.h>
#include <control/webservices/subscribe/CSubscribe.h>

#include <qtutil/QObjectThreadSafe.h>

#include <string>

class QDialog;
class CSubscribe;
class QtWengoPhone;
class QTextBrowser;
class QLineEdit;
class QLabel;
class QPushButton;

class QUrl;

/**
 * Qt account creation widget.
 *
 * @author Mathieu Stute
 */
class QtSubscribe : public QObjectThreadSafe, public PSubscribe {
	Q_OBJECT
public:

	QtSubscribe(CSubscribe & cSubscribe);

	QWidget * getWidget() const;

	void updatePresentation();

	void exec();

private Q_SLOTS:

	void sendRequest();

	void configureLogin();


public Q_SLOTS:

	void urlClicked(const QUrl & link);

private:

	void initThreadSafe();

	void updatePresentationThreadSafe();

	void wengoSubscriptionEventHandler(WsWengoSubscribe & sender, int id, WsWengoSubscribe::SubscriptionStatus status,
		const std::string & errorMessage, const std::string & password);

	QDialog * _subscribeWindow;

	QTextBrowser * _textBrowser;

	CSubscribe & _cSubscribe;

	QtWengoPhone * _qtWengoPhone;

	bool _sendRequestButtonEnabled;

	QString _errorMessage;

	QString _mailResult;

	QString _passwordResult;

	QString _nicknameResult;

	QWidget * _firstPage;

	QWidget * _secondPage;

	QWidget * _currentPage;

	QLineEdit * _nicknameLineEdit;

	QLineEdit * _passwordLineEdit;

	QLineEdit * _password2LineEdit;

	QLineEdit * _emailLineEdit;

	QLabel * _errorLabel;

	QLabel * _mailLabel;

	QLabel * _passwordLabel;

	QLabel * _nicknameLabel;

	QPushButton * _sendRequestButton;

	QPushButton * _goToWengoPhoneButton;

	static const QString _termsOfServiceLink;

	static const QString _termsOfServicePdfLink;
};

#endif	//QTSUBSCRIBE_H
