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

#include "QtToolBar.h"

#include "ui_WengoPhoneWindow.h"

#include "QtWengoPhone.h"
#include "QtAbout.h"
#include "profile/QtProfileDetails.h"
#include "profile/QtUserProfileHandler.h"
#include "config/QtWengoConfigDialog.h"
#include "webservices/sms/QtSms.h"
#include "webservices/directory/QtWsDirectory.h"
#include "phonecall/QtContactCallListWidget.h"
#include "phonecall/QtPhoneCall.h"
#include "imaccount/QtIMAccountManager.h"
#include "contactlist/QtContactList.h"
#include "filetransfer/QtFileTransfer.h"
#include "filetransfer/QtFileTransferWidget.h"

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>
#include <control/history/CHistory.h>

#include <model/contactlist/ContactProfile.h>
#include <model/phonecall/ConferenceCall.h>
#include <model/account/wengo/WengoAccount.h>
#include <model/profile/UserProfile.h>
#include <model/webservices/url/WsUrl.h>
#include <model/history/History.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/Logger.h>

#include <qtutil/LanguageChangeEventFilter.h>
#include <qtutil/WidgetBackgroundImage.h>
#include <qtutil/SafeConnect.h>
#include <qtutil/Object.h>
#include <qtutil/WidgetFactory.h>

#include <QtGui/QtGui>

QtToolBar::QtToolBar(QtWengoPhone & qtWengoPhone, Ui::WengoPhoneWindow * qtWengoPhoneUi, QWidget * parent)
	: QObject(parent),
	_qtWengoPhone(qtWengoPhone),
	_cWengoPhone(_qtWengoPhone.getCWengoPhone()) {

	_ui = qtWengoPhoneUi;


	LANGUAGE_CHANGE();

	//menuWengo
	SAFE_CONNECT(_ui->actionShowWengoAccount, SIGNAL(triggered()), SLOT(showWengoAccount()));
	SAFE_CONNECT(_ui->actionEditMyProfile, SIGNAL(triggered()), SLOT(editMyProfile()));
	SAFE_CONNECT(_ui->actionLogOff, SIGNAL(triggered()), SLOT(logOff()));

	QtUserProfileHandler * qtUserProfileHandler =
		(QtUserProfileHandler *) _cWengoPhone.getCUserProfileHandler().getPresentation();
	SAFE_CONNECT_RECEIVER(_ui->actionChangeProfile, SIGNAL(triggered()), qtUserProfileHandler, SLOT(showLoginWindow()));

	SAFE_CONNECT(_ui->actionWengoServices, SIGNAL(triggered()), SLOT(showWengoServices()));
	SAFE_CONNECT_RECEIVER(_ui->actionClose, SIGNAL(triggered()), &_qtWengoPhone, SLOT(closeWindow()));

	//menuContacts
	SAFE_CONNECT(_ui->actionAddContact, SIGNAL(triggered()), SLOT(addContact()));
	SAFE_CONNECT(_ui->actionSearchWengoContact, SIGNAL(triggered()), SLOT(searchWengoContact()));
	SAFE_CONNECT(_ui->actionShowHideOfflineContacts, SIGNAL(triggered()), SLOT(showHideOfflineContacts()));
	SAFE_CONNECT(_ui->actionShowHideContactGroups, SIGNAL(triggered()), SLOT(showHideContactGroups()));

	//menuActions
	SAFE_CONNECT(_ui->actionCreateConferenceCall, SIGNAL(triggered()), SLOT(createConferenceCall()));
	SAFE_CONNECT(_ui->actionSendSms, SIGNAL(triggered()), SLOT(sendSms()));
	SAFE_CONNECT(_ui->actionAcceptCall, SIGNAL(triggered()), SLOT(acceptCall()));
	SAFE_CONNECT(_ui->actionHangUpCall, SIGNAL(triggered()), SLOT(hangUpCall()));
	SAFE_CONNECT(_ui->actionHoldResumeCall, SIGNAL(triggered()), SLOT(holdResumeCall()));

	//menuHelp
	SAFE_CONNECT(_ui->actionShowWengoForum, SIGNAL(triggered()), SLOT(showWengoForum()));
	SAFE_CONNECT(_ui->actionWengoFAQ, SIGNAL (triggered()), SLOT(showWengoFAQ()));
	SAFE_CONNECT(_ui->actionShowAbout, SIGNAL(triggered()), SLOT(showAbout()));

	//menuTools
	SAFE_CONNECT(_ui->actionShowIMAccountSettings, SIGNAL(triggered()), SLOT(showIMAccountSettings()));
	SAFE_CONNECT(_ui->actionShowVolumePanel, SIGNAL(triggered()), SLOT(expandVolumePanel()));
	SAFE_CONNECT(_ui->actionShowConfig, SIGNAL(triggered()), SLOT(showConfig()));
	SAFE_CONNECT(_ui->actionShowFileTransfer, SIGNAL(triggered()), SLOT(showFileTransferWindow()));

	//menuClearHistory
	SAFE_CONNECT(_ui->actionClearOutgoingCalls, SIGNAL(triggered()), SLOT(clearHistoryOutgoingCalls()));
	SAFE_CONNECT(_ui->actionClearIncomingCalls, SIGNAL(triggered()), SLOT(clearHistoryIncomingCalls()));
	SAFE_CONNECT(_ui->actionClearMissedCalls, SIGNAL(triggered()), SLOT(clearHistoryMissedCalls()));
	SAFE_CONNECT(_ui->actionClearChatSessions, SIGNAL(triggered()), SLOT(clearHistoryChatSessions()));
	SAFE_CONNECT(_ui->actionClearSMS, SIGNAL(triggered()), SLOT(clearHistorySms()));
	SAFE_CONNECT(_ui->actionClearAll, SIGNAL(triggered()), SLOT(clearHistoryAll()));

	//Open chat window button
	_ui->actionOpenChatWindow->setEnabled(false);
	SAFE_CONNECT(_ui->actionOpenChatWindow, SIGNAL(triggered()), SLOT(showChatWindow()));
}

QtToolBar::~QtToolBar() {
}

QWidget * QtToolBar::getWidget() const {
	return _qtWengoPhone.getWidget();
}

void QtToolBar::showWengoAccount() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		//FIXME should not be called when no UserProfile set
		WsUrl::showWengoAccount();
	}
}

void QtToolBar::editMyProfile() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		QWidget * parent = qobject_cast<QWidget *>(sender()->parent());

		//FIXME this method should not be called if no UserProfile has been set
		QtProfileDetails qtProfileDetails(*_cWengoPhone.getCUserProfileHandler().getCUserProfile(),
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile(), parent);

		//TODO UserProfile must be updated if QtProfileDetails was accepted
		qtProfileDetails.show();
	}
}

void QtToolBar::addContact() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		QWidget * parent = qobject_cast<QWidget *>(sender()->parent());

		//FIXME this method should not be called if no UserProfile has been set
		ContactProfile contactProfile;
		QtProfileDetails qtProfileDetails(*_cWengoPhone.getCUserProfileHandler().getCUserProfile(),
				contactProfile, parent);
		if (qtProfileDetails.show()) {
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().addContact(contactProfile);
		}
	}
}

void QtToolBar::showConfig() {
	QWidget * parent = qobject_cast<QWidget *>(sender()->parent());
	QtWengoConfigDialog dialog(_cWengoPhone, parent);
	dialog.show();
}

void QtToolBar::showWengoForum() {
	WsUrl::showWengoForum();
}

void QtToolBar::showWengoServices() {
	WsUrl::showWengoCallOut();
}

void QtToolBar::searchWengoContact() {
	QtWsDirectory * qtWsDirectory = _qtWengoPhone.getQtWsDirectory();
	if (qtWsDirectory) {
		QWidget * parent = qobject_cast<QWidget *>(sender()->parent());
		qtWsDirectory->getWidget()->setParent(parent);
		qtWsDirectory->show();
	}
}

void QtToolBar::showAbout() {
	QWidget * parent = qobject_cast<QWidget *>(sender()->parent());
	static QtAbout * qtAbout = new QtAbout(parent);
	qtAbout->getWidget()->show();
}

void QtToolBar::sendSms() {
	QtSms * qtSms = _qtWengoPhone.getQtSms();
	if (qtSms) {
		QWidget * parent = qobject_cast<QWidget *>(sender()->parent());
		qtSms->getWidget()->setParent(parent);
		qtSms->getWidget()->show();
	}
}

//FIXME hack hack hack hack tired of it
void QtToolBar::createConferenceCall() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		QWidget * parent = qobject_cast<QWidget *>(sender()->parent());

		QDialog * conferenceDialog = qobject_cast<QDialog *>(WidgetFactory::create(":/forms/phonecall/ConferenceCallWidget.ui", parent));

		QLabel * conferenceLabel = Object::findChild<QLabel *>(conferenceDialog, "conferenceLabel");
		WidgetBackgroundImage::setBackgroundImage(conferenceLabel, ":pics/headers/conference.png", true);

		int ret = conferenceDialog->exec();

		QLineEdit * phoneNumber1LineEdit = Object::findChild<QLineEdit *>(conferenceDialog, "phoneNumber1LineEdit");
		QLineEdit * phoneNumber2LineEdit = Object::findChild<QLineEdit *>(conferenceDialog, "phoneNumber2LineEdit");

		if (ret == QDialog::Accepted) {
			IPhoneLine * phoneLine = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getActivePhoneLine();

			if (phoneLine != NULL) {
				ConferenceCall * confCall = new ConferenceCall(*phoneLine);
				confCall->addPhoneNumber(phoneNumber1LineEdit->text().toStdString());
				confCall->addPhoneNumber(phoneNumber2LineEdit->text().toStdString());
			} else {
				LOG_DEBUG("phoneLine is NULL");
			}
		}
	}
}
//!FIXME

void QtToolBar::showIMAccountSettings() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		QWidget * parent = qobject_cast<QWidget *>(sender()->parent());

		//FIXME IMAccountManager must not use UserProfile but only CUserProfile
		QtIMAccountManager imAccountManager(_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile(),
			true, parent);
	}
}

void QtToolBar::showWengoFAQ() {
	WsUrl::showWengoFAQ();
}

void QtToolBar::showHideOfflineContacts() {
	QtContactList * qtContactList = _qtWengoPhone.getQtContactList();
	if (qtContactList) {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		config.set(Config::GENERAL_SHOW_OFFLINE_CONTACTS_KEY, config.getShowOfflineContacts());
		qtContactList->hideOffLineContacts();
	}
}

void QtToolBar::showHideContactGroups() {
	QtContactList * qtContactList = _qtWengoPhone.getQtContactList();
	if (qtContactList) {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		config.set(Config::GENERAL_SHOW_GROUPS_KEY, config.getShowGroups());
		qtContactList->showHideGroups();
	}
}

void QtToolBar::clearHistoryOutgoingCalls() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::OutgoingCall);
	}
}

void QtToolBar::clearHistoryIncomingCalls() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::IncomingCall);
	}
}

void QtToolBar::clearHistoryMissedCalls() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::MissedCall);
	}
}

void QtToolBar::clearHistoryChatSessions() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::ChatSession);
	}
}

void QtToolBar::clearHistorySms() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::OutgoingSmsOk);
	}
}

void QtToolBar::clearHistoryAll() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::Any);
	}
}

void QtToolBar::logOff() {
	_cWengoPhone.getCUserProfileHandler().setCurrentUserProfile(String::null, WengoAccount::empty);
}

int QtToolBar::findFirstCallTab() {
	QtContactCallListWidget * widget;
	for (int i = 0; i < _ui->tabWidget->count(); i++) {
		widget = (QtContactCallListWidget *) _ui->tabWidget->widget(i);
		if (widget) {
			return i;
		}
	}
	return -1;
}

void QtToolBar::acceptCall() {
	int callIndex = findFirstCallTab();
	if (callIndex == -1) {
		return;
	}
	QtContactCallListWidget * widget = (QtContactCallListWidget *) _ui->tabWidget->widget(callIndex);
	if (!widget) {
		return;
	}

	QtPhoneCall * qtPhoneCall = widget->getFirstQtPhoneCall();
	if (qtPhoneCall) {
		qtPhoneCall->acceptActionTriggered(true);
	}
}

void QtToolBar::holdResumeCall() {
	int callIndex = findFirstCallTab();
	if (callIndex == -1) {
		return;
	}
	QtContactCallListWidget * widget = (QtContactCallListWidget *) _ui->tabWidget->widget(callIndex);
	if (!widget) {
		return;
	}

	QtPhoneCall * qtPhoneCall = widget->getFirstQtPhoneCall();
	if (qtPhoneCall) {
		qtPhoneCall->holdResumeActionTriggered(true);
	}
}

void QtToolBar::hangUpCall() {
	int callIndex = findFirstCallTab();
	if (callIndex == -1) {
		return;
	}
	QtContactCallListWidget * widget;

	widget = (QtContactCallListWidget *) _ui->tabWidget->widget(callIndex);
	if (!widget) {
		return;
	}
	widget->hangup();
}

void QtToolBar::expandVolumePanel() {
	static bool expand = true;

	if (expand) {
		_ui->configPanel->show();
	} else {
		_ui->configPanel->hide();
	}
	expand = !expand;
}

void QtToolBar::showChatWindow() {
	QWidget * chatWindow = _qtWengoPhone.getChatWindow();
	if (chatWindow) {
		chatWindow->showNormal();
		chatWindow->activateWindow();
	}
}

void QtToolBar::showFileTransferWindow() {
	static bool show = true;

	QtFileTransfer * fileTransfer = _qtWengoPhone.getFileTransfer();
	if (fileTransfer) {
		if (show) {
			fileTransfer->getFileTransferWidget()->show();
		} else {
			fileTransfer->getFileTransferWidget()->hide();
		}
	}
	show = !show;
}

void QtToolBar::languageChanged() {
#if defined(OS_MACOSX)
	// Avoids translation of these menus on Mac OS X. Thus Qt
	// will put these under the Application menu
	_ui->actionShowConfig->setText("Preferences");
	_ui->actionShowAbout->setText("About");
#endif
}
