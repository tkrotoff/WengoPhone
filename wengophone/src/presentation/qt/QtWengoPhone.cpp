/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "QtWengoPhone.h"

#include <control/CWengoPhone.h>
#include <WengoPhoneBuildId.h>

#include "phoneline/QtPhoneLine.h"
#include "phonecall/QtPhoneCall.h"
#include "QtWengoPhoneLogger.h"
#include "login/QtLogin.h"
#include "contactlist/QtContactList.h"
#include "contactlist/QtAddContact.h"
#include "QtDialpad.h"
#include "QtAbout.h"

#include <WidgetFactory.h>
#include <Object.h>
#include <Thread.h>
#include <Logger.h>

#include <QtGui>

QtWengoPhone::QtWengoPhone(CWengoPhone & cWengoPhone)
	: QObjectThreadSafe(),
	_cWengoPhone(cWengoPhone) {

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::initThreadSafe, this));
	postEvent(event);
}

void QtWengoPhone::initThreadSafe() {
	_wengoPhoneWindow = qobject_cast<QMainWindow *>(WidgetFactory::create(":/forms/WengoPhoneWindow.ui", NULL));
	_wengoPhoneWindow->setWindowTitle(WengoPhoneBuildId::SOFTPHONE_NAME);

	//callButton
	_callButton = Object::findChild<QPushButton *>(_wengoPhoneWindow, "callButton");

	//phoneComboBox
	_phoneComboBox = Object::findChild<QComboBox *>(_wengoPhoneWindow, "phoneComboBox");

	//Button initialization
	initButtons();

	//tabWidget
	_tabWidget = Object::findChild<QTabWidget *>(_wengoPhoneWindow, "tabWidget");

	//dialpad
	QtDialpad * qtDialpad = new QtDialpad(this);
	QWidget * tabDialpad = Object::findChild<QWidget *>(_tabWidget, "tabDialpad");
	createLayout(tabDialpad)->addWidget(qtDialpad->getWidget());


	//actionShowWengoAccount
	QAction * actionShowWengoAccount = Object::findChild<QAction *>(_wengoPhoneWindow, "actionShowWengoAccount");
	connect(actionShowWengoAccount, SIGNAL(triggered()), SLOT(showWengoAccount()));

	//actionOpenWengoAccount
	QAction * actionOpenWengoAccount = Object::findChild<QAction *>(_wengoPhoneWindow, "actionOpenWengoAccount");
	connect(actionOpenWengoAccount, SIGNAL(triggered()), SLOT(openWengoAccount()));

	//actionExit
	QAction * actionExit = Object::findChild<QAction *>(_wengoPhoneWindow, "actionExit");
	connect(actionExit, SIGNAL(triggered()), SLOT(exitApplication()));

	//actionAddContact
	QAction * actionAddContact = Object::findChild<QAction *>(_wengoPhoneWindow, "actionAddContact");
	connect(actionAddContact, SIGNAL(triggered()), SLOT(addContact()));

	//actionConfiguration
	QAction * actionConfiguration = Object::findChild<QAction *>(_wengoPhoneWindow, "actionConfiguration");
	connect(actionConfiguration, SIGNAL(triggered()), SLOT(showConfig()));

	//actionShowForum
	QAction * actionShowForum = Object::findChild<QAction *>(_wengoPhoneWindow, "actionShowForum");
	connect(actionShowForum, SIGNAL(triggered()), SLOT(showForum()));

	//actionShowHelp
	QAction * actionShowHelp = Object::findChild<QAction *>(_wengoPhoneWindow, "actionShowHelp");
	connect(actionShowHelp, SIGNAL(triggered()), SLOT(showHelp()));

	//actionShowAbout
	QAction * actionShowAbout = Object::findChild<QAction *>(_wengoPhoneWindow, "actionShowAbout");
	connect(actionShowAbout, SIGNAL(triggered()), SLOT(showAbout()));


	//centralwidget
	QWidget * centralwidget = Object::findChild<QWidget *>(_wengoPhoneWindow, "centralwidget");
	_wengoPhoneWindow->setCentralWidget(centralwidget);

	//menubar
	QMenuBar * menubar = Object::findChild<QMenuBar *>(_wengoPhoneWindow, "menubar");
	_wengoPhoneWindow->setMenuBar(menubar);

	//statusbar
	QStatusBar * statusbar = Object::findChild<QStatusBar *>(_wengoPhoneWindow, "statusbar");
	_wengoPhoneWindow->setStatusBar(statusbar);

	_wengoPhoneWindow->show();
}

void QtWengoPhone::initButtons() {
	//callButton
	_callButton->disconnect();
	connect(_callButton, SIGNAL(clicked()),
		this, SLOT(callButtonClicked()));
	enableCallButton();

	//phoneComboBox
	_phoneComboBox->disconnect();
	connect(_phoneComboBox, SIGNAL(activated(int)), SLOT(callButtonClicked()));
	connect(_phoneComboBox, SIGNAL(editTextChanged(const QString &)), SLOT(enableCallButton()));
}

void QtWengoPhone::enableCallButton() {
	std::string phoneNumber = _phoneComboBox->currentText().toStdString();
	_callButton->setEnabled(!phoneNumber.empty());
}

void QtWengoPhone::addPhoneLine(PPhoneLine * pPhoneLine) {
	typedef PostEvent1<void (PPhoneLine *), PPhoneLine *> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::addPhoneLineThreadSafe, this, _1), pPhoneLine);
	postEvent(event);
}

void QtWengoPhone::addPhoneLineThreadSafe(PPhoneLine * pPhoneLine) {
	QStackedWidget * stack = Object::findChild<QStackedWidget *>(_wengoPhoneWindow, "phoneLineStackedWidget");
	QWidget * widget = ((QtPhoneLine *) pPhoneLine)->getWidget();
	stack->addWidget(widget);
	stack->setCurrentWidget(widget);
	LOG_DEBUG("QtPhoneLine added" + widget->objectName().toStdString());
}

void QtWengoPhone::showLoginWindow() {
	static QtLogin * login = new QtLogin(_wengoPhoneWindow);

	int ret = login->exec();

	if (ret == QDialog::Accepted) {
		_cWengoPhone.addWengoAccount(login->getLogin(), login->getPassword(), login->hasAutoLogin());
	}
}

QLayout * QtWengoPhone::createLayout(QWidget * parent) {
	QGridLayout * layout = new QGridLayout(parent);
	layout->setSpacing(0);
	layout->setMargin(0);
	return layout;
}

void QtWengoPhone::setLogger(QtWengoPhoneLogger * qtWengoPhoneLogger) {
	QWidget * tabLogger = Object::findChild<QWidget *>(_tabWidget, "tabLogger");
	createLayout(tabLogger)->addWidget(qtWengoPhoneLogger->getWidget());
	LOG_DEBUG("QtWengoPhoneLogger added");
}

void QtWengoPhone::setContactList(QtContactList * qtContactList) {
	QWidget * tabContactList = Object::findChild<QWidget *>(_tabWidget, "tabContactList");
	createLayout(tabContactList)->addWidget(qtContactList->getWidget());
	LOG_DEBUG("QtContactList added");
}

void QtWengoPhone::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtWengoPhone::updatePresentationThreadSafe() {
}

void QtWengoPhone::wengoLoginStateChangedEvent(WengoPhone::LoginState state, const std::string & login, const std::string & password) {
	typedef PostEvent3<void (WengoPhone::LoginState, const std::string &, const std::string &), WengoPhone::LoginState, std::string, std::string> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::wengoLoginStateChangedEventThreadSafe, this, _1, _2, _3), state, login, password);
	postEvent(event);
}

void QtWengoPhone::wengoLoginStateChangedEventThreadSafe(WengoPhone::LoginState state, std::string login, std::string password) {
	switch (state) {
	case WengoPhone::LoginOk:
		break;

	case WengoPhone::LoginPasswordError:
		break;

	case WengoPhone::LoginNetworkError:
		break;

	case WengoPhone::LoginNoAccount:
		showLoginWindow();
		break;

	default:
		LOG_FATAL("Unknown state");
	};
}

void QtWengoPhone::dialpad(const QString & num) {
	//if (_cWengoPhone.hasActiveCall()) {
	//	_cWengoPhone.dialpad(num);
	//} else {
		_phoneComboBox->setEditText(_phoneComboBox->currentText() + num);
	//}
}

void QtWengoPhone::callButtonClicked() {
	std::string phoneNumber = _phoneComboBox->currentText().toStdString();
	if (!phoneNumber.empty()) {
		_cWengoPhone.makeCall(phoneNumber);
	}
}

void QtWengoPhone::showWengoAccount() {
	_cWengoPhone.showWengoAccount();
}

void QtWengoPhone::openWengoAccount() {
	showLoginWindow();
}

void QtWengoPhone::exitApplication() {
	static const unsigned int TIME_BEFORE_EXIT = 0000;

	_wengoPhoneWindow->hide();
	_cWengoPhone.terminate();
	Thread::msleep(TIME_BEFORE_EXIT);
	QCoreApplication::exit(EXIT_SUCCESS);
}

void QtWengoPhone::addContact() {
	QtAddContact * qtAddContact = new QtAddContact(_cWengoPhone, _wengoPhoneWindow);
	LOG_DEBUG("add contact");
}

void QtWengoPhone::showConfig() {
}

void QtWengoPhone::showForum() {
}

void QtWengoPhone::showHelp() {
}

void QtWengoPhone::showAbout() {
	static QtAbout * aboutWindow = new QtAbout(this);

	aboutWindow->getWidget()->show();
}

void QtWengoPhone::showHome() {
}

void QtWengoPhone::showContactList() {
}

void QtWengoPhone::showCallHistory() {
}
