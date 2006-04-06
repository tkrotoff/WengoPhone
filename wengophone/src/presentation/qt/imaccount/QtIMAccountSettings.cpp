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

#include "QtMSNSettings.h"

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


QtIMAccountSettings::QtIMAccountSettings(QWidget * parent, EnumIMProtocol::IMProtocol imProtocol, EditMode mode)
	: QObject(parent) {

	_imAccountTemplateWidget = WidgetFactory::create(":/forms/imaccount/IMAccountTemplate.ui", parent);
	QGroupBox * settingsGroupBox = Object::findChild<QGroupBox *>(_imAccountTemplateWidget, "settingsGroupBox");
	QDialog * imAccountTemplateWindow = createWindowFromWidget2(_imAccountTemplateWidget);

	QWidget * imProtocolWidget = NULL;
	switch (imProtocol) {
	case EnumIMProtocol::IMProtocolMSN:
		QtMSNSettings(settingsGroupBox);
		imProtocolWidget = WidgetFactory::create(":/forms/imaccount/MSNSettings.ui", settingsGroupBox);
		break;
	case EnumIMProtocol::IMProtocolYahoo:
		settingsGroupBox->setTitle("Yahoo " + tr("Settings"));
		imProtocolWidget = WidgetFactory::create(":/forms/imaccount/YahooSettings.ui", settingsGroupBox);
		break;
	case EnumIMProtocol::IMProtocolAIMICQ:
		settingsGroupBox->setTitle("AIM/ICQ " + tr("Settings"));
		imProtocolWidget = WidgetFactory::create(":/forms/imaccount/AIMICQSettings.ui", settingsGroupBox);
		break;
	case EnumIMProtocol::IMProtocolJabber:
		settingsGroupBox->setTitle("Jabber " + tr("Settings"));
		imProtocolWidget = WidgetFactory::create(":/forms/imaccount/JabberSettings.ui", settingsGroupBox);
		break;
	default:
		LOG_FATAL("unknown IM protocol=" + String::fromNumber(imProtocol));
	}

	createLayout2(settingsGroupBox)->addWidget(imProtocolWidget);
	imAccountTemplateWindow->setWindowTitle(imProtocolWidget->windowTitle());
	imAccountTemplateWindow->exec();
}
