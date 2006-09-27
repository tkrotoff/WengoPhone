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

#include "QtPhoneCall.h"

#include "ui_PhoneCallWidget.h"

#ifdef XV_HWACCEL
	#include "QtVideoXV.h"
#endif

#include "QtVideoQt.h"
#include "QtPhoneCallEventFilter.h"
#include "../toaster/QtCallToaster.h"

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/statusbar/QtStatusBar.h>

#include <control/phonecall/CPhoneCall.h>
#include <control/profile/CUserProfileHandler.h>
#include <control/CWengoPhone.h>

#include <model/phonecall/PhoneCall.h>
#include <model/phonecall/ConferenceCall.h>
#include <model/phoneline/PhoneLine.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <sipwrapper/CodecList.h>
#include <sipwrapper/SipWrapper.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

QtPhoneCall::QtPhoneCall(CPhoneCall & cPhoneCall)
	: QObject(NULL),
	_cPhoneCall(cPhoneCall) {

	_qtWengoPhone = (QtWengoPhone *) _cPhoneCall.getCWengoPhone().getPresentation();
	_videoWindow = NULL;

	_remoteVideoFrame = NULL;
	_localVideoFrame = NULL;
	_hold = true;
	_showVideo = false;

	_phoneCallWidget = new QWidget(_qtWengoPhone->getWidget());

	_ui = new Ui::PhoneCallWidget();
	_ui->setupUi(_phoneCallWidget);

	_phoneCallWidget->setAutoFillBackground(true);

	QString sipAddress = QString::fromStdString(_cPhoneCall.getPeerSipAddress());

	std::string tmpDisplayName = _cPhoneCall.getPhoneCall().getPeerSipAddress().getDisplayName();
	QString userInfo = QString::fromUtf8(tmpDisplayName.c_str());

	if (userInfo.isEmpty()) {
		userInfo = QString::fromStdString(_cPhoneCall.getPhoneCall().getPeerSipAddress().getUserName());
	}

	//init flip
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	PhoneLine & phoneLine = dynamic_cast < PhoneLine & > (_cPhoneCall.getPhoneCall().getPhoneLine());
	phoneLine.flipVideoImage(config.getVideoFlipEnable());
	////

	userInfo = getDisplayName(userInfo);

	QString tmp = QString("<html><head><meta name='qrichtext' content='1'/></head><body "
		"style=white-space: pre-wrap; font-family:MS Shell Dlg; font-size:8.25pt;"
		" font-weight:400; font-style:normal; text-decoration:none;'><p style=' margin-top:0px; "
		" margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; "
		"font-size:8pt;'><span style=' font-size:13pt; font-weight:600;'>%1</span></p></body></html>").arg(userInfo);

	_ui->nickNameLabel->setText(tmp);
	_ui->nickNameLabel->setToolTip(sipAddress);

	_ui->statusLabel->setText(QString::null);
	_ui->statusLabel->setToolTip(tr("Status"));

	_ui->durationLabel->setText("00:00:00");
	_ui->durationLabel->setToolTip(tr("Call Duration"));

	//Accept call
	_actionAcceptCall = new QAction(QIcon(":/pics/actions/accept-phone.png"), tr("Accept"), _phoneCallWidget);
	SAFE_CONNECT(_actionAcceptCall, SIGNAL(triggered(bool)), SLOT(acceptActionTriggered(bool)));

	//Hand-up call - Qt::QueuedConnection is needed ! Don't remove it !!!
	_actionHangupCall = new QAction(QIcon(":/pics/actions/hangup-phone.png"), tr("Hang-up"), _phoneCallWidget);
	SAFE_CONNECT_TYPE(_actionHangupCall, SIGNAL(triggered(bool)), SLOT(rejectActionTriggered(bool)), Qt::QueuedConnection);

	//Hold
	_actionHold = new QAction(tr("Hold"), _phoneCallWidget);
	SAFE_CONNECT(_actionHold, SIGNAL(triggered(bool)), SLOT(holdResumeActionTriggered(bool)));

	//Resume
	_actionResume = new QAction(tr("Resume"), _phoneCallWidget);
	SAFE_CONNECT(_actionResume, SIGNAL(triggered(bool)), SLOT(holdResumeActionTriggered(bool)));
	_actionResume->setEnabled(false);

	//Invite to conference
	_actionInvite = new QAction(tr("Invite to conference"), _phoneCallWidget);
	SAFE_CONNECT(_actionInvite, SIGNAL(triggered(bool)), SLOT(inviteToConference(bool)));
	//_actionInvite->setEnabled(false);

	//Add contact
	_actionAddContact = new QAction(QIcon(":/pics/actions/add-contact.png"), tr("Add contact"), _phoneCallWidget);
	SAFE_CONNECT(_actionAddContact, SIGNAL(triggered(bool)), SLOT(addContactActionTriggered(bool)));

	_popupMenu = createMenu();

	//Computes the call duration
	_callTimer = new QTimer(_phoneCallWidget);
	SAFE_CONNECT(_callTimer, SIGNAL(timeout()), SLOT(updateCallDuration()));

	QtPhoneCallEventFilter * filter = new QtPhoneCallEventFilter(_phoneCallWidget);
	_phoneCallWidget->installEventFilter(filter);
	SAFE_CONNECT(filter, SIGNAL(openPopup(int, int)), SLOT(openPopup(int, int)));

	showToaster(userInfo);

	if (!_cPhoneCall.getPhoneCall().getConferenceCall()) {
		_qtWengoPhone->addPhoneCall(this);
	} else {
		_qtWengoPhone->addToConference(this);
	}
}

QtPhoneCall::~QtPhoneCall() {
	if (_remoteVideoFrame) {
		pix_free(_remoteVideoFrame);
	}
	if (_localVideoFrame) {
		pix_free(_localVideoFrame);
	}
	OWSAFE_DELETE(_ui);
}

QString QtPhoneCall::getDisplayName(QString str) {
	QString tmp;

	int begin = str.indexOf("sip:", 0, Qt::CaseInsensitive);
	if (begin == -1) {
		// Not found, return ...
		return str;
	}
	begin += 4;
	int end = str.indexOf("@", begin, Qt::CaseInsensitive);
	if (end == -1) {
		//Not found, return ...
		return str;
	}
	tmp = str.mid(begin, end - begin);
	return tmp;
}

QMenu * QtPhoneCall::createMenu() const {
	QMenu * menu = new QMenu(_phoneCallWidget);

	menu->addAction(_actionAcceptCall);

	menu->addAction(_actionHangupCall);

	menu->addAction(_actionHold);

	menu->addAction(_actionResume);

	//FIXME Invite to conferente desactivated for the moment due to a crash
	//menu->addSeparator();
	//menu->addAction(_actionInvite);

	menu->addSeparator();

	menu->addAction(_actionAddContact);

//	menu->addAction(tr("Block contact"));

	return menu;
}

QMenu * QtPhoneCall::createInviteMenu() const {
	PhoneLine & phoneLine = dynamic_cast < PhoneLine & > (_cPhoneCall.getPhoneCall().getPhoneLine());

	PhoneLine::PhoneCallList phoneCallList = phoneLine.getPhoneCallList();
	if (phoneCallList.empty()) {
		return NULL;
	}

	QMenu * menu = new QMenu(tr("Invite to conference"));

	QString me = QString::fromStdString(_cPhoneCall.getPhoneCall().getPeerSipAddress().getUserName());

	PhoneLine::PhoneCallList::iterator it;
	for (it = phoneCallList.begin(); it != phoneCallList.end(); it++) {

		if ((*it)->getState() != EnumPhoneCallState::PhoneCallStateClosed) {

			QString str = QString::fromStdString((*it)->getPeerSipAddress().getUserName());
			if (str != me) {
				QAction * action = menu->addAction(str);
				SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(inviteToConference(bool)));
				menu->addAction(action);
			}
		}
	}
	return menu;
}

void QtPhoneCall::stateChangedEvent(EnumPhoneCallState::PhoneCallState state) {
 	std::string codecs;
	if (_cPhoneCall.getAudioCodecUsed() != CodecList::AudioCodecError) {
		codecs += CodecList::toString(_cPhoneCall.getAudioCodecUsed());
	}
	if (_cPhoneCall.getVideoCodecUsed() != CodecList::VideoCodecError) {
		codecs += "/" + CodecList::toString(_cPhoneCall.getVideoCodecUsed());
	}
	_qtWengoPhone->getQtStatusBar().showMessage(QString::fromStdString(codecs));


	switch (state) {
	case EnumPhoneCallState::PhoneCallStateUnknown:
		break;

	case EnumPhoneCallState::PhoneCallStateError:
		_ui->statusLabel->setText(tr("Error"));
		break;

	case EnumPhoneCallState::PhoneCallStateResumed:
		_actionResume->setEnabled(false);
		_actionHold->setEnabled(true);
		_ui->statusLabel->setText(tr("Talking"));
		_hold = true;
		break;

	case EnumPhoneCallState::PhoneCallStateTalking: {
		_duration = 0;
		_callTimer->start(1000);
		_actionAcceptCall->setEnabled(false);
		_actionHangupCall->setEnabled(true);
		_ui->statusLabel->setText(tr("Talking"));

		//FIXME Hack to put a modal window
		//this lets the user explain to the participants that
		//they are going to be put in a 3-way conf
		ConferenceCall * conferenceCall = _cPhoneCall.getPhoneCall().getConferenceCall();
		if (conferenceCall) {

			_ui->statusLabel->setText(tr("Talking - conference"));
			QMessageBox::question(
				_qtWengoPhone->getWidget(),
				tr("WengoPhone - Proceed Conference"),
				tr("When you are ready to start the conference, click on the button below\n"
				"Please wait for a few seconds...")
				/*tr("&Proceed conference"), tr("&Cancel"),
				QString(), 0, 1)*/);
				_cPhoneCall.hold();
		}
		break;
	}

	case EnumPhoneCallState::PhoneCallStateDialing:
		_actionAcceptCall->setEnabled(false);
		_actionHangupCall->setEnabled(true);
		_ui->statusLabel->setText(tr("Dialing"));
		break;

	case EnumPhoneCallState::PhoneCallStateRinging:
		_actionAcceptCall->setEnabled(false);
		_actionHangupCall->setEnabled(true);
		_ui->statusLabel->setText(tr("Ringing"));
		break;

	case EnumPhoneCallState::PhoneCallStateClosed:
		LOG_FATAL("should never reach this case since PPhoneCall::close() is done for this purpose");
		break;

	case EnumPhoneCallState::PhoneCallStateIncoming:
		_actionAcceptCall->setEnabled(true);
		_actionHangupCall->setEnabled(true);
		_ui->statusLabel->setText(tr("Incoming Call"));
		break;

	case EnumPhoneCallState::PhoneCallStateHold:
		_actionHold->setEnabled(false);
		_actionResume->setEnabled(true);
		_ui->statusLabel->setText(tr("Hold"));
		_hold = false;
		break;

	case EnumPhoneCallState::PhoneCallStateMissed:
		//Do nothing since QtPhoneCall is already destroyed
		//via PhoneCallStateClosed
		break;

	case EnumPhoneCallState::PhoneCallStateRedirected:
		_ui->statusLabel->setText(tr("Redirected"));
		break;

	default:
		LOG_FATAL("unknown PhoneCallState=" + EnumPhoneCallState::toString(state));
	}
}

void QtPhoneCall::videoFrameReceivedEvent(piximage * remoteVideoFrame, piximage * localVideoFrame) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
#ifdef XV_HWACCEL
	if (!_videoWindow) {
		_remoteVideoFrame = remoteVideoFrame;
		_localVideoFrame = localVideoFrame;

		_showVideo = true;
		if (config.getXVideoEnable()) {
			_videoWindow = new QtVideoXV(_phoneCallWidget, remoteVideoFrame->width, remoteVideoFrame->height,
				localVideoFrame->width, localVideoFrame->height);
			//Fallback if XV is not available
			if (!_videoWindow->isInitialized()) {
				OWSAFE_DELETE(_videoWindow);
				_videoWindow = new QtVideoQt(_phoneCallWidget);
				SAFE_CONNECT(_videoWindow, SIGNAL(toggleFlipVideoImageSignal()), SLOT(toggleFlipVideoImage()));
			}
		} else {
			_videoWindow = new QtVideoQt(_phoneCallWidget);
			SAFE_CONNECT(_videoWindow, SIGNAL(toggleFlipVideoImageSignal()), SLOT(toggleFlipVideoImage()));
		}
	}
#else
	if (!_videoWindow) {
		_remoteVideoFrame = remoteVideoFrame;
		_localVideoFrame = localVideoFrame;

		_showVideo = true;
		_videoWindow = new QtVideoQt(_phoneCallWidget);
		SAFE_CONNECT(_videoWindow, SIGNAL(toggleFlipVideoImageSignal()), SLOT(toggleFlipVideoImage()));
	}
#endif
	showVideoWidget();
	_videoWindow->showImage(remoteVideoFrame, localVideoFrame);
}

void QtPhoneCall::acceptActionTriggered(bool) {
	_ui->statusLabel->setText(tr("Initialization..."));
	_cPhoneCall.accept();
}

void QtPhoneCall::rejectActionTriggered(bool) {
	LOG_DEBUG("phone call hangup");
	switch (_cPhoneCall.getState()) {
	case EnumPhoneCallState::PhoneCallStateResumed:
	case EnumPhoneCallState::PhoneCallStateTalking:
	case EnumPhoneCallState::PhoneCallStateDialing:
	case EnumPhoneCallState::PhoneCallStateRinging:
	case EnumPhoneCallState::PhoneCallStateIncoming:
	case EnumPhoneCallState::PhoneCallStateHold:
	case EnumPhoneCallState::PhoneCallStateRedirected:
		_cPhoneCall.hangUp();
		break;
	default:
		LOG_DEBUG("call rejected");
		//delete _phoneCallWidget;
		//callRejected(); // Close the widget
	}
}

void QtPhoneCall::acceptCall() {
	acceptActionTriggered(true);
}

void QtPhoneCall::rejectCall() {
	rejectActionTriggered(true);
}

void QtPhoneCall::holdResumeActionTriggered(bool) {
	if (_hold) {
		_cPhoneCall.hold();
	} else {
		_cPhoneCall.resume();
	}
}

void QtPhoneCall::addContactActionTriggered(bool) {
	CWengoPhone & cWengoPhone = _cPhoneCall.getCWengoPhone();
	if (cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		//This method should not be called if no UserProfile has been set
		std::string callAddress = _cPhoneCall.getPeerDisplayName();

		if (callAddress.empty()) {
			callAddress = _cPhoneCall.getPeerUserName();
		}

		/*QtAddContact * qtAddContact = new QtAddContact(*_cPhoneCall.getCWengoPhone().getCUserProfile(), _phoneCallWidget, callAddress);
		LOG_DEBUG("add contact=" + callAddress);*/
	}
}

void QtPhoneCall::openPopup(int x, int y) {
	QMenu * menu = createInviteMenu();
	if (menu) {
		_actionInvite->setMenu(menu);
		_popupMenu->exec(QPoint(x, y));
		_actionInvite->setMenu(NULL);
		_actionInvite->setEnabled(true);
		OWSAFE_DELETE(menu);
	} else {
		_popupMenu->exec(QPoint(x, y));
	}
}

void QtPhoneCall::updateCallDuration() {
	if (!_ui->durationLabel) {
		return;
	}
	_duration++;
	QTime time;
	time = time.addSecs(_duration);

	_ui->durationLabel->setText(time.toString(Qt::TextDate));
}

void QtPhoneCall::showVideoWidget() {

	if (!_videoWindow->getWidget()) {
		return;
	}

	QGridLayout * layout = qobject_cast < QGridLayout * > (_phoneCallWidget->layout());
	if (!layout) {
		return;
	}

	//Removes the avatar from the widget
	layout->removeWidget(_ui->avatarLabel);
	_ui->avatarLabel->hide();

	//Inserts the video widget
	_videoWindow->getWidget()->setParent(_phoneCallWidget);
	_videoWindow->getWidget()->setMaximumSize(QSize(210, 160));
	_videoWindow->getWidget()->setMinimumSize(QSize(210, 160));

	layout->addWidget(_videoWindow->getWidget(), 0, 0);

	_videoWindow->getWidget()->show();
}

void QtPhoneCall::showAvatar() {
	QGridLayout * layout = dynamic_cast < QGridLayout * > (_phoneCallWidget->layout());

	//Removes the video widget
	layout->removeWidget(_videoWindow->getWidget());
	_videoWindow->getWidget()->hide();

	//Inserts the avatar label
	layout->addWidget(_ui->avatarLabel, 0, 0);
	_ui->avatarLabel->show();
}

void QtPhoneCall::inviteToConference(bool) {
	//startConference(this);
	QObject * source = sender();

	QAction * action = dynamic_cast < QAction * > (source);

	QString addCall = action->text();

	//Search the call
	PhoneLine & phoneLine = dynamic_cast < PhoneLine & > (_cPhoneCall.getPhoneCall().getPhoneLine());

	PhoneLine::PhoneCallList phoneCallList = phoneLine.getPhoneCallList();

	PhoneLine::PhoneCallList::iterator it;

	for (it = phoneCallList.begin(); it != phoneCallList.end(); it++) {

		if ((*it)->getState() != EnumPhoneCallState::PhoneCallStateClosed) {
			PhoneCall & me = _cPhoneCall.getPhoneCall();
			PhoneCall * add = (*it);
			startConference(& me, add);
			QString str = QString::fromStdString((*it)->getPeerSipAddress().getUserName());
			if (str == addCall) {

			}
		}
	}
}

bool QtPhoneCall::isIncoming() const {
	return (_cPhoneCall.getState() == EnumPhoneCallState::PhoneCallStateIncoming);
}

void QtPhoneCall::showToaster(const QString & userName) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if (!isIncoming()) {
		return;
	}

	//Shows toaster for incoming chats?
	if (!config.getNotificationShowToasterOnIncomingCall()) {
		return;
	}

	QtCallToaster * toaster = new QtCallToaster();
	toaster->setMessage(userName);
	SAFE_CONNECT(toaster, SIGNAL(pickUpButtonClicked()), SLOT(acceptCall()));
	SAFE_CONNECT(toaster, SIGNAL(hangUpButtonClicked()), SLOT(rejectCall()));
	toaster->show();
}

void QtPhoneCall::close() {
	_qtWengoPhone->getQtStatusBar().showMessage(QString::null);
	_ui->durationLabel = NULL;
	_callTimer->disconnect();
	_callTimer->stop();
	OWSAFE_DELETE(_callTimer);

	_actionAcceptCall->setEnabled(false);
	_actionHangupCall->setEnabled(false);
	_ui->statusLabel->setText(tr("Closed"));
	//stopConference();
	if (_videoWindow) {
		if (_videoWindow->isFullScreen()) {
			_videoWindow->unFullScreen();
		}
	}
	//TODO: disconnect from flipWebcamButtonClicked
	OWSAFE_DELETE(_videoWindow);
	OWSAFE_DELETE(_phoneCallWidget);
	deleteMe(this);
	callRejected();
}

void QtPhoneCall::toggleFlipVideoImage() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	PhoneLine & phoneLine = dynamic_cast < PhoneLine & > (_cPhoneCall.getPhoneCall().getPhoneLine());
	bool flip = !config.getVideoFlipEnable();
	phoneLine.flipVideoImage(flip);
	config.set(Config::VIDEO_ENABLE_FLIP, flip);
}
