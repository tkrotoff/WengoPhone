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

#include "QtProfileBar.h"

#include "QtIMProfileWidget.h"
#include "QtEventWidget.h"
#include "QtCreditWidget.h"

#include <presentation/qt/QtUserProfilePresenceMenu.h>
#include <presentation/qt/profile/QtUserProfile.h>
#include <presentation/qt/profile/QtProfileDetails.h>

#include <control/CWengoPhone.h>
#include <control/history/CHistory.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <model/account/wengo/WengoAccount.h>
#include <model/presence/PresenceHandler.h>
#include <model/profile/AvatarList.h>
#include <model/profile/UserProfile.h>

#include <imwrapper/IMAccount.h>

#include <util/Logger.h>

#include <qtutil/WengoStyleLabel.h>
#include <qtutil/SafeConnect.h>

QtProfileBar::QtProfileBar(CWengoPhone & cWengoPhone, CUserProfile & cUserProfile,
		ConnectHandler & connectHandler, QWidget * parent)
	: QWidget(parent),
	_cUserProfile(cUserProfile),
	_cWengoPhone(cWengoPhone),
	_connectHandler(connectHandler) {

	_isOpen = false;
	_nickNameWidgetVisible = false;
	_eventsWidgetVisible = false;
	_creditWidgetVisible = false;

	_widgetLayout = new QGridLayout(this);
	_widgetLayout->setMargin(0);
	_widgetLayout->setSpacing(0);

	_gridlayout = new QGridLayout();
	_gridlayout->setMargin(0);
	_gridlayout->setSpacing(0);

	_widgetLayout->addLayout(_gridlayout, 0, 0);

	//The status widget
	_statusLabel = new WengoStyleLabel(this);
	_statusLabel->setToolTip(tr("Click here to change your status"));
	_statusLabel->setMinimumSize(QSize(46, 65));
	_statusLabel->setMaximumSize(QSize(46, 65));
	_statusLabel->setPixmaps(QPixmap(":/pics/profilebar/bar_start_status_green.png"),
					QPixmap(), //no end
					QPixmap(), //no fill

					QPixmap(":/pics/profilebar/bar_on_start_status_green.png"),
					QPixmap(),  //no end
					QPixmap()
					); //no fill

	//Nickname label
	_nicknameLabel = new WengoStyleLabel(this, WengoStyleLabel::Normal);
	_nicknameLabel->setMinimumSize(QSize(0, 65));
	_nicknameLabel->setMaximumSize(QSize(1000, 65));
	_nicknameLabel->setPixmaps(
					QPixmap(), //no start
					QPixmap(":/pics/profilebar/bar_separator.png"), //end
					QPixmap(":/pics/profilebar/bar_fill.png"),
					QPixmap(),  //no start
					QPixmap(":/pics/profilebar/bar_on_separator_left.png"),  //end
					QPixmap(":/pics/profilebar/bar_on_fill.png") //fill
					);

	//The events label
	/*
	_eventsLabel = new WengoStyleLabel(this);
	_eventsLabel->setMinimumSize(QSize(46, 65));
	_eventsLabel->setMaximumSize(QSize(1000, 65));
	_eventsLabel->setPixmaps(QPixmap(), //no start
					QPixmap(":/pics/profilebar/bar_separator.png"), //end
					QPixmap(":/pics/profilebar/bar_fill.png"),
					QPixmap(),  //no start
					QPixmap(":/pics/profilebar/bar_on_separator_left.png"),  //end
					QPixmap(":/pics/profilebar/bar_on_fill.png")
					);
	*/

	//The credit label
	_creditLabel = new WengoStyleLabel(this);
	_creditLabel->setMinimumSize(QSize(46, 65));
	_creditLabel->setMaximumSize(QSize(1000, 65));
	_creditLabel->setPixmaps(QPixmap(), //no start
					QPixmap(":/pics/profilebar/bar_end.png"),
					QPixmap(":/pics/profilebar/bar_fill.png"),
					QPixmap(),  //no start
					QPixmap(":/pics/profilebar/bar_on_end.png"),
					QPixmap(":/pics/profilebar/bar_on_fill.png")
					);

	//Add the labels to the gridlayout
	_gridlayout->addWidget(_statusLabel, 0, 0);
	_gridlayout->addWidget(_nicknameLabel, 0, 1);
	//_gridlayout->addWidget(_eventsLabel, 0, 2);
	_gridlayout->addWidget(_creditLabel, 0, 3);

	//_statusLabel->setText("S");
	_nicknameLabel->setText("  " + tr("NickName"));
	_nicknameLabel->setTextColor(Qt::white);

//	_eventsLabel->setText(tr("events"));
//	_eventsLabel->setTextColor(Qt::white);

	_creditLabel->setText(QString() + QChar(0x20ac) + QString(" 0.00"));
	_creditLabel->setTextColor(Qt::white);

	//Objects connection
	SAFE_CONNECT(_statusLabel, SIGNAL(clicked()), SLOT(statusClicked()));
	SAFE_CONNECT(_nicknameLabel, SIGNAL(clicked()), SLOT(nicknameClicked()));
//	SAFE_CONNECT(_eventsLabel, SIGNAL(clicked()), SLOT(eventsClicked()));
	SAFE_CONNECT(_creditLabel, SIGNAL(clicked()), SLOT(creditClicked()));

	//resize
	setMinimumSize(QSize(120, 65));

	//create internal widgets
	_qtImProfileWidget = new QtIMProfileWidget(_cUserProfile, _cWengoPhone, this);
	_qtImProfileWidget->getWidget()->setVisible(false);

	_eventWidget = new QtEventWidget(_cWengoPhone, _cUserProfile, this);
	//_eventWidget->getWidget()->setVisible(false);

	_creditWidget = new QtCreditWidget(_cWengoPhone, _cUserProfile.getUserProfile(), this);

	_cUserProfile.getUserProfile().wsInfoCreatedEvent +=
		boost::bind(&QtProfileBar::wsInfoCreatedEventHandler, this, _1, _2);

	_cUserProfile.getUserProfile().phoneLineCreatedEvent +=
		boost::bind(&QtProfileBar::phoneLineCreatedEventHandler, this, _1, _2);

	_connectHandler.connectedEvent +=
		boost::bind(&QtProfileBar::connectedEventHandler,this,_1,_2);

	_connectHandler.connectionProgressEvent +=
		boost::bind(&QtProfileBar::connectionProgressEventHandler, this, _1, _2, _3, _4, _5);

	_connectHandler.disconnectedEvent +=
		boost::bind(&QtProfileBar::disconnectedEventHandler, this, _1, _2, _3, _4);

	PresenceHandler & presence = _cUserProfile.getUserProfile().getPresenceHandler();

	presence.myPresenceStatusEvent +=
		boost::bind(&QtProfileBar::myPresenceStatusEventHandler, this, _1, _2, _3);

	SAFE_CONNECT_RECEIVER_TYPE(this, SIGNAL(connectedEventSignal(IMAccount *)),
		_qtImProfileWidget, SLOT(connected(IMAccount *)), Qt::QueuedConnection);

	SAFE_CONNECT_RECEIVER_TYPE(this, SIGNAL(disconnectedEventSignal(IMAccount *, bool, const QString &)),
		_qtImProfileWidget, SLOT(disconnected(IMAccount *, bool, const QString &)), Qt::QueuedConnection);

	SAFE_CONNECT_RECEIVER_TYPE(this, SIGNAL(connectionProgressEventSignal(IMAccount *, int, int, const QString &)),
		_qtImProfileWidget, SLOT(connectionProgress(IMAccount *, int, int, const QString &)), Qt::QueuedConnection);

	SAFE_CONNECT_RECEIVER_TYPE(this, SIGNAL(myPresenceStatusEventSignal(QVariant)),
		this, SLOT(myPresenceStatusEventSlot(QVariant)), Qt::QueuedConnection);

	if (_cUserProfile.getCHistory()) {
		_eventWidget->setMissedCall(_cUserProfile.getCHistory()->getUnseenMissedCalls());
	}

	SAFE_CONNECT_TYPE(this, SIGNAL(wsInfoWengosEvent(float)),
		SLOT(wsInfoWengosEventSlot(float)), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(wsInfoVoiceMailEvent(int)),
		SLOT(wsInfoVoiceMailEventSlot(int)), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(wsInfoLandlineNumberEvent(const QString &)),
		SLOT(wsInfoLandlineNumberEventSlot(const QString &)), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(wsCallForwardInfoEvent(const QString &)),
		SLOT(wsCallForwardInfoEventSlot(const QString &)), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(phoneLineCreatedEvent()),
		SLOT(phoneLineCreatedEventSlot()), Qt::QueuedConnection);

	QtUserProfile * qtUserProfile = ((QtUserProfile *) (_cUserProfile.getPresentation()));
	/*SAFE_CONNECT_TYPE(qtUserProfile, SIGNAL(cHistoryCreatedEventHandlerSignal()),
		SLOT(cHistoryCreatedEventHandlerSlot()), Qt::QueuedConnection);*/

	//Check if events already occured
	//FIXME: must not use model class
	if (_cUserProfile.getUserProfile().getActivePhoneLine()) {
		phoneLineCreatedEventHandler(_cUserProfile.getUserProfile(), *_cUserProfile.getUserProfile().getActivePhoneLine());
	}

	if (_cUserProfile.getUserProfile().getWsInfo()) {
		wsInfoCreatedEventHandler(_cUserProfile.getUserProfile(), *_cUserProfile.getUserProfile().getWsInfo());
	}
	////
}

QtProfileBar::~QtProfileBar() {
}

//Called in the model thread
void QtProfileBar::connectedEventHandler(ConnectHandler & sender, IMAccount & imAccount) {
	IMAccount * pImAccount = &imAccount;
	connectedEventSignal(pImAccount);
}

//Called in the model thread
void QtProfileBar::disconnectedEventHandler (ConnectHandler & sender, IMAccount & imAccount,
	bool connectionError, const std::string & reason) {

	IMAccount * pImAccount = &imAccount;
	disconnectedEventSignal(pImAccount, connectionError, QString::fromStdString(reason));
}

void QtProfileBar::connectionProgressEventHandler(ConnectHandler & sender, IMAccount & imAccount,
			int currentStep, int totalSteps, const std::string & infoMessage) {

	IMAccount * pImAccount = &imAccount;
	connectionProgressEventSignal(pImAccount, currentStep, totalSteps, QString::fromStdString(infoMessage));
}

void QtProfileBar::myPresenceStatusEventHandler(PresenceHandler & sender,
	const IMAccount & imAccount, EnumPresenceState::MyPresenceStatus status) {

	EnumPresenceState::MyPresenceStatus * pstatus = new EnumPresenceState::MyPresenceStatus;
	QVariant v(status);
	myPresenceStatusEventSignal(v);
}

void QtProfileBar::myPresenceStatusEventSlot(QVariant status) {

	if (status.toInt() == (int) EnumPresenceState::MyPresenceStatusOk) {

		switch(_cUserProfile.getUserProfile().getPresenceState()) {
		case EnumPresenceState::PresenceStateAway:
			setAway();
			break;
		case EnumPresenceState::PresenceStateOnline:
			setOnline();
			break;
		case EnumPresenceState::PresenceStateInvisible:
			setInvisible();
			break;
		case EnumPresenceState::PresenceStateDoNotDisturb:
			setDoNotDisturb();
			break;
		default:
			LOG_DEBUG("Change presence state display to -- Not yet handled\n");
			break;
		}
	}
}

void QtProfileBar::statusClicked() {
	createStatusMenu();
}

void QtProfileBar::nicknameClicked() {
	if (_nickNameWidgetVisible) {
		removeNickNameWidget();
	}
	else {
		_nicknameLabel->setSelected(true);
//		_eventsLabel->setSelected(false);
		_creditLabel->setSelected(false);
		showNickNameWidget();
	}
}

void QtProfileBar::showNickNameWidget() {
	removeEventsWidget();
	removeCreditWidget();

	if (!_nickNameWidgetVisible) {
		QWidget * parentWidget = qobject_cast<QWidget *>(parent());
		QSize widgetSize = parentWidget->size();
		_qtImProfileWidget->getWidget()->resize(widgetSize.width(), 150);
		_widgetLayout->addWidget(_qtImProfileWidget->getWidget(), 1, 0);
		_qtImProfileWidget->getWidget()->setVisible(true);
		_nickNameWidgetVisible=true;
		setOpen(true);
	}
}

void QtProfileBar::removeNickNameWidget() {
	if (_nickNameWidgetVisible) {
		_qtImProfileWidget->getWidget()->setVisible(false);
		_widgetLayout->removeWidget(_qtImProfileWidget->getWidget());

		setMinimumSize(QSize(0, 0));
		resize(QSize(0, 0));
		_nickNameWidgetVisible=false;
		_nicknameLabel->setSelected(false);
		setOpen(false);
	}
}

void QtProfileBar::eventsClicked() {
	if (_eventsWidgetVisible) {
		removeEventsWidget();
	} else {
		_nicknameLabel->setSelected(false);
//		_eventsLabel->setSelected(true);
		_creditLabel->setSelected(false);
		showEventsWidget();
	}
}

void QtProfileBar::showEventsWidget() {

	removeNickNameWidget();
	removeCreditWidget();
	if (!_eventsWidgetVisible) {
		QWidget * parentWidget = qobject_cast<QWidget *>(parent());
		QSize widgetSize = parentWidget->size();
		_eventWidget->getWidget()->resize(widgetSize.width(), 150);
		_widgetLayout->addWidget(_eventWidget->getWidget(), 1, 0);
		_eventWidget->getWidget()->setVisible(true);
		_eventsWidgetVisible = true;
		setOpen(true);
	}
}

void QtProfileBar::removeEventsWidget() {
	if (_eventsWidgetVisible) {
		_eventWidget->getWidget()->setVisible(false);
		_widgetLayout->removeWidget(_eventWidget->getWidget());
		setMinimumSize(QSize(0, 0));
		resize(QSize(0, 0));
		_eventsWidgetVisible = false;
//		_eventsLabel->setSelected(false);
		setOpen(false);
	}
}

void QtProfileBar::creditClicked() {
	if (_creditWidgetVisible) {
		removeCreditWidget();
	} else {
		_nicknameLabel->setSelected(false);
		//_eventsLabel->setSelected(false);
		_creditLabel->setSelected(true);
		showCreditWidget();
	}
}

void QtProfileBar::showCreditWidget() {
	removeNickNameWidget();
	removeEventsWidget();
	if (!_creditWidgetVisible) {
		QWidget * parentWidget = qobject_cast<QWidget *>(parent());
		QSize widgetSize = parentWidget->size();
		_creditWidget->getWidget()->resize(widgetSize.width(), 150);
		_widgetLayout->addWidget(_creditWidget->getWidget(), 1, 0);
		_creditWidget->getWidget()->setVisible(true);
		_creditWidgetVisible = true;
		setOpen(true);
	}
}

void QtProfileBar::removeCreditWidget() {
	if (_creditWidgetVisible) {
		_creditWidget->getWidget()->setVisible(false);
		_widgetLayout->removeWidget(_creditWidget->getWidget());
		setMinimumSize(QSize(0, 0));
		resize(QSize(0, 0));
		_creditWidgetVisible = false;
		_creditLabel->setSelected(false);
		setOpen(false);
	}
}

void QtProfileBar::createStatusMenu() {
	bool connected = _cUserProfile.getUserProfile().isConnected();
	QtUserProfilePresenceMenu * menu = new QtUserProfilePresenceMenu(EnumPresenceState::PresenceStateUnknown, connected, QString::null, this);

	SAFE_CONNECT(menu, SIGNAL(onlineClicked()), SLOT(onlineClicked()));

	SAFE_CONNECT(menu, SIGNAL(doNotDisturbClicked()), SLOT(doNotDisturbClicked()));

	SAFE_CONNECT(menu, SIGNAL(invisibleClicked()), SLOT(invisibleClicked()));

	SAFE_CONNECT(menu, SIGNAL(awayClicked()), SLOT(awayClicked()));

	SAFE_CONNECT(menu, SIGNAL(disconnectClicked()), SLOT(disconnectClicked()));

	//SAFE_CONNECT(menu, SIGNAL(connectClicked()), SLOT(connectClicked()));
	QPoint p = _statusLabel->pos();
	p.setY(p.y() + _statusLabel->rect().bottom() - 18);
	menu->popup(mapToGlobal(p));
}

void QtProfileBar::onlineClicked() {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateOnline, NULL);
}

void QtProfileBar::setOnline() {
	setStatusLabel(":/pics/profilebar/bar_start_status_green.png",
		":/pics/profilebar/bar_on_start_status_green.png");
}

void QtProfileBar::doNotDisturbClicked() {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateDoNotDisturb, NULL);
}

void QtProfileBar::setDoNotDisturb() {
	setStatusLabel(":/pics/profilebar/bar_start_status_red.png",
		":/pics/profilebar/bar_on_start_status_red.png");
}

void QtProfileBar::invisibleClicked() {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateInvisible, NULL);
}

void QtProfileBar::setInvisible() {
	setStatusLabel(":/pics/profilebar/bar_start_status_gray.png",
		":/pics/profilebar/bar_on_start_status_gray.png");
}

void QtProfileBar::awayClicked() {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateAway, NULL);
}

void QtProfileBar::disconnectClicked() {
	_cUserProfile.getUserProfile().disconnect();
}

void QtProfileBar::setAway() {
	setStatusLabel(":/pics/profilebar/bar_start_status_orange.png",
		":/pics/profilebar/bar_on_start_status_orange.png");
}

void QtProfileBar::wsInfoCreatedEventHandler(UserProfile & sender, WsInfo & wsInfo) {
	wsInfo.wsInfoWengosEvent += boost::bind(&QtProfileBar::wsInfoWengosEventHandler, this, _1, _2, _3, _4);
	wsInfo.wsInfoVoiceMailEvent += boost::bind(&QtProfileBar::wsInfoVoiceMailEventHandler, this, _1, _2, _3, _4);
	wsInfo.wsInfoLandlineNumberEvent += boost::bind(&QtProfileBar::wsInfoLandlineNumberEventHandler, this, _1, _2, _3, _4);
	wsInfo.wsCallForwardInfoEvent += boost::bind(&QtProfileBar::wsCallForwardInfoEventHandler, this, _1, _2, _3, _4, _5, _6, _7, _8);

	wsInfo.getWengosCount(true);
	wsInfo.getUnreadVoiceMail(true);
	wsInfo.getCallForwardInfo(true);
	wsInfo.getLandlineNumber(true);
	wsInfo.execute();
}

void QtProfileBar::wsInfoWengosEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status, float wengos) {
	if (status == WsInfo::WsInfoStatusOk) {
		wsInfoWengosEvent(wengos);
	}
}

void QtProfileBar::wsInfoVoiceMailEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status, int voicemail) {
	if (status == WsInfo::WsInfoStatusOk) {
		wsInfoVoiceMailEvent(voicemail);
	}
}

void QtProfileBar::wsInfoLandlineNumberEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status, std::string number) {
	if (status == WsInfo::WsInfoStatusOk) {
		wsInfoLandlineNumberEvent(QString::fromStdString(number));
	}
}

void QtProfileBar::wsCallForwardInfoEventHandler(WsInfo & sender, int id, WsInfo::WsInfoStatus status,
	WsInfo::WsInfoCallForwardMode mode, bool voicemail, std::string dest1, std::string dest2, std::string dest3) {

	if (status == WsInfo::WsInfoStatusOk) {

		switch(mode) {
		case WsInfo::WsInfoCallForwardModeVoicemail:
			wsCallForwardInfoEvent(tr("active") + " (" + tr("voicemail") + ")");
			break;
		case WsInfo::WsInfoCallForwardModeNumber:
			wsCallForwardInfoEvent(tr("active") + " (" + QString::fromStdString(dest1) + ")");
			break;
		case WsInfo::WsInfoCallForwardModeDisabled:
			wsCallForwardInfoEvent(tr("inactive"));
			break;
		case WsInfo::WsInfoCallForwardModeUnauthorized:
			wsCallForwardInfoEvent(tr("unauthorized"));
			break;
		}
	}
}

void QtProfileBar::setOpen(bool status) {
	_isOpen = status;
	update();
}

void QtProfileBar::setStatusLabel(const QString & on, const QString & off) {

	_statusLabel->setPixmaps(QPixmap(on),
					QPixmap(), //no end
					QPixmap(), //no fill
					QPixmap(off),
					QPixmap(),  //no end
					QPixmap()
					); //no fill
	_statusLabel->update();
}

void QtProfileBar::phoneLineCreatedEventHandler(UserProfile & sender, IPhoneLine & phoneLine) {
	phoneLineCreatedEvent();
}

void QtProfileBar::phoneLineCreatedEventSlot() {
	_nicknameLabel->setText("  " + QString::fromStdString(_cUserProfile.getUserProfile().getWengoAccount()->getIdentity()));
}

void QtProfileBar::paintEvent(QPaintEvent * event) {
	if (_isOpen) {
		QRect r = rect();

		QLinearGradient lg(QPointF(1, r.top()), QPointF(1, r.bottom()));

		lg.setColorAt(0, palette().color(QPalette::Window));
		QColor dest = palette().color(QPalette::Window);

		float red = ((float) dest.red()) / 1.3f;
		float blue = ((float) dest.blue()) / 1.3f;
		float green = ((float) dest.green()) / 1.3f;

		dest = QColor((int) red, (int) green, (int) blue);
		lg.setColorAt(1, dest);

		QPainter painter(this);
		painter.fillRect(r, QBrush(lg));
		painter.end();
	} else {
		QWidget::paintEvent(event);
	}
}

void QtProfileBar::cHistoryCreatedEventHandlerSlot() {
	//FIXME CHistory API changed
	/*_cUserProfile.getCHistory()->unseenMissedCallsChangedEvent +=
		boost::bind(&QtProfileBar::unseenMissedCallsChangedEventHandler, this, _1, _2);*/

	_eventWidget->setMissedCall(_cUserProfile.getCHistory()->getUnseenMissedCalls());
}

void QtProfileBar::unseenMissedCallsChangedEventHandler(CHistory &, int count) {
	_eventWidget->setMissedCall(count);
}

void QtProfileBar::wsInfoWengosEventSlot(float wengos) {
	//0x20ac is the unicode code for the euros currency symbol
	_creditLabel->setText(QString() + QChar(0x20ac) + QString(" %1").arg(wengos));
}

void QtProfileBar::wsInfoVoiceMailEventSlot(int count) {
	_eventWidget->setVoiceMail(count);
}

void QtProfileBar::wsInfoLandlineNumberEventSlot(const QString & number) {
	_creditWidget->setLandlineNumber(number);
}

void QtProfileBar::wsCallForwardInfoEventSlot(const QString & mode) {
	_creditWidget->setCallForwardMode(mode);
}
