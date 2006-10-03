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

#include "QtWebDirectory.h"

#include <model/webservices/url/WsUrl.h>

#include <qtutil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

QtWebDirectory::QtWebDirectory(QWidget * parent)
	: QtBrowser(parent) {

	QWidget * widget = (QWidget *) getWidget();
	widget->resize(715, 569);
	LANGUAGE_CHANGE(widget);
	init();
}

QtWebDirectory::~QtWebDirectory() {
}

void QtWebDirectory::raise() {
	QWidget * widget = (QWidget *) getWidget();
	widget->show();
	widget->raise();
}

void QtWebDirectory::init() {
	setUrl(WsUrl::getWengoDirectoryUrl());
	QWidget * widget = (QWidget *) getWidget();
	widget->setWindowTitle(tr("WengoPhone - Directory"));
}

void QtWebDirectory::languageChanged() {
	init();
}
