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

#include "QtIMProfileWidget.h"

#include "ui_IMProfileWidget.h"

#include "QtIMAccountPresenceMenu.h"

#include <presentation/qt/profile/QtProfileDetails.h>
#include <presentation/qt/imaccount/QtIMAccountManager.h>
#include <presentation/PFactory.h>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <model/profile/UserProfile.h>

#include <util/Logger.h>
#include <thread/ThreadEvent.h>

#include <qtutil/ToolTipLineEdit.h>

#include <QtGui/QtGui>

static const char * PICS_MSN_ON = ":pics/protocols/msn.png";
static const char * PICS_MSN_OFF = ":pics/protocols/msn_off.png";
static const char * PICS_MSN_ERROR = ":pics/protocols/msn_error.png";

static const char * PICS_YAHOO_ON = ":pics/protocols/yahoo.png";
static const char * PICS_YAHOO_OFF = ":pics/protocols/yahoo_off.png";
static const char * PICS_YAHOO_ERROR = ":pics/protocols/yahoo_error.png";

static const char * PICS_WENGO_ON = ":pics/protocols/wengo.png";
static const char * PICS_WENGO_OFF = ":pics/protocols/wengo_off.png";
static const char * PICS_WENGO_ERROR = ":pics/protocols/wengo_error.png";

static const char * PICS_AIM_ON = ":pics/protocols/aim.png";
static const char * PICS_AIM_OFF = ":pics/protocols/aim_off.png";
static const char * PICS_AIM_ERROR = ":pics/protocols/aim_error.png";

static const char * PICS_JABBER_ON = ":pics/protocols/jabber.png";
static const char * PICS_JABBER_OFF = ":pics/protocols/jabber_off.png";
static const char * PICS_JABBER_ERROR = ":pics/protocols/jabber_error.png";

QtIMProfileWidget::QtIMProfileWidget(CUserProfile & cUserProfile, CWengoPhone & cWengoPhone, QWidget * parent)
	: QObject(parent),
	_cUserProfile(cUserProfile),
	_cWengoPhone(cWengoPhone) {

	_imProfileWidget = new QWidget(parent);

	_ui = new Ui::IMProfileWidget();
	_ui->setupUi(_imProfileWidget);

	//Widget connections
	connect(_ui->msnButton, SIGNAL(clicked()), SLOT(msnClicked()));
	connect(_ui->yahooButton, SIGNAL(clicked()), SLOT(yahooClicked()));
	connect(_ui->wengoButton, SIGNAL(clicked()), SLOT(wengoClicked()));
	connect(_ui->aimButton, SIGNAL(clicked()), SLOT(aimClicked()));
	connect(_ui->jabberButton, SIGNAL(clicked()), SLOT(jabberClicked()));
	connect(_ui->aliasLineEdit, SIGNAL(returnPressed()), SLOT(aliasTextChanged()));
	connect(_ui->aliasLineEdit, SIGNAL(textChanged(const QString &)), SLOT(aliasTextChanged(const QString &)));
	connect(_ui->avatarButton, SIGNAL(clicked()), SLOT(changeAvatarClicked()));
	connect(_ui->editProfileButton, SIGNAL(clicked()), SLOT(editProfileClicked()));

	//UserProfile changed event connection
	_cUserProfile.getUserProfile().profileChangedEvent += boost::bind(&QtIMProfileWidget::profileChangedEventHandler, this);

	//Widget initialization
	init();
}

QtIMProfileWidget::~QtIMProfileWidget() {
	delete _ui;
}

QWidget * QtIMProfileWidget::getWidget() const {
	return _imProfileWidget;
}

void QtIMProfileWidget::connected(IMAccount * pImAccount) {
	EnumIMProtocol::IMProtocol imProtocol = pImAccount->getProtocol();

	switch (imProtocol) {
	case EnumIMProtocol::IMProtocolMSN:
		_ui->msnButton->setIcon(QPixmap(PICS_MSN_ON));
		_ui->msnButton->setToolTip("MSN");
		break;

	case EnumIMProtocol::IMProtocolWengo:
		_ui->wengoButton->setIcon(QPixmap(PICS_WENGO_ON));
		_ui->wengoButton->setToolTip("Wengo");
		break;

	case EnumIMProtocol::IMProtocolYahoo:
		_ui->yahooButton->setIcon(QPixmap(PICS_YAHOO_ON));
		_ui->yahooButton->setToolTip("Yahoo!");
		break;

	case EnumIMProtocol::IMProtocolAIMICQ:
		_ui->aimButton->setIcon(QPixmap(PICS_AIM_ON));
		_ui->aimButton->setToolTip("AIM/ICQ");
		break;

	case EnumIMProtocol::IMProtocolJabber:
		_ui->jabberButton->setIcon(QPixmap(PICS_JABBER_ON));
		_ui->jabberButton->setToolTip("Jabber/GoogleTalk");
		break;

	default:
		LOG_FATAL("unknown IM protocol=" + String::fromNumber(imProtocol));
	}
}

void QtIMProfileWidget::disconnected(IMAccount * pImAccount, bool connectionError, const QString & reason) {
	EnumIMProtocol::IMProtocol imProtocol = pImAccount->getProtocol();

	QString tooltip;
	QString pixmap;

	switch (imProtocol) {
	case EnumIMProtocol::IMProtocolMSN: {
		if (connectionError) {
			_ui->msnButton->setIcon(QPixmap(PICS_MSN_ERROR));
			_ui->msnButton->setToolTip("MSN Error: " + reason);
		} else {
			_ui->msnButton->setIcon(QPixmap(PICS_MSN_OFF));
			_ui->msnButton->setToolTip("MSN");
		}
		break;
	}

	case EnumIMProtocol::IMProtocolWengo: {
		if (connectionError) {
			_ui->wengoButton->setIcon(QPixmap(PICS_WENGO_ERROR));
			_ui->wengoButton->setToolTip("Wengo Error: " + reason);
		} else {
			_ui->wengoButton->setIcon(QPixmap(PICS_WENGO_OFF));
			_ui->wengoButton->setToolTip("Wengo");
		}
		break;
	}

	case EnumIMProtocol::IMProtocolYahoo: {
		if (connectionError) {
			_ui->yahooButton->setIcon(QPixmap(PICS_YAHOO_ERROR));
			_ui->yahooButton->setToolTip("Yahoo! Error: " + reason);
		} else {
			_ui->yahooButton->setIcon(QPixmap(PICS_YAHOO_OFF));
			_ui->yahooButton->setToolTip("Yahoo!");
		}
		break;
	}

	case EnumIMProtocol::IMProtocolAIMICQ: {
		if (connectionError) {
			_ui->aimButton->setIcon(QPixmap(PICS_AIM_ERROR));
			_ui->aimButton->setToolTip("AIM/ICQ Error: " + reason);
		} else {
			_ui->aimButton->setIcon(QPixmap(PICS_AIM_OFF));
			_ui->aimButton->setToolTip("AIM/ICQ");
		}
		break;
	}

	case EnumIMProtocol::IMProtocolJabber: {
		if (connectionError) {
			_ui->jabberButton->setIcon(QPixmap(PICS_JABBER_ERROR));
			_ui->jabberButton->setToolTip("Jabber/GoogleTalk Error: " + reason);
		} else {
			_ui->jabberButton->setIcon(QPixmap(PICS_JABBER_OFF));
			_ui->jabberButton->setToolTip("Jabber/GoogleTalk");
		}
		break;
	}

	default:
		LOG_FATAL("unknown IM protocol=" + String::fromNumber(imProtocol));
	}
}

void QtIMProfileWidget::connectionProgress(IMAccount * pImAccount,
	int currentStep, int totalSteps, const QString & infoMessage) {

	EnumIMProtocol::IMProtocol imProtocol = pImAccount->getProtocol();

	switch (imProtocol) {
	case EnumIMProtocol::IMProtocolMSN:
		_ui->msnButton->setIcon(QPixmap(PICS_MSN_ERROR));
		_ui->msnButton->setToolTip("MSN Info: " + infoMessage);
		break;

	case EnumIMProtocol::IMProtocolWengo:
		_ui->wengoButton->setIcon(QPixmap(PICS_WENGO_ERROR));
		_ui->wengoButton->setToolTip("Wengo Info: " + infoMessage);
		break;

	case EnumIMProtocol::IMProtocolYahoo:
		_ui->yahooButton->setIcon(QPixmap(PICS_YAHOO_ERROR));
		_ui->yahooButton->setToolTip("Yahoo! Info: " + infoMessage);
		break;

	case EnumIMProtocol::IMProtocolAIMICQ:
		_ui->aimButton->setIcon(QPixmap(PICS_AIM_ERROR));
		_ui->aimButton->setToolTip("AIM/ICQ Info: " + infoMessage);
		break;

	case EnumIMProtocol::IMProtocolJabber:
		_ui->jabberButton->setIcon(QPixmap(PICS_JABBER_ERROR));
		_ui->jabberButton->setToolTip("Jabber/GoogleTalk Info: " + infoMessage);
		break;

	default:
		LOG_FATAL("unknown IM protocol=" + String::fromNumber(imProtocol));
	}
}

void QtIMProfileWidget::imButtonClicked(EnumIMProtocol::IMProtocol imProtocol) {
	std::set<IMAccount *> list = _cUserProfile.getIMAccountsOfProtocol(imProtocol);
	if (!list.empty()) {
		showIMMenu(imProtocol);
	} else {
		showImAccountManager();
	}
}

void QtIMProfileWidget::showIMMenu(EnumIMProtocol::IMProtocol imProtocol) {
	QMenu * imMenu = new QMenu(_imProfileWidget);

	std::set<IMAccount *> list = _cUserProfile.getIMAccountsOfProtocol(imProtocol);

	for (std::set<IMAccount *>::const_iterator it = list.begin();
		it != list.end(); ++it) {
		QtIMAccountPresenceMenu * menu = new QtIMAccountPresenceMenu(_cUserProfile, *(*it), imMenu);
		imMenu->addMenu(menu);
	}

	/*QPoint p = _msnLabel->pos();
	p.setY(p.y() + _msnLabel->rect().bottom());
	_msnIMAccountMenu->popup(mapToGlobal(p));*/

	imMenu->exec(QCursor::pos());
}

void QtIMProfileWidget::msnClicked() {
	imButtonClicked(EnumIMProtocol::IMProtocolMSN);
}

void QtIMProfileWidget::yahooClicked() {
	imButtonClicked(EnumIMProtocol::IMProtocolYahoo);
}

void QtIMProfileWidget::wengoClicked() {
	imButtonClicked(EnumIMProtocol::IMProtocolWengo);
}

void QtIMProfileWidget::aimClicked() {
	imButtonClicked(EnumIMProtocol::IMProtocolAIMICQ);
}

void QtIMProfileWidget::jabberClicked() {
	imButtonClicked(EnumIMProtocol::IMProtocolJabber);
}

void QtIMProfileWidget::changeAvatarClicked() {
	QtProfileDetails qtProfileDetails(_cUserProfile, _cUserProfile.getUserProfile(), _imProfileWidget);
	//TODO UserProfile must be updated if QtProfileDetails was accepted
	qtProfileDetails.changeUserProfileAvatar();
	updateAvatar();
}

void QtIMProfileWidget::editProfileClicked() {
	QtProfileDetails qtProfileDetails(_cUserProfile, _cUserProfile.getUserProfile(), _imProfileWidget);
	//TODO UserProfile must be updated if QtProfileDetails was accepted
	qtProfileDetails.show();
	updateAvatar();
}

void QtIMProfileWidget::init() {
	if (!_cUserProfile.getUserProfile().getAlias().empty()) {
		_ui->aliasLineEdit->setText(QString::fromStdString(_cUserProfile.getUserProfile().getAlias()));
	}

	aliasTextChanged();

	updateAvatar();

	//Updates IMAccounts icons status
	for (IMAccountHandler::const_iterator it = _cUserProfile.getUserProfile().getIMAccountHandler().begin();
		it != _cUserProfile.getUserProfile().getIMAccountHandler().end(); it++) {
		if ((*it).isConnected()) {
			connected((IMAccount *) &(*it));
		} else {
			disconnected((IMAccount *) &(*it), false, QString::null);
		}
	}
}

void QtIMProfileWidget::updateAvatar() {
	QPixmap background = QPixmap(":/pics/avatar_background.png");
	std::string myData = _cUserProfile.getUserProfile().getIcon().getData();

	QPixmap pixmap;
	pixmap.loadFromData((uchar *) myData.c_str(), myData.size());

	if (!pixmap.isNull()) {
		QPainter painter(& background);
		painter.drawPixmap(5, 5, pixmap.scaled(60, 60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
		painter.end();
		_ui->avatarButton->setIcon(background);
	} else {
		_ui->avatarButton->setIcon(background);
	}
}

void QtIMProfileWidget::showImAccountManager() {
	QtIMAccountManager imAccountManager(_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile(),
		true, _imProfileWidget);
}

void QtIMProfileWidget::aliasTextChanged(const QString & text) {
	//Update alias text
	std::string alias(_ui->aliasLineEdit->text().toUtf8().constData());
	_cUserProfile.getUserProfile().setAlias(alias, NULL);

	//Update alias text color
	QPalette palette = _ui->aliasLineEdit->palette();
	palette.setColor(QPalette::Text, Qt::black);
	_ui->aliasLineEdit->setPalette(palette);

	_ui->aliasLineEdit->update();
}

void QtIMProfileWidget::profileChangedEventHandler() {
	typedef ThreadEvent0<void ()> MyThreadEvent;
	MyThreadEvent * event = new MyThreadEvent(boost::bind(&QtIMProfileWidget::init, this));
	PFactory::postEvent(event);
}
