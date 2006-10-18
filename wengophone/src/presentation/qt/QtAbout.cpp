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

#include <curl/curl.h>
#ifndef OS_MACOSX
	#include <portaudio.h>
#endif
#include <boost/version.hpp>
#include <avcodec.h>
#include <tinyxml.h>
extern "C" {
#include <glib.h>
#include <gaim/core.h>
}

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
				"Boost: " + QString(BOOST_LIB_VERSION) + eol +
				"Gaim: " + QString(gaim_core_get_version()) + eol +
				"cURL: " + QString(curl_version()) + eol +
				"TinyXML: " + QString::number(TIXML_MAJOR_VERSION) + "." + QString::number(TIXML_MINOR_VERSION) + "." + QString::number(TIXML_PATCH_VERSION) + eol +
				"FFmpeg's libavcodec: " + QString::number(avcodec_version())
#ifndef OS_MACOSX
				+ eol + "PortAudio: " + QString(Pa_GetVersionText())
#endif
				);
}
