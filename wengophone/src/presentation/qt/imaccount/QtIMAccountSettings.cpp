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

#include "QtIMAccountPlugin.h"
#include "QtMSNSettings.h"
/*#include "QtAIMSettings.h"
#include "QtYahooSettings.h"
#include "QtJabberSettings.h"*/

#include <model/profile/UserProfile.h>

#include <util/Logger.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

#include <QtGui>

QLayout * createLayout2(QWidget * parent) {
	QGridLayout * layout = new QGridLayout(parent);
	layout->setSpacing(0);
	layout->setMargin(0);
	return layout;
}

QDialog * createWindowFromWidget2(QWidget * widget) {
	QDialog * dialog = new QDialog(widget->parentWidget());
	widget->setParent(NULL);
	createLayout2(dialog)->addWidget(widget);
	return dialog;
}


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
	_imAccountTemplateWidget = WidgetFactory::create(":/forms/imaccount/IMAccountTemplate.ui", parent);

	//settingsGroupBox
	QGroupBox * settingsGroupBox = Object::findChild<QGroupBox *>(_imAccountTemplateWidget, "settingsGroupBox");

	//imAccountTemplateWindow
	QDialog * imAccountTemplateWindow = createWindowFromWidget2(_imAccountTemplateWidget);
	connect(imAccountTemplateWindow, SIGNAL(accepted()), SLOT(saveIMAccountSettings()));

	//saveButton
	QPushButton * saveButton = Object::findChild<QPushButton *>(_imAccountTemplateWidget, "saveButton");
	connect(saveButton, SIGNAL(clicked()), imAccountTemplateWindow, SLOT(accept()));

	//cancelButton
	QPushButton * cancelButton = Object::findChild<QPushButton *>(_imAccountTemplateWidget, "cancelButton");
	connect(cancelButton, SIGNAL(clicked()), imAccountTemplateWindow, SLOT(reject()));

	switch (imProtocol) {
	case EnumIMProtocol::IMProtocolMSN: {
		_imAccountPlugin = new QtMSNSettings(_userProfile, _imAccount, settingsGroupBox);
		break;
	}

	case EnumIMProtocol::IMProtocolYahoo: {
		/*QtYahooSettings * yahooSettings = new QtYahooSettings(_userProfile, _imAccount, settingsGroupBox);
		imProtocolWidget = yahooSettings->getWidget();*/
		break;
	}

	case EnumIMProtocol::IMProtocolAIMICQ: {
		/*QtAIMSettings * aimSettings = new QtAIMSettings(_userProfile, _imAccount, settingsGroupBox);
		imProtocolWidget = aimSettings->getWidget();*/
		break;
	}

	case EnumIMProtocol::IMProtocolJabber: {
		/*QtJabberSettings * jabberSettings = new QtJabberSettings(_userProfile, _imAccount, settingsGroupBox);
		imProtocolWidget = jabberSettings->getWidget();*/
		break;
	}

	default:
		LOG_FATAL("unknown IM protocol=" + String::fromNumber(imProtocol));
	}

	QWidget * imProtocolWidget = _imAccountPlugin->getWidget();
	createLayout2(settingsGroupBox)->addWidget(imProtocolWidget);
	settingsGroupBox->setTitle(imProtocolWidget->windowTitle());
	imAccountTemplateWindow->setWindowTitle(imProtocolWidget->windowTitle());
	imAccountTemplateWindow->exec();
}

void QtIMAccountSettings::saveIMAccountSettings() {
	if (!_imAccountPlugin) {
		LOG_FATAL("_imAccountPlugin cannot be NULL");
	}

	_imAccountPlugin->save();
}
