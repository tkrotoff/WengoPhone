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

#include "QtIMAccountSettings.h"

#include "ui_IMAccountTemplate.h"

#include "QtIMAccountPlugin.h"
#include "QtMSNSettings.h"
#include "QtAIMSettings.h"
#include "QtYahooSettings.h"
#include "QtJabberSettings.h"
#include "QtGoogleTalkSettings.h"

#include <model/profile/UserProfile.h>

#include <util/Logger.h>

#include <qtutil/Widget.h>

#include <QtGui>

QtIMAccountSettings::QtIMAccountSettings(UserProfile & userProfile, IMAccount * imAccount, QWidget * parent)
	: QObject(parent),
	_userProfile(userProfile) {

	if (!imAccount) {
		LOG_FATAL("imAccount cannot be NULL");
	}

	_imAccountPlugin = NULL;
	_imAccount = imAccount;
	createIMProtocolWidget(parent, imAccount->getProtocol());
}

QtIMAccountSettings::QtIMAccountSettings(UserProfile & userProfile, EnumIMProtocol::IMProtocol imProtocol, QWidget * parent)
	: QObject(parent),
	_userProfile(userProfile) {

	_imAccountPlugin = NULL;
	_imAccount = NULL;
	createIMProtocolWidget(parent, imProtocol);
}

void QtIMAccountSettings::createIMProtocolWidget(QWidget * parent, EnumIMProtocol::IMProtocol imProtocol) {
	QDialog * imAccountTemplateWindow = new QDialog(parent);

	_ui = new Ui::IMAccountTemplate();
	_ui->setupUi(imAccountTemplateWindow);

	//saveButton
	connect(_ui->saveButton, SIGNAL(clicked()), imAccountTemplateWindow, SLOT(accept()));

	//cancelButton
	connect(_ui->cancelButton, SIGNAL(clicked()), imAccountTemplateWindow, SLOT(reject()));

	switch (imProtocol) {
	case EnumIMProtocol::IMProtocolMSN: {
		_imAccountPlugin = new QtMSNSettings(_userProfile, _imAccount, _ui->settingsGroupBox);
		break;
	}

	case EnumIMProtocol::IMProtocolYahoo: {
		_imAccountPlugin = new QtYahooSettings(_userProfile, _imAccount, _ui->settingsGroupBox);
		break;
	}

	case EnumIMProtocol::IMProtocolAIMICQ: {
		_imAccountPlugin = new QtAIMSettings(_userProfile, _imAccount, _ui->settingsGroupBox);
		break;
	}

	case EnumIMProtocol::IMProtocolJabber: {
		_imAccountPlugin = new QtJabberSettings(_userProfile, _imAccount, _ui->settingsGroupBox);
		break;
	}

	case EnumIMProtocol::IMProtocolGoogleTalk: {
		_imAccountPlugin = new QtGoogleTalkSettings(_userProfile, _imAccount, _ui->settingsGroupBox);
		break;
	}

	default:
		LOG_FATAL("unknown IM protocol=" + String::fromNumber(imProtocol));
	}

	QWidget * imProtocolWidget = _imAccountPlugin->getWidget();
	Widget::createLayout(_ui->settingsGroupBox)->addWidget(imProtocolWidget);
	_ui->settingsGroupBox->setTitle(imProtocolWidget->windowTitle());

	connect(imAccountTemplateWindow, SIGNAL(accepted()), _imAccountPlugin, SLOT(save()));
	imAccountTemplateWindow->setWindowTitle(imProtocolWidget->windowTitle());
	imAccountTemplateWindow->exec();
}
