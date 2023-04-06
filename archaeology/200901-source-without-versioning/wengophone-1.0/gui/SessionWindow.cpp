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

#include "SessionWindow.h"

#include "DialPadWidget.h"
#include "contact/ContactWindow.h"
#include "contact/ContactWidget.h"
#include "contact/ContactList.h"
#include "contact/Contact.h"
#include "util/emit_signal.h"
#include "MyWidgetFactory.h"
#include "Softphone.h"
#include "AudioCall.h"
#include "sip/SipAddress.h"
#include "MainWindow.h"
#include "util/EventFilter.h"

#include <qmainwindow.h>
#include <qstring.h>
#include <qapplication.h>
#include <qdialog.h>
#include <qwidgetstack.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qstatusbar.h>
#include <qcombobox.h>
#include <qlayout.h>

#include <iostream>
using namespace std;

#include <cassert>

const int SessionWindow::INDEX_PHONE_PAGE = 0;
const int SessionWindow::INDEX_VIDEO_PAGE = 1;
const int SessionWindow::INDEX_CHAT_PAGE = 2;
const int SessionWindow::INDEX_SMS_PAGE = 3;

//VisioPageWidget * SessionWindow::_visioPageWidget = 0;

SessionWindow::SessionWindow(AudioCall & audioCall, QWidget * parent)
: QObject(parent), _audioCall(audioCall) {
	_contact = NULL;

	_currentPageIndex = INDEX_PHONE_PAGE;

	_sessionWindow = (QMainWindow *) MyWidgetFactory::create("SessionWindowForm.ui", parent);
	_sessionWindow->reparent(0,
				WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu |
				WStyle_MinMax /*| WStyle_StaysOnTop*/ /*| WX11BypassWM*/,
				_sessionWindow->pos());

	//ResizeEvent filter
	ResizeEventFilter * resizeEvent = new ResizeEventFilter(this, SLOT(resize()));
	_sessionWindow->installEventFilter(resizeEvent);

	//CloseEvent filter
	CloseEventFilter * closeEventFilter = new CloseEventFilter(this, SLOT(closeWindow()));
	_sessionWindow->installEventFilter(closeEventFilter);

	//Closes the current communication before to quit the application
	connect(&Softphone::getInstance().getQApplication(), SIGNAL(aboutToQuit()),
		this, SLOT(closeWindow()));

	//PhonePageWidget
	_phonePageWidget = new PhonePageWidget(this);
	QWidgetStack * phoneWidgetStack = (QWidgetStack *) _sessionWindow->child("phoneWidgetStack",
										"QWidgetStack");
	phoneWidgetStack->addWidget(_phonePageWidget->getWidget());
	phoneWidgetStack->raiseWidget(_phonePageWidget->getWidget());


	//ChatPageWidget
	_chatPageWidget = new ChatPageWidget(_sessionWindow);
	QWidgetStack * chatWidgetStack = (QWidgetStack *) _sessionWindow->child("chatWidgetStack",
										"QWidgetStack");
	chatWidgetStack->addWidget(_chatPageWidget->getWidget());
	chatWidgetStack->raiseWidget(_chatPageWidget->getWidget());

#ifdef ENABLE_VIDEO
	_visioPageWidget = new VisioPageWidget(this, _audioCall);
	QWidgetStack * visioWidgetStack = (QWidgetStack *) _sessionWindow->child("visioWidgetStack", "QWidgetStack");
	visioWidgetStack->addWidget(_visioPageWidget->getWidget());
	visioWidgetStack->raiseWidget(_visioPageWidget->getWidget());
#endif

	//SmsPageWidget
	_smsPageWidget = new SmsPageWidget(_sessionWindow);
	QWidgetStack * smsWidgetStack = (QWidgetStack *) _sessionWindow->child("smsWidgetStack",
										"QWidgetStack");
	smsWidgetStack->addWidget(_smsPageWidget->getWidget());
	smsWidgetStack->raiseWidget(_smsPageWidget->getWidget());


	//Change page connection
	_tabWidget = (QTabWidget *) _sessionWindow->child("tabWidget", "QTabWidget");
	connect(_tabWidget, SIGNAL(currentChanged(QWidget *)),
		this, SLOT(pageChanged(QWidget *)));

	//Disables some pages by default
	_tabWidget->setTabEnabled(_tabWidget->page(INDEX_SMS_PAGE), true);
	_tabWidget->setTabEnabled(_tabWidget->page(INDEX_CHAT_PAGE), true);
#ifdef ENABLE_VIDEO
	_tabWidget->setTabEnabled(_tabWidget->page(INDEX_VIDEO_PAGE), true);
#else
	_tabWidget->setTabEnabled(_tabWidget->page(INDEX_VIDEO_PAGE), false);
#endif
	_sessionWindow->move(50, 50);

	//_tabWidget->setCurrentPage(INDEX_VIDEO_PAGE);
#ifdef ENABLE_VIDEO
	_visioPageWidget->setSessionWindow(_sessionWindow);
	_visioPageWidget->repack();
#endif
	_smsPageSize = _sessionWindow->size();
	_chatPageSize = _sessionWindow->size();
	_videoPageSize = _sessionWindow->size();

#ifndef WIN32	
        _sessionWindow->setIcon(QPixmap::fromMimeSource("logo-spiral.png"));
#endif
}

SessionWindow::~SessionWindow() {
	/*delete _phonePageWidget;
	delete _contactWidget;
	delete _tabWidget;
	delete _sessionWindow;*/
}

void SessionWindow::setContact(const QString & phoneNumber, const QString & displayName, Contact * contact) {
	QString realPhoneNumber = phoneNumber;
	_contact = contact;

	if (_contact == NULL) {
		//Try to get the Contact just from the phone number
		_contact = ContactList::getInstance().getContact(realPhoneNumber);
		if (_contact == NULL) {
			_phonePageWidget->reset();
			_chatPageWidget->reset();
			_contact = new Contact();
			if (!displayName.isEmpty()) {
				realPhoneNumber = displayName;
			}
			_contact->setHomePhone(realPhoneNumber);
		}
	}

	if (!_contact->toString().isEmpty()) {
		//SessionWindow caption
		_sessionWindow->setCaption(_contact->toString());
	}

	_phonePageWidget->setContact(*_contact);
	_smsPageWidget->setContact(realPhoneNumber, *_contact);
	_chatPageWidget->setContact(*_contact);
	_phonePageWidget->setCallerPhoneNumber(realPhoneNumber);
}

void SessionWindow::showOutgoingCallPhonePage(const SipAddress & sipAddress, Contact * contact) {
	if (!Softphone::getInstance().isEnabled()) {
		return;
	}
	
	if (!_sessionWindow->isVisible() || !_audioCall.isCalling()) {
		setContact(sipAddress.getValidPhoneNumber(), QString::null, contact);
		_phonePageWidget->setPhoneNumberToCall(sipAddress.getValidPhoneNumber());
		_phonePageWidget->setCallerPhoneNumber(QString::null);

		//Do not show the SessionWindow anymore if the MainWindow is not visible
		bool mainWindowVisible = Softphone::getInstance().getMainWindow().getWidget()->isVisible();
		if (mainWindowVisible) {
			_tabWidget->setCurrentPage(INDEX_PHONE_PAGE);
			_sessionWindow->show();
			_sessionWindow->raise();
		}
		
		//Calls the Contact directly
		_phonePageWidget->call();
	}
}

Contact * SessionWindow::showIncomingCallPhonePage(const SipAddress & sipAddress) {
	setContact(sipAddress.getValidPhoneNumber(), sipAddress.getDisplayName(), NULL);
	_phonePageWidget->setPhoneNumberToCall(QString::null);

	//Do not show the SessionWindow anymore if the MainWindow is not visible
	bool mainWindowVisible = Softphone::getInstance().getMainWindow().getWidget()->isVisible();
	if (mainWindowVisible || _sessionWindow->isVisible()) {
		_tabWidget->setCurrentPage(INDEX_PHONE_PAGE);
		_sessionWindow->show();
	}

	return _contact;
}

void SessionWindow::showCurrentCallPhonePage() {
	Softphone::getInstance().getMainWindow().getWidget()->showNormal();
	_sessionWindow->setActiveWindow();
	_sessionWindow->showNormal();
}

void SessionWindow::showEmptyDefaultPage() {
	if (!Softphone::getInstance().isEnabled()) {
		return;
	}

	if (!_sessionWindow->isVisible()) {
		_contact = NULL;
		setContact(QString::null, QString::null, _contact);
	}
	_tabWidget->setCurrentPage(INDEX_PHONE_PAGE);
}

void SessionWindow::showVisioPage() {
	_tabWidget->setCurrentPage(INDEX_VIDEO_PAGE);
}

void SessionWindow::pageChanged(QWidget * currentPage) {
	//FIXME why this method is being called several times in a row?
	int pageIndex = _tabWidget->indexOf(currentPage);

	_currentPageIndex = pageIndex;

	switch(pageIndex) {
	case INDEX_PHONE_PAGE:
		_sessionWindow->resize(_sessionWindow->minimumSize());

		_phonePageWidget->setFocus();
#ifdef ENABLE_VIDEO
		_visioPageWidget->recalculateSizes();
#endif
		break;

	case INDEX_SMS_PAGE:
		_sessionWindow->resize(_smsPageSize);

		//FIXME Ugly trick so that it loads the SMS signature
		_smsPageWidget->show(NULL);

		_smsPageWidget->setFocus();
		break;

	case INDEX_CHAT_PAGE:
		_sessionWindow->resize(_chatPageSize);

		_chatPageWidget->setFocus();
		break;

	case INDEX_VIDEO_PAGE: {
		_sessionWindow->resize(_videoPageSize);

#ifdef ENABLE_VIDEO
		QWidgetStack * visioWidgetStack = (QWidgetStack *) _sessionWindow->child("visioWidgetStack", "QWidgetStack");
		visioWidgetStack->addWidget(_visioPageWidget->getWidget());
		visioWidgetStack->raiseWidget(_visioPageWidget->getWidget());
		_visioPageWidget->setSessionWindow(_sessionWindow);
		_visioPageWidget->repack();
#endif
		break;
	}

	default:
		assert(NULL && "The name of the page is not valid");
	}

	//Trick so that the SessionWindow does not
	//appear on top of the other windows and applications.
	//setActiveWindow() from Qt does not work properly
	//on some computers.
	if (!_sessionWindow->isVisible()) {
		QPoint oldPos = _sessionWindow->pos();

		int desktopWidth = QApplication::desktop()->width();
		int desktopHeight = QApplication::desktop()->height();
		_sessionWindow->move(desktopWidth, desktopHeight);

		_sessionWindow->show();
		_sessionWindow->setWindowState(WindowMinimized);

		_sessionWindow->setWindowState(WindowNoState);
		_sessionWindow->move(oldPos);
	}

	//Does not work properly
	//Replaced by flashWindow()
	//_sessionWindow->setActiveWindow();

	flashWindow();
}

void SessionWindow::editContact() {
	ContactWindow * contactWindow = new ContactWindow(_sessionWindow, _contact);
	contactWindow->execShowContact();
}

void SessionWindow::closeWindow() {
	emit_signal(_phonePageWidget->getHangUpButton(), SIGNAL(clicked()));
	_sessionWindow->hide();
	_chatPageWidget->hideEmoticons();
}

void SessionWindow::sendSms(const QString & phoneNumber, const QString & message, Contact * contact) {
	setContact(phoneNumber, QString::null, contact);
	_smsPageWidget->setMessage(message);
	_tabWidget->setCurrentPage(INDEX_SMS_PAGE);
	_sessionWindow->show();
}

void SessionWindow::startChat(Contact & contact, const QString & messageReceived) {
	setContact(QString::null, QString::null, &contact);
	if (!messageReceived.isEmpty()) {
		_chatPageWidget->appendMessageToHistory(messageReceived);
	}
	_tabWidget->setCurrentPage(INDEX_CHAT_PAGE);
	_sessionWindow->show();
}

void SessionWindow::flashWindow() {
#ifdef WIN32
	FLASHWINFO flashInfo;
	flashInfo.cbSize = sizeof(FLASHWINFO);
	flashInfo.hwnd = _sessionWindow->winId();
	flashInfo.dwFlags = FLASHW_TRAY | FLASHW_TIMERNOFG;
	flashInfo.uCount = 5;
	flashInfo.dwTimeout = 500;

	FlashWindowEx(&flashInfo);
#endif	//WIN32
}

void SessionWindow::showSmsPage() {
	_smsPageWidget->setFocus();
	_sessionWindow->show();
	_tabWidget->setCurrentPage(INDEX_SMS_PAGE);
}

void SessionWindow::resize() {
	switch(_currentPageIndex) {
	case INDEX_PHONE_PAGE:
		break;

	case INDEX_SMS_PAGE:
		//_smsPageSize = _sessionWindow->size();
		break;

	case INDEX_CHAT_PAGE:
		_chatPageSize = _sessionWindow->size();
		break;

	case INDEX_VIDEO_PAGE:
		_videoPageSize = _sessionWindow->size();
		break;

	default:
		assert(NULL && "The name of the page is not valid");
	}
}
