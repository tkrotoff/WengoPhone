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

#include "Update.h"

#include "Softphone.h"
#include "StatusBar.h"
#include "config/Config.h"

#include <qprocess.h>
#include <qmessagebox.h>

#include <iostream>
using namespace std;

Update::Update() {
}

void Update::setUpdate(const QString & updateUrl, bool updateCompulsery, const QString & /*updateName*/) {
	_updateUrl = updateUrl;

	//FIXME to remove
	_updateCompulsery = true;

	_downloadProc = NULL;
}

void Update::cancel() {
	if (_updateCompulsery) {
		//Wengophone is not up to date since a compulsery update is available
		Softphone::getInstance().getStatusBar().setUpToDateStatus(false);
	}
}

void Update::update() {
	if (_updateCompulsery) {
		//Wengophone is not up to date since a compulsery update is available
		Softphone::getInstance().getStatusBar().setUpToDateStatus(false);
	}

	QStringList downloadApp;
	downloadApp += "download";
	downloadApp += "-style=" + Config::getInstance().getStyle();
	downloadApp += _updateUrl;
	downloadApp += "update.exe";
	downloadApp += Softphone::getInstance().getLanguagesDir() + Config::getInstance().getLanguage();

	_downloadProc = new QProcess();
	_downloadProc->setArguments(downloadApp);
	connect(_downloadProc, SIGNAL(processExited()),
		this, SLOT(updateDownloadFinished()));

	if (!_downloadProc->start()) {
		//Error handling
		QMessageBox::critical(Softphone::getInstance().getActiveWindow(),
				tr("Update failed"),
				tr("The program download failed to start"));
	}
}

Update::~Update() {
	if (_downloadProc != NULL) {
		delete _downloadProc;
	}
}

void Update::updateDownloadFinished() {
	if (!_downloadProc->normalExit()) {
		//Do nothing: the user cancelled the download
		return;
	}

	QProcess * proc = new QProcess();
	proc->setArguments(QStringList("update.exe"));
	if (proc->start()) {
		//FIXME Exits without calling disconnect, it does not work otherwise
		Softphone::getInstance().exit(false);
	} else {
		//Error handling
		QMessageBox::critical(Softphone::getInstance().getActiveWindow(),
				tr("Update failed"),
				tr("Wengophone update failed to start: try to update Wengophone manually"));
	}
}
