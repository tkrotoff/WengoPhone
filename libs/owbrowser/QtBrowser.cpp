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

#include "QtBrowser.h"

#include <util/Logger.h>

#include <QtGui>

#if (defined OS_WINDOWS) && (defined QT_COMMERCIAL)
#include <QAxWidget>
#endif

#include <iostream>
using namespace std;

QtBrowser::QtBrowser(QWidget * parent, BrowserMode mode) : QObject() {
	_browserWidget = new QWidget(parent);

#if (defined OS_WINDOWS) && (defined QT_COMMERCIAL)
	_ieBrowser = NULL;
#endif
	_qtBrowser = NULL;

	_layout = new QVBoxLayout(_browserWidget);
	_layout->setMargin(1);
	_layout->setSpacing(0);

	setMode(mode);
}

void QtBrowser::setUrl(const std::string & url) {
	LOG_DEBUG(url);
	_url = QString::fromStdString(url);
	if (_mode == IEMODE) {
#if (defined OS_WINDOWS) && (defined QT_COMMERCIAL)
		_ieBrowser->dynamicCall("Navigate(const QString&)", _url);
#endif
	} else {
		_qtBrowser->setSource(_url);
	}
}

void QtBrowser::show() {
	_browserWidget->show();
}

void * QtBrowser::getWidget() const {
	return (void *) _browserWidget;
}

std::string QtBrowser::getUrl() const {
	return _url.toStdString();
}

void QtBrowser::backward() {
	LOG_FATAL("Not yet implemented");
}

void QtBrowser::forward() {
	LOG_FATAL("Not yet implemented");
}

void QtBrowser::beforeNavigate(const QUrl & link) {
	urlClickedEvent(link.toString().toStdString());
}

void QtBrowser::beforeNavigate(const QString & url, int, const QString &, const QVariant &, const QString &, bool &) {
	urlClickedEvent(url.toStdString());
}

bool QtBrowser::setMode(BrowserMode mode) {
	//IEMODE is only available on Windows & Qt commercial
#if (defined OS_WINDOWS) && (!defined QT_COMMERCIAL)
	if (mode == IEMODE) {
		_mode = QTMODE;
		initBrowser();
		return false;
	}
#endif
	_mode = mode;
	initBrowser();
	return true;
}

void QtBrowser::initBrowser() {
#if (defined OS_WINDOWS) && (defined QT_COMMERCIAL)
	if (_mode == IEMODE) {
		LOG_FATAL("IEMODE not allowed");
	}
#endif

	if (_mode == QTMODE) {
#if (defined OS_WINDOWS) && (defined QT_COMMERCIAL)
		//clean ie browser
		if (_ieBrowser) {
			_layout->removeWidget(_ieBrowser);
			delete _ieBrowser;
			_ieBrowser = NULL;
		}
#endif

		//Init Qt browser
		_qtBrowser = new QTextBrowser(_browserWidget);
		connect(_qtBrowser, SIGNAL(anchorClicked(const QUrl &)),
			SLOT(beforeNavigate(const QUrl &)));
		_layout->addWidget(_qtBrowser);

	} else {
#if (defined OS_WINDOWS) && (defined QT_COMMERCIAL)
		//clean qt browser
		if (_qtBrowser) {
			_layout->removeWidget(_qtBrowser);
			delete _qtBrowser;
			_qtBrowser = NULL;
		}

		//Init IE browser
		_ieBrowser = new QAxWidget(_browserWidget);
		_ieBrowser->setControl(QString::fromUtf8("{8856F961-340A-11D0-A96B-00C04FD705A2}"));
		_ieBrowser->setObjectName(QString::fromUtf8("mwbAx"));
		_ieBrowser->setFocusPolicy(Qt::StrongFocus);
		connect(_ieBrowser, SIGNAL(BeforeNavigate(const QString &, int, const QString &, const QVariant &, const QString &, bool &)),
			SLOT(beforeNavigate(const QString &, int, const QString &, const QVariant &, const QString &, bool &)));
		_layout->addWidget(_ieBrowser);
#endif
	}
}
