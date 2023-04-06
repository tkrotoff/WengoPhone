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

#include "Config.h"
#include "Codec.h"

#include "database/Database.h"
#include "database/FileTarget.h"
#include "Softphone.h"
#include "exception/FileNotFoundException.h"
#include "CodecList.h"
#include <observer/Event.h>

#include <qstring.h>
#include <qpalette.h>
#include <qcolor.h>
#include <qapplication.h>

#include <iostream>
using namespace std;

//TODO const QString Config::DOC_TYPE = Softphone::NAME;
const QString Config::DOC_TYPE = "Wengo";
const QString Config::STYLE_TAG = "style";
const QString Config::LANGUAGE_TAG = "lang";

Config::Config() {
	/* Audio and video codecs lists are in two different objects */
	_audioCodecList = new CodecList();
	_videoCodecList = new CodecList();

	/* The addition of codecs in the codecs lists is done here */

	_audioCodecList->addCodec(Codec(AUDIO_CODEC_AMR_WB, 0));
	_audioCodecList->addCodec(Codec(AUDIO_CODEC_ILBC, 1));
	_audioCodecList->addCodec(Codec(AUDIO_CODEC_ALAW, 2));
	_audioCodecList->addCodec(Codec(AUDIO_CODEC_ULAW, 3));
	_audioCodecList->addCodec(Codec(AUDIO_CODEC_AMR_NB, 4));
	_audioCodecList->addCodec(Codec(AUDIO_CODEC_GSM, 5));

	_videoCodecList->addCodec(Codec(VIDEO_CODEC_H263, 0));
	_videoCodecList->addCodec(Codec(VIDEO_CODEC_MPEG4, 1));
}

Config::~Config() {
	delete _audioCodecList;
	delete _videoCodecList;
}

void Config::saveAsXml() {
	using namespace database;

	Database & database = Database::getInstance();
	QString xml = database.transformConfigToXml(*this);

	FileTarget * target = NULL;
	try {
		target = new FileTarget(Softphone::getInstance().getGeneralConfigFile());
	} catch (const FileNotFoundException & e) {
		cerr << e.what() << endl;
		return;
	}

	database.load(xml, *target);
	delete target;
}

void Config::setStyle(const QString & style) {
	_style = style;
	QApplication::setStyle(_style);
}

void Config::setLanguage(const QString & language) {
	_language = language;
	QTranslator & translator = Softphone::getInstance().getTranslator();
	translator.load(_language, Softphone::getInstance().getLanguagesDir());
	notify(Event());
}

QString Config::getLanguageCode() {
	return QObject::tr("eng");
}
