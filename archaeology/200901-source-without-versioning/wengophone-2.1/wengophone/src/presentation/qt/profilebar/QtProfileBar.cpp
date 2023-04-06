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

#include "QtProfileBar.h"

#include "QtIMProfileWidget.h"
#include "QtEventWidget.h"
#include "QtCreditWidget.h"

#include <presentation/qt/QtIMAccountHelper.h>
#include <presentation/qt/QtIMAccountMonitor.h>
#include <presentation/qt/QtPresenceMenuManager.h>
#include <presentation/qt/QtPresencePixmapHelper.h>
#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/profile/QtUserProfile.h>
#include <presentation/qt/profile/QtProfileDetails.h>

#include <control/CWengoPhone.h>
#include <control/history/CHistory.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <model/account/wengo/WengoAccount.h>
#include <model/connect/ConnectHandler.h>
#include <model/profile/AvatarList.h>
#include <model/profile/UserProfile.h>

#include <imwrapper/IMAccount.h>

#include <util/Logger.h>

#include <qtutil/WengoStyleLabel.h>
#include <qtutil/SafeConnect.h>

#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include <QtGui/QMenu>
#include <QtGui/QPainter>
#include <QtCore/QMetaType>

static const char* STATUS_LABEL_OFF_TEMPLATE = ":/pics/profilebar/bar_start_status_%1.png";
static const char* STATUS_LABEL_ON_TEMPLATE = ":/pics/profilebar/bar_on_start_status_%1.png";

// These constants are used to find the position of the state "ball" in the
// profilebar pixmap
static const int STATUS_LABEL_CENTERX = 26;
static const int STATUS_LABEL_CENTERY = 32;
static const int STATUS_LABEL_RADIUS  = 10;

// Defines position of the expand indicator in the picture. used in
// createPixmapWithExpandIndicator
static const int EXPAND_INDICATOR_OFFSET = 4;


/**
 * This helper functions blit an expand indicator (or contract if @expand is
 * false) over the pixmap whose name is @name
 */
static QPixmap createPixmapWithExpandIndicator(const QString& name, bool expand) {
	QPixmap bg(":/pics/profilebar/" + name);

	QString indicatorName;
	if (expand) {
		indicatorName = "indicator_expand.png";
	} else {
		indicatorName = "indicator_contract.png";
	}
	QPixmap indicator(":/pics/profilebar/" + indicatorName);

	QPainter painter(&bg);

	int top = (bg.height() - indicator.height()) / 2;
	painter.drawPixmap(EXPAND_INDICATOR_OFFSET, top, indicator);

	return bg;
}


QtProfileBar::QtProfileBar(CWengoPhone & cWengoPhone, CUserProfile* cUserProfile, QtIMAccountMonitor* qtIMAccountMonitor, QWidget * parent)
	: QWidget(parent),
	_cUserProfile(cUserProfile),
	_cWengoPhone(cWengoPhone),
	_qtImAccountMonitor(qtIMAccountMonitor) {

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
	_statusLabel->setFixedSize(QSize(46, 65));

	//Nickname label
	_nicknameLabel = new WengoStyleLabel(this, WengoStyleLabel::Normal);
	_nicknameLabel->setPixmaps(
					QPixmap(), //no start
					createPixmapWithExpandIndicator("bar_separator.png", true), //end
					QPixmap(":/pics/profilebar/bar_fill.png"), //fill
					QPixmap(),  //no start
					createPixmapWithExpandIndicator("bar_on_separator_left.png", false), //end
					QPixmap(":/pics/profilebar/bar_on_fill.png") //fill
					);

	//The credit label
	_creditLabel = new WengoStyleLabel(this);
	_creditLabel->setPixmaps(QPixmap(), //no start
					createPixmapWithExpandIndicator("bar_end.png", true),
					QPixmap(":/pics/profilebar/bar_fill.png"),
					QPixmap(),  //no start
					createPixmapWithExpandIndicator("bar_on_end.png", false),
					QPixmap(":/pics/profilebar/bar_on_fill.png")
					);

	//Add the labels to the gridlayout
	_gridlayout->addWidget(_statusLabel, 0, 0);
	_gridlayout->addWidget(_nicknameLabel, 0, 1);
	_gridlayout->addWidget(_creditLabel, 0, 2);

	//_statusLabel->setText("S");
	_nicknameLabel->setText("  " + tr("NickName"));
	_nicknameLabel->setTextColor(Qt::white);

	_creditLabel->setText(QString() + QChar(0x20ac) + QString(" 0.00"));
	_creditLabel->setTextColor(Qt::white);

	//Objects connection
	SAFE_CONNECT(_statusLabel, SIGNAL(clicked()), SLOT(statusClicked()));
	SAFE_CONNECT(_nicknameLabel, SIGNAL(clicked()), SLOT(nicknameClicked()));
	SAFE_CONNECT(_creditLabel, SIGNAL(clicked()), SLOT(creditClicked()));

	//resize
	setMinimumSize(QSize(120, 65));

	//create internal widgets
	_qtImProfileWidget = new QtIMProfileWidget(_cUserProfile, _cWengoPhone, _qtImAccountMonitor, this);
	_qtImProfileWidget->getWidget()->setVisible(false);
	_qtImProfileWidget->getWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	_widgetLayout->addWidget(_qtImProfileWidget->getWidget(), 1, 0);

	_creditWidget = new QtCreditWidget(_cWengoPhone, _cUserProfile->getUserProfile(), this);
	_creditWidget->getWidget()->setVisible(false);
	_creditWidget->getWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	_widgetLayout->addWidget(_creditWidget->getWidget(), 2, 0);

	//init label tracking vars
	_currentLabel = 0;
	_labelWidget[_nicknameLabel] = _qtImProfileWidget->getWidget();
	_labelWidget[_creditLabel] = _creditWidget->getWidget();
	////

	_cUserProfile->getUserProfile().wsInfoCreatedEvent +=
		boost::bind(&QtProfileBar::wsInfoCreatedEventHandler, this, _1, _2);

	_cUserProfile->getUserProfile().phoneLineCreatedEvent +=
		boost::bind(&QtProfileBar::phoneLineCreatedEventHandler, this, _1, _2);

	qRegisterMetaType<IMAccount>("IMAccount");

	SAFE_CONNECT_TYPE(_qtImAccountMonitor, SIGNAL(imAccountAdded(QString)),
		SLOT(updateStatusLabel()), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(_qtImAccountMonitor, SIGNAL(imAccountRemoved(QString)),
		SLOT(updateStatusLabel()), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(_qtImAccountMonitor, SIGNAL(imAccountUpdated(QString)),
		SLOT(updateStatusLabel()), Qt::QueuedConnection);

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


	//Check if events already occured
	//FIXME: must not use model class
	if (_cUserProfile->getUserProfile().getActivePhoneLine()) {
		phoneLineCreatedEventHandler(_cUserProfile->getUserProfile(), *_cUserProfile->getUserProfile().getActivePhoneLine());
	}

	if (_cUserProfile->getUserProfile().getWsInfo()) {
		wsInfoCreatedEventHandler(_cUserProfile->getUserProfile(), *_cUserProfile->getUserProfile().getWsInfo());
	}
	////
	updateStatusLabel();
}

QtProfileBar::~QtProfileBar() {
}

void QtProfileBar::updateStatusLabel() {
	// Create pixmap
	IMAccountList accountList = _cUserProfile->getUserProfile().getIMAccountManager().getIMAccountListCopy();
	QtPresencePixmapHelper helper(accountList);
	QPixmap off = helper.createPixmap(STATUS_LABEL_OFF_TEMPLATE, 
		STATUS_LABEL_CENTERX, 
		STATUS_LABEL_CENTERY,
		STATUS_LABEL_RADIUS);
	QPixmap on = helper.createPixmap(STATUS_LABEL_ON_TEMPLATE,
		STATUS_LABEL_CENTERX, 
		STATUS_LABEL_CENTERY,
		STATUS_LABEL_RADIUS);

	_statusLabel->setPixmaps(off,
					QPixmap(), //no end
					QPixmap(), //no fill
					on,
					QPixmap(),  //no end
					QPixmap()
					); //no fill
	_statusLabel->update();

	// Sort account using the same order as in the status pixmap
	QtIMAccountHelper::QtIMAccountPtrVector vector;
	QtIMAccountHelper::copyListToPtrVector(accountList, &vector);
	std::sort(vector.begin(), vector.end(), QtIMAccountHelper::compareIMAccountPtrs);
	////

	// Create tooltip
	QStringList tooltipLines;
	QtIMAccountHelper::QtIMAccountPtrVector::const_iterator
		it = vector.begin(),
		end = vector.end();
	for (;it!=end; ++it) {
		const IMAccount* imAccount = *it;
		QString login = QString::fromStdString(imAccount->getLogin());
		QString protocol = QString::fromStdString( EnumIMProtocol::toString(imAccount->getProtocol()) );
		QString id = QString::fromStdString(imAccount->getUUID());
		QtIMAccountMonitor::IMAccountInfoAutoPtr info = _qtImAccountMonitor->getIMAccountInfo(id);

		QString line =
			QString("<b>%1 (%2):</b> %3")
				.arg(login)
				.arg(protocol)
				.arg(info->message());
		tooltipLines.append(line);
	}
	_statusLabel->setToolTip(tooltipLines.join("<br>\n"));
}

void QtProfileBar::statusClicked() {

	// no presence when user got a SIPAccount without presence and no IMAccount
	if (_cUserProfile) {
	
		UserProfile & userProfile = _cUserProfile->getUserProfile();
		SipAccount * sipAccount = userProfile.getSipAccount();
		
		if (userProfile.getIMAccountManager().getIMAccountListCopy().size() <= 1) {
			if (!sipAccount || !sipAccount->isPIMEnabled()) {
				return;
			}
		}
	}
	////
	
	// by default we crete the menu
	createStatusMenu();
}

void QtProfileBar::nicknameClicked() {
	toggleLabel(_nicknameLabel);
}

void QtProfileBar::creditClicked() {
	toggleLabel(_creditLabel);
}

void QtProfileBar::toggleLabel(WengoStyleLabel* label) {
	setUpdatesEnabled(false);
	Q_ASSERT(label);
	if (_currentLabel) {
		// Hide current
		_currentLabel->setSelected(false);
		_labelWidget[_currentLabel]->hide();
	}

	if (label == _currentLabel) {
		// Clicked on current, no more _currentLabel
		_currentLabel = 0;
	} else {
		// Clicked on another label, there is a new _currentLabel
		label->setSelected(true);
		Q_ASSERT(_labelWidget.contains(label));
		
		//Make sure all internal widgets are at the same height
		int height = qMax(_qtImProfileWidget->getWidget()->minimumSizeHint().height(), _creditWidget->getWidget()->minimumSizeHint().height());
		_qtImProfileWidget->getWidget()->setFixedHeight(height);
		_creditWidget->getWidget()->setFixedHeight(height);
		
		_labelWidget[label]->show();
		_currentLabel = label;
	}

	setUpdatesEnabled(true);
}

void QtProfileBar::createStatusMenu() {
	QMenu* menu = new QMenu(this);
	QtPresenceMenuManager* manager = new QtPresenceMenuManager(menu, 
		&_cWengoPhone.getCUserProfileHandler(),
		dynamic_cast<QtWengoPhone*>(_cWengoPhone.getPresentation())
	);

	QAction* action = menu->addAction(tr("Set global presence to:"));
	action->setEnabled(false);
	manager->addPresenceActions(menu);

	QPoint p = _statusLabel->pos();
	p.setY(p.y() + _statusLabel->rect().bottom() - 18);
	menu->popup(mapToGlobal(p));
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

void QtProfileBar::phoneLineCreatedEventHandler(UserProfile & sender, IPhoneLine & phoneLine) {
	phoneLineCreatedEvent();
}

void QtProfileBar::phoneLineCreatedEventSlot() {
	_nicknameLabel->setText("  " + QString::fromStdString(_cUserProfile->getUserProfile().getSipAccount()->getDisplayName()));
}

void QtProfileBar::paintEvent(QPaintEvent * event) {
	if (_currentLabel) {
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

void QtProfileBar::wsInfoWengosEventSlot(float wengos) {
	//0x20ac is the unicode code for the euros currency symbol
	_creditLabel->setText(QString() + QChar(0x20ac) + QString(" %1").arg(wengos));
}

void QtProfileBar::wsInfoVoiceMailEventSlot(int count) {
	// FIXME: Show VoiceMail info somewhere
}

void QtProfileBar::wsInfoLandlineNumberEventSlot(const QString & number) {
	_creditWidget->setLandlineNumber(number);
}

void QtProfileBar::wsCallForwardInfoEventSlot(const QString & mode) {
	_creditWidget->setCallForwardMode(mode);
}
