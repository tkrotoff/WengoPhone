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

#include "QtWengoPhone.h"

#include <model/account/SipAccount.h>
#include <model/phonecall/PhoneCall.h>
#include <model/connect/ConnectHandler.h>
#include <control/CWengoPhone.h>

#include <imwrapper/EnumIMProtocol.h>

#include "phoneline/QtPhoneLine.h"
#include "phonecall/QtPhoneCall.h"
#include "QtLogger.h"
#include "login/QtLogin.h"
#include "login/QtSetLogin.h"
#include "contactlist/QtContactList.h"
#include "contactlist/QtAddContact.h"
#include "QtDialpad.h"
#include "QtAbout.h"
#include "sms/QtSms.h"
#include "QtHttpProxyLogin.h"
#include "config/QtAdvancedConfig.h"

#include <WidgetFactory.h>
#include <Object.h>
#include <Thread.h>
#include <Logger.h>

#include <global.h>

#include <QtGui>

using namespace std;

QtWengoPhone::QtWengoPhone(CWengoPhone & cWengoPhone)
	: QObjectThreadSafe(),
	_cWengoPhone(cWengoPhone) {

	_qtSms = NULL;

	_cWengoPhone.loginStateChangedEvent +=
		boost::bind(&QtWengoPhone::loginStateChangedEventHandler, this, _1, _2);
	_cWengoPhone.noAccountAvailableEvent +=
		boost::bind(&QtWengoPhone::noAccountAvailableEventHandler, this, _1);
	_cWengoPhone.proxyNeedsAuthenticationEvent +=
		boost::bind(&QtWengoPhone::proxyNeedsAuthenticationEventHandler, this, _1, _2, _3);
	_cWengoPhone.wrongProxyAuthenticationEvent +=
		boost::bind(&QtWengoPhone::wrongProxyAuthenticationEventHandler, this, _1, _2, _3, _4, _5);

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::initThreadSafe, this));
	postEvent(event);
}

void QtWengoPhone::initThreadSafe() {
#ifndef OS_MACOSX
	QApplication::setStyle(new QPlastiqueStyle);
#endif
	_wengoPhoneWindow = qobject_cast<QMainWindow *>(WidgetFactory::create(":/forms/WengoPhoneWindow.ui", NULL));

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

	//logger
	//FIXME no more logger tab widget
	/*QtLogger * qtLogger = new QtLogger(_wengoPhoneWindow);
	QWidget * tabLogger = Object::findChild<QWidget *>(_tabWidget, "tabLogger");
	createLayout(tabLogger)->addWidget(qtLogger->getWidget());*/

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

	//actionSetLogin
	QAction * actionSetLogin = Object::findChild<QAction *>(_wengoPhoneWindow, "actionSetLogin");
	connect(actionSetLogin, SIGNAL(triggered()), SLOT(actionSetLogin()));

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

	//actionSendSms
	QAction * actionSendSms = Object::findChild<QAction *>(_wengoPhoneWindow, "actionSendSms");
	connect(actionSendSms, SIGNAL(triggered()), SLOT(sendSms()));

	//actionAdvancedConfiguration
	QAction * actionAdvancedConfiguration = Object::findChild<QAction *>(_wengoPhoneWindow, "actionAdvancedConfiguration");
	connect(actionAdvancedConfiguration, SIGNAL(triggered()), SLOT(showAdvancedConfig()));

#if QT_VERSION == 0x040100
	//FIXME
	//QT 4.1.1 correctly creates following stuff from QTDesigner specs
	//QT 4.1.0 obliges us to create them explicitly

	//centralwidget
	QWidget * centralwidget = Object::findChild<QWidget *>(_wengoPhoneWindow, "centralwidget");
	_wengoPhoneWindow->setCentralWidget(centralwidget);

	//menubar
	QMenuBar * menubar = Object::findChild<QMenuBar *>(_wengoPhoneWindow, "menubar");
	_wengoPhoneWindow->setMenuBar(menubar);

	//statusbar
	QStatusBar * statusbar = Object::findChild<QStatusBar *>(_wengoPhoneWindow, "statusbar");
	_wengoPhoneWindow->setStatusBar(statusbar);
#endif

	_wengoPhoneWindow->show();
}

void QtWengoPhone::initButtons() {
	//callButton
	_callButton->disconnect();
	connect(_callButton, SIGNAL(clicked()), SLOT(callButtonClicked()));
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

void QtWengoPhone::addPhoneCall(QtPhoneCall * qtPhoneCall) {
	static QWidget * tabPhoneCall = Object::findChild<QWidget *>(_tabWidget, "tabPhoneCall");
	static QGridLayout * layout = new QGridLayout(tabPhoneCall);

	layout->addWidget(qtPhoneCall->getWidget());
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

void QtWengoPhone::setContactList(QtContactList * qtContactList) {
	QWidget * tabContactList = Object::findChild<QWidget *>(_tabWidget, "tabContactList");
	createLayout(tabContactList)->addWidget(qtContactList->getWidget());
	LOG_DEBUG("QtContactList added");
}

void QtWengoPhone::setSms(QtSms * qtSms) {
	_qtSms = qtSms;
}

void QtWengoPhone::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtWengoPhone::updatePresentationThreadSafe() {
}

void QtWengoPhone::loginStateChangedEventHandler(SipAccount & sender, SipAccount::LoginState state) {
	typedef PostEvent2<void (SipAccount & sender, SipAccount::LoginState), SipAccount &, SipAccount::LoginState> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::loginStateChangedEventHandlerThreadSafe, this, _1, _2), sender, state);
	postEvent(event);
}

void QtWengoPhone::loginStateChangedEventHandlerThreadSafe(SipAccount & sender, SipAccount::LoginState state) {
	switch (state) {
	case SipAccount::LoginStateReady:
		break;

	case SipAccount::LoginStateConnected:
		break;

	case SipAccount::LoginStateDisconnected:
		break;

	case SipAccount::LoginStatePasswordError:
		break;

	case SipAccount::LoginStateNetworkError:
		break;

	default:
		LOG_FATAL("Unknown state");
	};
}

void QtWengoPhone::noAccountAvailableEventHandler(WengoPhone & sender) {
	typedef PostEvent1<void (WengoPhone & sender), WengoPhone &> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::noAccountAvailableEventHandlerThreadSafe, this, _1), sender);
	postEvent(event);
}

void QtWengoPhone::noAccountAvailableEventHandlerThreadSafe(WengoPhone & sender) {
	showLoginWindow();
}

void QtWengoPhone::dialpad(const std::string & tone, const std::string & soundFile) {
	PhoneCall * phoneCall = _cWengoPhone.getActivePhoneCall();
	if (phoneCall) {
		if (soundFile.empty()) {
			if (tone == "0") { phoneCall->playTone(EnumTone::Tone0); }
			if (tone == "1") { phoneCall->playTone(EnumTone::Tone1); }
			if (tone == "2") { phoneCall->playTone(EnumTone::Tone2); }
			if (tone == "3") { phoneCall->playTone(EnumTone::Tone3); }
			if (tone == "4") { phoneCall->playTone(EnumTone::Tone4); }
			if (tone == "5") { phoneCall->playTone(EnumTone::Tone5); }
			if (tone == "6") { phoneCall->playTone(EnumTone::Tone6); }
			if (tone == "7") { phoneCall->playTone(EnumTone::Tone7); }
			if (tone == "8") { phoneCall->playTone(EnumTone::Tone8); }
			if (tone == "9") { phoneCall->playTone(EnumTone::Tone9); }
			if (tone == "*") { phoneCall->playTone(EnumTone::ToneStar); }
			if (tone == "#") { phoneCall->playTone(EnumTone::TonePound); }
		} else {
			phoneCall->playSoundFile(soundFile);
		}
	} else {
		_phoneComboBox->setEditText(_phoneComboBox->currentText() + QString::fromStdString(tone));
	}
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
	_wengoPhoneWindow->hide();
	_cWengoPhone.terminate();
	QCoreApplication::exit(EXIT_SUCCESS);
}

void QtWengoPhone::addContact() {
	QtAddContact * qtAddContact = new QtAddContact(_cWengoPhone, _wengoPhoneWindow);
	LOG_DEBUG("add contact");
}

void QtWengoPhone::actionSetLogin() {
	QtSetLogin * qtSetLogin = new QtSetLogin(_wengoPhoneWindow);

	qtSetLogin->exec();

	EnumIMProtocol::IMProtocol protocol;
	string selProtocol = qtSetLogin->getProtocol();
	string login = qtSetLogin->getLogin();
	string password = qtSetLogin->getPassword();

	if (selProtocol == "MSN") {
		protocol = EnumIMProtocol::IMProtocolMSN;
	} else if (selProtocol == "Yahoo") {
		protocol = EnumIMProtocol::IMProtocolYahoo;
	} else if (selProtocol == "Jabber") {
		protocol = EnumIMProtocol::IMProtocolJabber;
	}

	_cWengoPhone.addIMAccount(login, password, protocol);
	_cWengoPhone.getWengoPhone().getConnectHandler().connect(IMAccount(login, password, protocol));
	LOG_DEBUG("set login");
}

void QtWengoPhone::showConfig() {
}

void QtWengoPhone::showForum() {
}

void QtWengoPhone::showHelp() {
}

void QtWengoPhone::showAbout() {
	static QtAbout * aboutWindow = new QtAbout(_wengoPhoneWindow);

	aboutWindow->getWidget()->show();
}

void QtWengoPhone::showHome() {
}

void QtWengoPhone::showContactList() {
}

void QtWengoPhone::showCallHistory() {
}

void QtWengoPhone::sendSms() {
	if (_qtSms) {
		_qtSms->getWidget()->show();
	}
}

void QtWengoPhone::showAdvancedConfig() {
	static QtAdvancedConfig * configWindow = new QtAdvancedConfig(_wengoPhoneWindow);

	configWindow->populate();
	configWindow->getWidget()->show();
}

void QtWengoPhone::wrongProxyAuthenticationEventHandler(SipAccount & sender,
		const std::string & proxyAddress, unsigned proxyPort,
		const std::string & proxyLogin, const std::string & proxyPassword) {

	typedef PostEvent3<void (SipAccount & sender, const std::string & proxyAddress, int proxyPort),
			SipAccount &, std::string, unsigned> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::proxyNeedsAuthenticationEventHandlerThreadSafe, this, _1, _2, _3),
				sender, proxyAddress, proxyPort);
	postEvent(event);
}

void QtWengoPhone::proxyNeedsAuthenticationEventHandler(SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort) {
	typedef PostEvent3<void (SipAccount & sender, const std::string & proxyAddress, int proxyPort),
			SipAccount &, std::string, unsigned> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::proxyNeedsAuthenticationEventHandlerThreadSafe, this, _1, _2, _3),
				sender, proxyAddress, proxyPort);
	postEvent(event);
}

void QtWengoPhone::proxyNeedsAuthenticationEventHandlerThreadSafe(SipAccount & sender,
		const std::string & proxyAddress, unsigned proxyPort) {

	static QtHttpProxyLogin * httpProxy = new QtHttpProxyLogin(_wengoPhoneWindow, proxyAddress, proxyPort);

	int ret = httpProxy->exec();

	if (ret == QDialog::Accepted) {
		sender.setProxySettings(httpProxy->getProxyAddress(), httpProxy->getProxyPort(),
			httpProxy->getLogin(), httpProxy->getPassword());
	}
}
