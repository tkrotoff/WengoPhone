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

#include "AboutWindow.h"

#include "MyWidgetFactory.h"
#include "Softphone.h"
#include "update/CheckUpdate.h"
#include "config/Config.h"

#include <WebBrowser.h>

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstring.h>

#include <iostream>
using namespace std;

const int AboutWindow::TIMER_TIMEOUT = 200;

AboutWindow::AboutWindow(QWidget * parent) : QObject(parent) {
	_aboutWindow = (QDialog *) MyWidgetFactory::create("AboutWindowForm.ui", this, parent);
	
	//WengoPhone build Id String
	QLabel * wengoBuildIdStringLabel = (QLabel *) _aboutWindow->child("wengoBuildIdStringLabel", "QLabel");
	wengoBuildIdStringLabel->setText("WengoPhone " + Softphone::BUILD_ID_STRING);
	
	//WengoPhone internal build Id
	QLabel * wengoBuildIdLabel = (QLabel *) _aboutWindow->child("wengoBuildIdLabel", "QLabel");
	wengoBuildIdLabel->setText(QString::number(Softphone::BUILD_ID));
	
	//Updates
	Config & conf = Config::getInstance();
	const QString langCode = conf.getLanguageCode();
#ifdef WIN32
	_checkUpdate = new CheckUpdate(Softphone::BUILD_ID, "nsiswin32", langCode);
#endif
	//Check update button
	QPushButton * checkUpdateButton = (QPushButton *) _aboutWindow->child("checkUpdateButton", "QPushButton");
	connect(checkUpdateButton, SIGNAL(clicked()),
		this, SLOT(checkUpdate()));
	
	//Translator Credits
	QPushButton *creditsButton = (QPushButton *) _aboutWindow->child("creditsButton", "QPushButton");
	connect(creditsButton, SIGNAL(clicked()),
		this, SLOT(displayTranslatorsCredits()));

	_timer = new QTimer(this);
	_timer->start(TIMER_TIMEOUT);
	connect(_timer, SIGNAL(timeout()),
		this, SLOT(updateAboutText()));
}

AboutWindow::~AboutWindow() {
}

void AboutWindow::updateAboutText() {
	if (CheckUpdate::getLastBuildIdAvailable() != 0) {
		QString buildId = QString::number(CheckUpdate::getLastBuildIdAvailable());

		cerr << "AboutWindow: last build Id available: " << buildId << endl;

		//Last WengoPhone version number available
		QLabel * lastBuildIdLabel = (QLabel *) _aboutWindow->child("lastBuildIdLabel", "QLabel");
		lastBuildIdLabel->setText(buildId);

		_timer->stop();
		delete _timer;
	}
}

void AboutWindow::checkUpdate() {
#ifdef WIN32
	_checkUpdate->checkUpdate();
#endif
}

void AboutWindow::displayTranslatorsCredits() const {
  WebBrowser::openUrl("http://dev.openwengo.com/trac/openwengo/trac.cgi/wiki/Credits");
}
