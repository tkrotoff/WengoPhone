/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "StatusBar.h"

#include "Softphone.h"
#include "systray/Systray.h"
#include "MainWindow.h"
#include "HomePageWidget.h"
#include "connect/Connect.h"

#include <qmainwindow.h>
#include <qstatusbar.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qpalette.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qtooltip.h>

StatusBar::StatusBar(QMainWindow * mainWindow)
: QObject(mainWindow) {
	_statusBar = mainWindow->statusBar();
	
	_firstRegister = true;
	
	//Background color for the status bar
	_statusBar->setPaletteBackgroundColor(QColor(239, 238, 241));

	//Uses a minimum of space
	int stretch = 0;

	_connectionLabel = new QLabel(_statusBar);
	_statusBar->addWidget(_connectionLabel, stretch, true);
	setConnectionStatus(false);

	_updateLabel = new QLabel(_statusBar);
	_statusBar->addWidget(_updateLabel, stretch, true);
	//By default WengoPhone is up-to-date
	setUpToDateStatus(true);

	_sipLabel = new QLabel(_statusBar);
	_statusBar->addWidget(_sipLabel, stretch, true);
	setSipStatus(false);

	_audioLabel = new QLabel(_statusBar);
	_statusBar->addWidget(_audioLabel, stretch, true);
	setAudioStatus(true);

	_statusBar->setSizeGripEnabled(true);
}

StatusBar::~StatusBar() {
}

QStatusBar * StatusBar::getWidget() const {
	return _statusBar;
}

void StatusBar::setConnectionStatus(bool status) {
	_isConnectionEnabled = status;

	if (status) {
		_connectionLabel->setPixmap(QPixmap::fromMimeSource("statusbar_connect.png"));
		QToolTip::add(_connectionLabel, tr("Your Internet connection is working properly"));
	} else {
		_connectionLabel->setPixmap(QPixmap::fromMimeSource("statusbar_connect_error.png"));
		QToolTip::add(_connectionLabel, tr("Your Internet connection is not working properly"));
	}
	changeSystrayIcon();
}

bool StatusBar::isConnectionEnabled() const {
	return _isConnectionEnabled;
}

void StatusBar::setUpToDateStatus(bool status) {
	_isUpToDate = status;

	if (status) {
		setConnectionStatus(true);
		_updateLabel->setPixmap(QPixmap::fromMimeSource("statusbar_uptodate.png"));
		QToolTip::add(_updateLabel, tr("The last version of Wengo is installed"));
	} else {
		_updateLabel->setPixmap(QPixmap::fromMimeSource("statusbar_uptodate_error.png"));
		QToolTip::add(_updateLabel, tr("Please install the last version of Wengo"));
	}
	changeSystrayIcon();
}

bool StatusBar::isUpToDate() const {
	return _isUpToDate;
}

void StatusBar::setSipStatus(bool status) {
	_isSipEnabled = status;

	if (status) {
		setConnectionStatus(true);
		if (Connect::isHttpTunnelEnabled()) {
			_sipLabel->setPixmap(QPixmap::fromMimeSource("statusbar_sip_httptunnel.png"));
			QToolTip::add(_sipLabel, tr("Your are authentified on the Wengo platform via HTTP"));
			_statusBar->message("Connected using HTTP tunnel");
		} else {
			_sipLabel->setPixmap(QPixmap::fromMimeSource("statusbar_sip.png"));
			QToolTip::add(_sipLabel, tr("Your are authentified on the Wengo platform"));
			_statusBar->message("Connected");
		}
#ifdef WIN32
		Softphone & softphone = Softphone::getInstance();
		MainWindow & mainWindow = softphone.getMainWindow();
		if(_firstRegister) {
			_firstRegister = false;
			mainWindow.getHomePageWidget()->goToSoftphoneWeb(true);
		}
#endif
	} else {
		_sipLabel->setPixmap(QPixmap::fromMimeSource("statusbar_sip_error.png"));
		QToolTip::add(_sipLabel, tr("Your are not authentified on the Wengo platform"));
		_statusBar->message(tr("WENGOPHONE IS NOT PROPERLY CONFIGURED"));
	}
	//changeSystrayIcon();
}

bool StatusBar::isSipEnabled() const {
	return _isSipEnabled;
}

void StatusBar::setAudioStatus(bool status) {
	_isAudioEnabled = status;

	if (status) {
		_audioLabel->setPixmap(QPixmap::fromMimeSource("statusbar_audio.png"));
		QToolTip::add(_audioLabel, tr("Your audio system is correctly configured"));
	} else {
		_audioLabel->setPixmap(QPixmap::fromMimeSource("statusbar_audio_error.png"));
		QToolTip::add(_audioLabel, tr("Your audio system is not configured properly"));
	}
	changeSystrayIcon();
}

bool StatusBar::isAudioEnabled() const {
	return _isAudioEnabled;
}

void StatusBar::changeSystrayIcon() const {
	if (isConnectionEnabled() && isUpToDate() && isSipEnabled() && isAudioEnabled()) {
		Softphone::getInstance().getSystray().setToDefault();
	} else {
		Softphone::getInstance().getSystray().setToConfigError();
	}
}

void StatusBar::setMessage(const QString & message) {
	_statusBar->message(message);
}
