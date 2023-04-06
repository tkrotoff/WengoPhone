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

#include "CodecList.h"

#include "Codec.h"

#include <qstringlist.h>

#include <algorithm>
#include <cassert>
#include <iostream>
using namespace std;

CodecList::CodecList() {
}

CodecList::~CodecList() {
	//Deletes all the contacts
	_codecList.clear();
}

void CodecList::loadDefaultCodecList() {
	//Deletes all the contacts
	_codecList.clear();
	//addCodec(Codec(CODEC_AMR_WB, 0));
	//addCodec(Codec(CODEC_AMR_NB, 1));
	//addCodec(Codec(CODEC_ILBC, 2));
	//addCodec(Codec(CODEC_ALAW, 3));
	//addCodec(Codec(CODEC_ULAW, 4));
	//addCodec(Codec(CODEC_GSM, 5));
	//addCodec(Codec(CODEC_SPEEX, 6));
	sort(_codecList.begin(), _codecList.end());
}

void CodecList::addCodec(const Codec & codec) {
	_codecList.push_back(codec);
}

Codec * CodecList::getCodec(const QString & codecName) {
	for (unsigned int i = 0; i < _codecList.size(); i++) {
		if (_codecList[i].getName() == codecName) {
			return &(_codecList[i]);
		}
	}
	return NULL;
}

unsigned int CodecList::size() const {
	return _codecList.size();
}

const Codec & CodecList::operator[] (unsigned int i) const {
	return _codecList[i];
}

Codec & CodecList::operator[] (unsigned int i) {
	return _codecList[i];
}

QString CodecList::toString() {
	sort(_codecList.begin(), _codecList.end());
	QStringList codecNameList;
	for (unsigned int i = 0; i < _codecList.size(); i++) {
		cerr << "CodecList: " << _codecList[i].getName() << _codecList[i].getPriority() << endl;
		codecNameList += _codecList[i].getName();
	}
	return codecNameList.join(",");
}
