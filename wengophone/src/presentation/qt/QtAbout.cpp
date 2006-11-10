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

#include "QtAbout.h"

#include "ui_AboutWindow.h"

#include <WengoPhoneBuildId.h>

#include <util/Logger.h>
#include <cutil/global.h>

#include <QtGui/QtGui>

QtAbout::QtAbout(QWidget * parent) {
	_aboutDialog = new QDialog(parent);

	_ui = new Ui::AboutWindow();
	_ui->setupUi(_aboutDialog);

	_ui->wengoPhoneBuildIdStringLabel->setText(WengoPhoneBuildId::SOFTPHONE_NAME + QString(" ") +
					WengoPhoneBuildId::VERSION +
					QString(" rev") + WengoPhoneBuildId::REVISION +
					QString("-") + QString::number(WengoPhoneBuildId::BUILDID));

	QFile file(":/data/AUTHORS");
	if (file.open(QFile::ReadOnly)) {
		QString authors = file.readAll();
		file.close();
		_ui->authorsTextEdit->setPlainText(authors);
	} else {
		LOG_ERROR("couldn't locate file=" + file.fileName().toStdString());
	}

	QString eol = QString::fromStdString(String::EOL);
	_ui->versionLabel->setText("Qt: " + QString(qVersion()) + eol +
				"Boost: " + QString(WengoPhoneBuildId::LIBBOOST_VERSION) + eol +
				"LibGaim: " + QString(WengoPhoneBuildId::GAIM_VERSION) + eol +
				"cURL: " + QString(WengoPhoneBuildId::CURL_VERSION) + eol +
				"TinyXML: " + QString(WengoPhoneBuildId::TINYXML_VERSION) + eol +
				"FFmpeg's libavcodec: " + QString(WengoPhoneBuildId::AVCODEC_VERSION)
#ifndef OS_MACOSX
				+ eol + "PortAudio: " + QString(WengoPhoneBuildId::PORTAUDIO_VERSION)
#endif
				);
}
