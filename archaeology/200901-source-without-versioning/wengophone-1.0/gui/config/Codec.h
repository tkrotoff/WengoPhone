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

#ifndef CODEC_H
#define CODEC_H

#include <qstring.h>

/* Audio codecs */
const QString AUDIO_CODEC_ULAW = "PCMU";
const QString AUDIO_CODEC_ALAW = "PCMA";
const QString AUDIO_CODEC_ILBC = "ILBC";
const QString AUDIO_CODEC_GSM = "GSM";
const QString AUDIO_CODEC_AMR_NB = "AMR";
const QString AUDIO_CODEC_AMR_WB = "AMR-WB";

/* Video codecs */

const QString VIDEO_CODEC_H263 = "H263";
const QString VIDEO_CODEC_H264 = "H264";
const QString VIDEO_CODEC_MPEG4 = "MPEG4";


/**
 * Represents a Codec.
 *
 * @author Tanguy Krotoff
 */
class Codec {
public:

	Codec(const QString & name, int priority, const QString & description = QString::null);

	QString getName() const {
		return _name;
	}

	int getPriority() const {
		return _priority;
	}

	void setPriority(int priority) {
		_priority = priority;
	}

	QString getDescription() const {
		return _description;
	}

	/**
	 * Compares two Codec together.
	 *
	 * @param codec1 left part of the operator
	 * @param codec2 right part of the operator
	 * @return true if priority codec1 < priority codec2, false otherwise
	 */
	friend bool operator<(const Codec & codec1, const Codec & codec2) {
		return codec1._priority < codec2._priority;
	}

	friend bool operator>(const Codec & codec1, const Codec & codec2) {
		return codec1._priority > codec2._priority;
	}

private:

	/**
	 * Codec name.
	 */
	QString _name;

	/**
	 * Lower is the priority and more chances has the codec
	 * to be used.
	 */
	int _priority;

	/**
	 * Description of the Codec.
	 * For information purpose only, not mandatory.
	 */
	QString _description;
};

#endif	//CODEC_H
